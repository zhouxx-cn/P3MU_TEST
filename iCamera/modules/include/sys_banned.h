#ifndef BANNED_H
#define BANNED_H

/*
 * This header lists functions that have been banned from our code base,
 * because they're too easy to misuse (and even if used correctly,
 * complicate audits). Including this header turns them into compile-time
 * errors.
 */
#ifdef __cplusplus
extern "C"
{
#endif
#define BANNED(func) sorry_##func##_is_a_banned_function

#if 1
/*字符串操作相关*/
#undef gets
#define gets(x) BANNED(gets)

#undef strcpy
#define strcpy(x,y) BANNED(strcpy)

#undef strcat
#define strcat(x,y) BANNED(strcat)

#undef strtok
#define strtok(x, y) BANNED(strtok)
//#undef strncpy
//#define strncpy(x,y,n) BANNED(strncpy)
//#undef strncat
//#define strncat(x,y,n) BANNED(strncat)

#undef sprintf
#define sprintf(...) BANNED(sprintf)

#undef vsprintf
#define vsprintf(...) BANNED(vsprintf)

#undef scanf
#define scanf(...) BANNED(scanf)

#undef fscanf
#define fscanf(...) BANNED(fscanf)

#undef vfscanf
#define vfscanf(...) BANNED(vfscanf)

#undef vscanf
#define vscanf(...) BANNED(vscanf)

#undef vsscanf
#define vsscanf(...) BANNED(vsscanf)

#undef getopt
#define getopt(...) BANNED(getopt)

#undef getopt_long
#define getopt_long(...) BANNED(getopt_long)

#undef getchar
#define getchar() BANNED(getchar)

#undef fgetc
#define fgetc(x) BANNED(fgetc)

#undef getc
#define getc(x) BANNED(getc)


/*时间操作相关*/
#undef gmtime
#define gmtime(t) BANNED(gmtime)

#undef localtime
#define localtime(t) BANNED(localtime)

#undef ctime
#define ctime(t) BANNED(ctime)
//#undef ctime_r
//#define ctime_r(t, buf) BANNED(ctime_r)
#undef asctime
#define asctime(t) BANNED(asctime)

//#undef asctime_r
//#define asctime_r(t, buf) BANNED(asctime_r)

#undef rand
#define rand() BANNED(rand)

#undef gethostbyaddr
#define gethostbyaddr(x, y, z) BANNED(gethostbyaddr)
//   gethostbyname_r         getaddrinfo
// #undef gethostbyname
// #define gethostbyname(x) BANNED(gethostbyname)

#undef inet_ntoa
#define inet_ntoa(x) BANNED(inet_ntoa)

#undef realpath
#define realpath(x, y) BANNED(realpath)

// #undef syslog
// #define syslog(...) BANNED(syslog)

#undef getpass
#define getpass(x) BANNED(getpass)

#endif

//#undef pthread_mutex_lock
//#define pthread_mutex_lock(t, buf) BANNED(pthread_mutex_lock)

//#undef pthread_mutex_unlock
//#define pthread_mutex_unlock(t, buf) BANNED(pthread_mutex_unlock)

#ifdef __cplusplus
}
#endif
#endif /* BANNED_H */
