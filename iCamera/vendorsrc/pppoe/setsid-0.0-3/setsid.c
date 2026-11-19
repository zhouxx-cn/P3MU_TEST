/*
 * setsid.c -- execute a command in a new session
 * Rick Sladkey <jrs@world.std.com>
 * In the public domain.
 *
 * 1999-02-22 Arkadiusz Mi¶kiewicz <misiek@pld.ORG.PL>
 * - added Native Language Support
 *
 * 2001-01-18 John Fremlin <vii@penguinpowered.com>
 * - fork in case we are process group leader
 *
 * 2003-09-18  Corinna Vinschen <corinna@vinschen.de>
 * - tweak for Cygwin (redirect stdio to /dev/null, register as service
 *   under 9x/Me)
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>
#ifndef __CYGWIN__
//## include "nls.h"
#else
# include <fcntl.h>
# include <windows.h>
# define setlocale(x, y)
# define bindtextdomain(x, y)
# define textdomain(x)
# define _(Text) (Text)
# define is_winnt	(GetVersion() < 0x80000000)
#endif

#ifdef __CYGWIN__
static void
register_9x_service(void) {
	HINSTANCE kerneldll;
	DWORD (*RegisterServiceProcess)(DWORD, DWORD);

	if (!(kerneldll = LoadLibrary("KERNEL32.DLL")))
		return;
	if (!(RegisterServiceProcess = (DWORD (*)(DWORD, DWORD))
		GetProcAddress(kerneldll, "RegisterServiceProcess")))
		return;
	RegisterServiceProcess(0, 1);
}
#endif

int
main(int argc, char *argv[]) {
	setlocale(LC_ALL, "");
//	bindtextdomain(PACKAGE, LOCALEDIR);
//	textdomain(PACKAGE);
	int fd;

	if (argc < 2) {
		fprintf(stderr, "usage: %s program [arg ...]\n",
			argv[0]);
		exit(1);
	}
	if (getpgrp() == getpid()) {
		switch(fork()){
		case -1:
			perror("fork");
			exit(1);
		case 0:		/* child */
#ifdef __CYGWIN__
			if (!is_winnt)
				register_9x_service ();
#endif
			break;
		default:	/* parent */
			exit(0);
		}
	}
	if (setsid() < 0) {
		perror("setsid"); /* cannot happen */
		exit(1);
	}
#ifdef __CYGWIN__
	if ((fd = open ("/dev/null", O_RDWR, 0)) >= 0) {
		if (isatty (STDIN_FILENO))
			dup2 (fd, STDIN_FILENO);
		if (isatty (STDOUT_FILENO))
			dup2 (fd, STDOUT_FILENO);
		if (isatty (STDERR_FILENO))
			dup2 (fd, STDERR_FILENO);
		if (fd > 2)
			close (fd);
	}
#endif
	execvp(argv[1], argv + 1);
	perror("execvp");
	exit(1);
}
