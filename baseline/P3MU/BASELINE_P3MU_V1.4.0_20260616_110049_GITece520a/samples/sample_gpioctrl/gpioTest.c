#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h> 

#include "gpio_user.h"

#define GPIO_FILE   "/dev/gpio_ctrl"

static  int b_gpio_init = 0 ;
static  int g_fd_gpio   = -1;

int init_gpio(void)
{
    if(!b_gpio_init)
    {
        g_fd_gpio = open(GPIO_FILE,O_RDWR);
        if (g_fd_gpio < 0)
        {
            perror("Can't open g_fd_gpio");
            return -1;
        }
    }
    b_gpio_init = 1;

    return 0 ;
}

int deInit_gpio(void)
{
    if (g_fd_gpio > 0)
    {
        close(g_fd_gpio);
    }

    g_fd_gpio = -1;
    b_gpio_init = 0;

    return 0 ;
}
int gpio_ioctl(int ioct_value, void *pValue)
{
    if(!b_gpio_init)
    {
        if(init_gpio())
            return 0xFF;
    }

    return ioctl (g_fd_gpio, ioct_value, pValue);
}

static pthread_mutex_t gpio_export;


/* 配置GPIOgroupnumber的gpio_id为输出或者输入(为1则输出，为0则输入) */
int	Com_gpio_config(unsigned int  groupnumber, unsigned char gpio_id,unsigned char inout)
{
	gpio_groupbit_info gpio_info;
	gpio_info.groupnumber = groupnumber;	
	gpio_info.bitnumber   = gpio_id;   // GPIOgroupnumber_gpio_id

	if (inout)
		gpio_info.value = 1;
	else
		gpio_info.value = 0;
	pthread_mutex_lock(&gpio_export);
	
	if(init_gpio())
	{		
		printf("gpio_ioctl error!");
		pthread_mutex_unlock(&gpio_export);
		goto gpio_config_exit;
	}
	if (gpio_ioctl(GPIO_SET_DIR, (void *)&gpio_info))
	{
		printf("gpio_ioctl error!");
		pthread_mutex_unlock(&gpio_export);
		goto gpio_config_exit;
	}
	pthread_mutex_unlock(&gpio_export);

gpio_config_exit:
	return 0;
}

/* 获取GPIOgroupnumber的gpio_id位的输入输出状态，放在dir中 */
int Com_gpio_get_dir(unsigned int  groupnumber, unsigned char gpio_id, int *dir)
{
	gpio_groupbit_info gpio_info;
	
	gpio_info.groupnumber = groupnumber;
	gpio_info.bitnumber   = gpio_id;	 // GPIOgroupnumber_gpio_id
	gpio_info.value       = 0;

	pthread_mutex_lock(&gpio_export);
	if (gpio_ioctl(GPIO_GET_DIR, (void *)&gpio_info))
	{
		printf("gpio_ioctl error!");
		pthread_mutex_unlock(&gpio_export);
		goto gpio_get_exit;
	}
	*dir = gpio_info.value;
	pthread_mutex_unlock(&gpio_export);

gpio_get_exit:
	return 0;
}

/* 设置GPIOgroupnumber的gpio_id位为1 */
void Com_gpio_set(unsigned int  groupnumber, unsigned char gpio_id)
{
	gpio_groupbit_info gpio_info;
	
	gpio_info.groupnumber = groupnumber;
	gpio_info.bitnumber   = gpio_id;   // GPIOgroupnumber_gpio_id
	gpio_info.value       = 1;
	
	pthread_mutex_lock(&gpio_export);
	
	if (gpio_ioctl(GPIO_WRITE_BIT, (void *)&gpio_info))
	{
		printf("gpio_ioctl error!");
		pthread_mutex_unlock(&gpio_export);
		goto gpio_set_exit;
	}
	pthread_mutex_unlock(&gpio_export);
	
gpio_set_exit:
	return;
}

/* 清GPIOgroupnumber的gpio_id位为0 */
void Com_gpio_clr(unsigned int  groupnumber, unsigned char gpio_id)
{
	gpio_groupbit_info gpio_info;

	gpio_info.groupnumber = groupnumber;
	gpio_info.bitnumber   = gpio_id;	 // GPIOgroupnumber_gpio_id
	gpio_info.value 	  = 0;
	
	pthread_mutex_lock(&gpio_export);
	
	if (gpio_ioctl(GPIO_WRITE_BIT, (void *)&gpio_info))
	{
		printf("gpio_ioctl error!");
		pthread_mutex_unlock(&gpio_export);
		goto gpio_clr_exit;
	}
	pthread_mutex_unlock(&gpio_export);
	
gpio_clr_exit:
	return;
}

/* 读取GPIOgroupnumber的gpio_id位状态，放在level中 */
void Com_gpio_get(unsigned int  groupnumber, unsigned char gpio_id, int *level)
{
	gpio_groupbit_info gpio_info;
	
	gpio_info.groupnumber = groupnumber;
	gpio_info.bitnumber   = gpio_id;   // GPIOgroupnumber_gpio_id
	gpio_info.value       = 0;
	
	pthread_mutex_lock(&gpio_export);
	
	if (gpio_ioctl(GPIO_READ_BIT, (void *)&gpio_info))
	{
		printf("gpio_ioctl error!");
		pthread_mutex_unlock(&gpio_export);
		goto gpio_set_exit;
	}
	*level = gpio_info.value;
	pthread_mutex_unlock(&gpio_export);
	
gpio_set_exit:
	return;
}

