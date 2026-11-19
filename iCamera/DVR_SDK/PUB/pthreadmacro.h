#ifndef __PTHREAD_MACRO_H__

#define __PTHREAD_MACRO_H__

/**************************************************************************
*                            宏定义                                       *
 **************************************************************************/
//初始化互斥锁
#define  INIT_MUTEX(mutex); \
{\
	pthread_mutex_init(&mutex,NULL);\
}

//销魂互斥锁
#define  UNINIT_MUTEX(mutex); \
{\
	pthread_mutex_destroy(&mutex);\
}
//加锁
#define  LOCK(mutex); \
{\
	pthread_mutex_lock(&mutex);\
}

//加锁
#define  UNLOCK(mutex); \
{\
	pthread_mutex_unlock(&mutex);\
}

#endif
