#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <sys/prctl.h>
#include "xtrace.h"

#ifdef __SUPPORT_FUNC_INSTRUMENT__
void __cyg_profile_func_enter(void *this_fn, void *call_site) {}
void __cyg_profile_func_exit(void *this_fn, void *call_site) {}
#endif

pthread_mutex_t tLock1;
pthread_mutex_t tLock2;

int tLockData1 = 0;
int tLockData2 = 0;

pthread_t tLockTestThreadID1;
pthread_t tLockTestThreadID2;

void* tLockTestThread1( void * lpParameter)
{
	prctl(PR_SET_NAME,"tLockTestThread1");
	int i = 0;

	while(1)
	{
        printf("%s\n", __FUNCTION__);
		XTRACE_MUTEX_LOCK(&tLock1);
		usleep(200*1000);
		XTRACE_MUTEX_LOCK(&tLock2);
		usleep(200*1000);
        XTRACE_MUTEX_UNLOCK(&tLock2);
		XTRACE_MUTEX_UNLOCK(&tLock1);

		usleep(700*1000);
	}
}

void* tLockTestThread2( void * lpParameter)
{
	prctl(PR_SET_NAME,"tLockTestThread2");
	int ret = 0;

	sleep(1);

	while(1)
	{
        printf("%s\n", __FUNCTION__);
		XTRACE_MUTEX_LOCK(&tLock2);
		usleep(200*1000);
#if 0
		ret = XTRACE_MUTEX_TRYLOCK(&tLock1);
        if (ret == 0)
        {
            usleep(540*1000);
            XTRACE_MUTEX_UNLOCK(&tLock1);
        }
#else
        XTRACE_MUTEX_LOCK(&tLock1);
        usleep(540*1000);
        XTRACE_MUTEX_UNLOCK(&tLock1);
#endif
		XTRACE_MUTEX_UNLOCK(&tLock2);

		usleep(100*1000);
	}
}

int main(int argc, char **argv)
{
	int ret = 0;
	pthread_attr_t		attr;
	struct sched_param 	param;

    XTrace_Init(1, 1000, 1000);
	
	XTRACE_MUTEX_INIT(&tLock1, NULL);
	XTRACE_MUTEX_INIT(&tLock2, NULL);

	pthread_attr_init(&attr);
	pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
	pthread_attr_getschedparam(&attr, &param);
	param.sched_priority = 0;
	pthread_attr_setschedparam(&attr, &param);

	ret = pthread_create(&tLockTestThreadID1, &attr, tLockTestThread1, NULL);

	ret = pthread_create(&tLockTestThreadID2, &attr, tLockTestThread2, NULL);

	pthread_attr_destroy(&attr);


	while(1){sleep(1);}

    pthread_mutex_destroy(&tLock1);
	pthread_mutex_destroy(&tLock2);

    XTrace_Destory();
	
	return 0;
}