#define IRCUT_ENABLE_TIME 500000
#define FUNCTION_PARAM_OPERATE_FUNC "func"
#define FUNCTION_PARAM_OPERATE_GPIO "gpio"
#define FUNCTION_PARAM_OPERATE_IRCUT "ircut"
#define GPIO_OPERATE_DIRECTE_OUT "out"
#define GPIO_OPERATE_DIRECTE_IN "in"

/* 管脚方向 */
typedef enum
{
    GPIO_DIRECTION_OUT = 0, 			/* 输出 */
    GPIO_DIRECTION_IN,       			/* 输入 */
} ISP_TUNE_AWB_MODE_E;

void PrintMenu(char *pProgram)
{
	printf("1: %s [out/in] [gpioGrup] [gpioNum] {1/0}\n", pProgram);
	printf("	eg1: %s out 1 17 1    //GPIO1_17 output high!\n", pProgram);
	printf("	eg2: %s out 1 17 0    //GPIO1_17 output low!\n", pProgram);
	printf("	eg3: %s in 1 18    	  //GPIO1_18 input status!\n", pProgram);
}

/**
********************************************************************************
 * @函数功能: 
 * @作者    : HL.LIU
 * @param[in] argc
 * @param[in] *
 * @param[out] *pValue 返回IO口为输出时的高低电平
 * @param[out] *pDirection 返回IO的方向，1：输入，0：输出
 * @param[out] *pGpioGrp 返回IO的组
 * @param[out] *pGpioNum 返回IO的号
 * @返回值  : 
 * @其他    : 
********************************************************************************
*/
int CheckParam(int argc, char **argv, int *pValue, int *pDirection, int *pGpioGrp, int *pGpioNum)
{
	if(argc < 4)
	{
		PrintMenu(argv[0]);
		return -1;
	}

	if(0 == strncmp(GPIO_OPERATE_DIRECTE_OUT, argv[1], 3))
	{
		*pDirection = GPIO_DIRECTION_OUT;
	}
	else
	if(0 == strncmp(GPIO_OPERATE_DIRECTE_IN, argv[1], 3))
	{
		*pDirection = GPIO_DIRECTION_IN;
	}
	else
	{
		printf("Param error!\n");
		PrintMenu(argv[0]);
		return -1;
	}

	if(GPIO_DIRECTION_OUT == *pDirection)				/* out */
	{
		if(argc != 5)
		{
			printf("Param error!\n");
			PrintMenu(argv[0]);
			return -1;
		}
		*pGpioGrp = atoi(argv[2]);
		*pGpioNum = atoi(argv[3]);
		*pValue = atoi(argv[4]);
	}
	if(GPIO_DIRECTION_IN == *pDirection)				/* in */
	{
		if(argc != 4)
		{
			printf("Param error!\n");
			PrintMenu(argv[0]);
			return -1;
		}
		*pGpioGrp = atoi(argv[2]);
		*pGpioNum = atoi(argv[3]);
	}

	return 0;
}

int main(int argc, char **argv)
{
	int GPIO_GRP = 0;
	int GPIO_NUM = 0;
	int direction = 0;
	int value = 0;
	int ret = 0;
	
	ret = CheckParam(argc, argv, &value, &direction, &GPIO_GRP, &GPIO_NUM);
	if(ret)
	{
		return -1;
	}

	if(GPIO_DIRECTION_OUT == direction)
	{
		printf("Set gpio%d_%d %s!\n", GPIO_GRP, GPIO_NUM, value ? "High" : "Low");
	}
	else
	{
		printf("Get gpio%d_%d value=", GPIO_GRP, GPIO_NUM);
	}

	#if 1
	if(GPIO_DIRECTION_OUT == direction)
	{
		Com_gpio_config(GPIO_GRP, GPIO_NUM, 1);
		if(value)
		{
			Com_gpio_set(GPIO_GRP, GPIO_NUM);
		}
		else
		{
			Com_gpio_clr(GPIO_GRP, GPIO_NUM);
		}
	}
	else
	{
		Com_gpio_config(GPIO_GRP, GPIO_NUM, 0);
		Com_gpio_get(GPIO_GRP, GPIO_NUM, &value);
		printf("%d!\n", value);
	}

	#else
	GPIO_GRP = 1;
	int GPIOF_1 = 17;
	int GPIOF_2 = 18;
	int dayOrNight = 0;
	dayOrNight = atoi(argv[1]);
	printf("Set ircut to %s\n", dayOrNight ? "Day" : "Night");
	
    /* 设置ircut相关引脚为输出 */
    Com_gpio_config(GPIO_GRP, GPIOF_1, 1); 
    Com_gpio_config(GPIO_GRP, GPIOF_2, 1); 

	if(1 == dayOrNight)
	{
		Com_gpio_set(GPIO_GRP, GPIOF_1);	 // GPIOF_2 	AIN  
		Com_gpio_clr(GPIO_GRP, GPIOF_2);		  // GPIOF_1	 BIN  
		usleep(IRCUT_ENABLE_TIME);			 /* 延时 */
		Com_gpio_clr(GPIO_GRP, GPIOF_1);	 /* 0 */   
		Com_gpio_clr(GPIO_GRP, GPIOF_2);	 /* 0 */					
	}
	else
	{
		Com_gpio_clr(GPIO_GRP, GPIOF_1);	 // GPIOF_2 	AIN  
		Com_gpio_set(GPIO_GRP, GPIOF_2);		  // GPIOF_1	 BIN  
		usleep(IRCUT_ENABLE_TIME);			 /* 延时 */
		Com_gpio_clr(GPIO_GRP, GPIOF_1);	 /* 0 */   
		Com_gpio_clr(GPIO_GRP, GPIOF_2);	 /* 0 */						
	}
	#endif

	return 0;
}