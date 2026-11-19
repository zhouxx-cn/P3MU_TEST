


#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


static int ShellCmdOut(const char *cmd, char *result, int len) 
{  
    struct timeval now;
    char fileName[64] = {0};
    char command2[256] = {0};
    int ret = 0;
    FILE *fp = NULL;

    if(!cmd || !strlen(cmd) || len<=0)    
        return -1;

    gettimeofday(&now, NULL);

    sprintf(fileName, "/tmp/.ShellCmdOut%ld", now.tv_usec);

    memset(result, 0, len); 
    sprintf(command2, "%s > %s", cmd, fileName);

    system(command2);
    if ((fp = fopen(fileName, "r")) == NULL)
    {
        perror("fopen");
        ret = -1;
    }
    else
    {
        fread(result, sizeof(char), len-1, fp);
        
        if(fclose(fp))
        {
            perror("fclose");
            ret = -1;
        }
    }
    
    memset(command2, 0, sizeof(command2));
    sprintf(command2, "rm -rf %s", fileName);
    system(command2);
    
    return ret;                                
}



int main()
{
	char SYS_TIME_CFG[128] = "/mnt/mtd/config/system_time.cfg";
	int  Restart_time = 13;
	char buf[128] = {0};
	char command[128] = {0};
	int ret = 0;
	int pid = -1;


	
	time_t t_systime = time(NULL);
	
	t_systime += Restart_time;
	
	struct tm *tp = localtime(&t_systime);
	
	snprintf(command, sizeof(command), "ps | grep \"/mnt/app/scripts/record_system_time.sh\"  | grep -v grep | awk -F \" \" '{print $1}'");
	printf ("command %s\n",command);
	
	ret = ShellCmdOut(command, buf, 5);//PID最大为5位
	
	if(ret)
		printf("Not found grep /root/edvr/record_system_time.sh\n");
	else
	{
		pid = atoi(buf);
		if((pid > 100) && (pid <= 0x8000))//100是经验值，0x8000是linux最大pid号
		{
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "kill %d", pid);
			system(buf);
		}
		else
			printf("found grep /root/edvr/record_system_time.sh PID number is invalid\n");
	}
	
	snprintf(buf,sizeof(buf),"%04d-%02d-%02d %02d:%02d:%02d",tp->tm_year + 1900, tp->tm_mon+1, tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);
	
	printf("buf %s\n",buf);
	
	memset(command, 0,sizeof(command));
	
	
		
	snprintf(command, sizeof(command),"echo \\\"%s\\\"> %s",buf,SYS_TIME_CFG);
	
	printf("command, %s\n",command);
	
	system(command);
	
	return 0;
}
