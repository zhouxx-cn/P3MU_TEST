#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include "disk_shm.h"

static void rsleep(unsigned int ntime)
{
	fd_set rfds;

	struct timeval tv;

	int retval;					/* Watch stdin  (fd 0) to see when it has input. */

	FD_ZERO(&rfds);
	FD_SET(0, &rfds);			/* Wait up to five seconds. */
	tv.tv_sec = ntime / 1000000;
	tv.tv_usec = ntime % 1000000;
	retval = select(0, &rfds, NULL, NULL, &tv);	/* Don't rely on the value of tv now! */

	return;
}

int main()
{
	int index;

	struct system_shareinfo_t shareinfo;

	struct system_shminfo_t *pshminfo;

	struct sigaction sa;

/**
 * To resolve the zombie systemd process.
 */
	sa.sa_handler = SIG_IGN;
#ifdef SA_NOCLDWAIT
	sa.sa_flags = SA_NOCLDWAIT;
#else
	sa.sa_flags = 0;
#endif
	sigemptyset(&sa.sa_mask);
	sigaction(SIGCHLD, &sa, NULL);

/**
 *  Init the share memory and semaphone.
 */
	system_init(&shareinfo);

	while (1)
	{

		system_sem_in(&shareinfo);

		for (index = 0; index < MAX_SYSTEM_CMD_NUM; index++)
		{
			pshminfo = (struct system_shminfo_t *) (shareinfo.shm) + index;

			if (SYSTEM_READY == pshminfo->status)	//command ready, fork a child process to execute it.
			{
				pid_t pidparent;

				int statusparent;

				pshminfo->status = SYSTEM_EXECUTING;
				system_sem_out(shareinfo);

				if (0 == (pidparent = fork()))
				{
					pid_t pid;

					int cmdindex;

					int status;

					int ret = 0;

					int i;

					struct system_shareinfo_t shareinfochld;

					struct system_shminfo_t *pshminfochld;

					cmdindex = index;

					if (0 == (pid = fork()))
					{
						char cmdstr[1024];

						struct system_shareinfo_t shareinfograndchld;

						struct system_shminfo_t *pshminfograndchld;

						int grandindex = cmdindex;

						system_sem_in(&shareinfograndchld);
						pshminfograndchld = (struct system_shminfo_t *) (shareinfograndchld.shm) + grandindex;

						strncpy(cmdstr, pshminfograndchld->cmd, sizeof (cmdstr) - 1);
						cmdstr[sizeof (cmdstr) - 1] = '\0';
						#ifdef __PRINT_DEBUG__
						printf("\n[SYSTEMD] %s\n", cmdstr);
						#endif
						system_sem_out(shareinfograndchld);

						//ret=system(cmdstr);
						ret = execl("/bin/sh", "sh", "-c", cmdstr, (char *) 0);
						if (-1 != ret)
							ret = 0;

						system_sem_in(&shareinfograndchld);
						pshminfograndchld = (struct system_shminfo_t *) (shareinfograndchld.shm) + grandindex;
						pshminfograndchld->ret = ret;
						system_sem_out(shareinfograndchld);
						_exit(0);
					}

					i = 0;
					while (0 == waitpid(pid, &status, WNOHANG))	//check if the the time is out
					{

						if (i++ < SYSTEM_TIME_OUT)
						{
							rsleep(10000);	//Here, this sleep period cannot be too long, otherwise, mysystem cannot return immediately.
						}
						else
						{
							printf("\n[SYSTEMD]command index %d executing time out!\n", cmdindex);
							kill(pid, SIGKILL);
							waitpid(pid, &status, WNOHANG);

							system_sem_in(&shareinfochld);
							pshminfochld = (struct system_shminfo_t *) (shareinfochld.shm) + cmdindex;
							pshminfochld->timeout = 1;
							system_sem_out(shareinfochld);
							break;
						}
					}

					system_sem_in(&shareinfochld);
					pshminfochld = (struct system_shminfo_t *) (shareinfochld.shm) + cmdindex;
					pshminfochld->ret = ret;
					pshminfochld->status = SYSTEM_FINISHED;
					system_sem_out(shareinfochld);

					_exit(0);

				}				//childprocess over
				waitpid(pidparent, &statusparent, WNOHANG);

				break;
			}
		}

		if (MAX_SYSTEM_CMD_NUM == index)
		{
			system_sem_out(shareinfo);
		}

		//rsleep(10000);			//10ms
		usleep(15000);
	}

	return 0;
}
