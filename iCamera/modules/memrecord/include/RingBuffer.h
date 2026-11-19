/* 所有 (C)2017, zviewtech.com
 * 文件名称： RingBuffer.h
 * 文件标识： 
 * 内容摘要： 环形缓冲区模型
 * 其它说明： 
 * 当前版本： 1.0.2
 * 作    者： cong fan
 * 完成日期： 2017年4月28日
 * 
 * 修改记录：仿照linux kfifo写的环形缓冲区ring buffer
 * 修改日期：
 * 版 本 号：
 * 修 改 人：
 * 修改内容：
 * 1. 实现消费者和生产者的无锁并发访问(已经加锁处理，可以在多线程使用)。
 * 2. 使用与运算in & (size-1)代替模运算
 * 3. 在更新in或者out的值时不做模运算，而是让其自动溢出，利用溢出后的值参与运算，并且能够保证结果的正确,
      溢出运算保证了以下几点：
      (1) 环形缓冲区一般有两个指针: 读指针out和写指针in；
      (2) 读写操作交替进行，不会或极少出现只读或者只写的情况；
      (3) in - out 为环形缓冲区的数据长度；
      (4) 缓冲区大小size - (in - out) = 缓冲区空闲长度；
      (5) in == out 时缓冲区为空；
      (6) size == (in - out) 时缓冲区满了；
      (7) 缓冲区满了之后，如果继续写，数据写不进去，即不会破坏原来的数据；
      (8) 缓冲区空了之后，如果继续读，数据获取为空；
*/

#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>

#include "global_constant.h"

/* 入参    */
#define IN

/* 出参    */
#define OUT

/* 判断x是否是2的幂
 * 若x为2的次幂,则 x & (x-1) == 0，也就是x和x-1的各个位都不相同。例如 8(1000)和7(0111）
 * 若x不是2的次幂,则 x & (x-1) != 0,也就是x和x-1的各个位肯定有相同的，例如7(0111)和6(0110)
 * 假设有两个数A和B, B为2^n，其中n>=0，A>=0。则：
 * (A * B) 等价于 (A << n)
 * (A / B) 等价于 (A >> n)
 * (A % B) 等价于 (A & ( B - 1 ))
 */
#define IS_POWER_OF_2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))

/* 由于in和out为无符号类型，在put和get时，in和out都是增加，
 * 当达到最大值时，产生溢出，使得从0开始，进行循环使用 
 */
typedef struct tagRingBuffer  
{  
    void                *pBuffer;      // 指向整个环形缓冲区的buffer(该缓冲区的大小必须2的次幂，即size)
    unsigned int         size;         // 环形缓冲区的总大小(如果不为2的次幂，则自动向上取值)
    unsigned int         in;           // 入口位置
    unsigned int         out;          // 出口位置
    unsigned int         resolve_size; // 环形缓冲区预留空间大小(字节, 必须小于size)
    pthread_mutex_t     *pLock;        // 互斥锁 
}stuRingBuffer, *pRingBuffer;

/* 将数字向上取整为2的次幂 */
static inline unsigned int RoundUpToPowerOfTwo(IN unsigned int i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    i |= i >> 16;
    return i + 1;
}

/* 环形缓冲区中数据的长度 */
static unsigned int __ring_buffer_len(IN const pRingBuffer pRingBuf)
{
    return (pRingBuf->in - pRingBuf->out);
}

/* 唤醒缓冲区剩余空间长度 */
static unsigned int __ring_buffer_rest_len(IN const pRingBuffer pRingBuf)
{
    return (pRingBuf->size - pRingBuf->in + pRingBuf->out);
}

/* 从环形缓冲区里面获取数据, 返回实际读取的数据长度 */
static unsigned int __ring_buffer_get(IN pRingBuffer pRingBuf, IN void * pBuffer, IN unsigned int size)
{
    assert(pRingBuf && pBuffer);
    assert(size >= 0);

    unsigned int len = 0;
    /* 有数据的缓冲区的长度(判断是否有足够的数据取出) */
    size  = MIN(size, pRingBuf->in - pRingBuf->out);
    /* 首先从[out, pBuffer end]读取数据 */
    len = MIN(size, pRingBuf->size - (pRingBuf->out & (pRingBuf->size - 1)));
    memcpy(pBuffer, (char *)pRingBuf->pBuffer + (pRingBuf->out & (pRingBuf->size - 1)), len);
    /*  从[pBuffer start, ...]读取数据(如果不够则从buffer的开始位置取) */
    memcpy((char *)pBuffer + len, (char *)pRingBuf->pBuffer, size - len);

    /* 直接加，不做模运算。溢出后，从pBuffer的起始位置重新开始 */
    pRingBuf->out += size;

    return size;
}

/* 向环形缓冲区里面塞数据, 返回实际写入缓冲区中的数据
 * 要put的数据指针pBuffer和期望能够put的数据长度size，返回值是实际存放到缓冲区中的数据长度
 * (当缓冲区中空间不足时该值小于len)
 */
static unsigned int __ring_buffer_put(IN pRingBuffer pRingBuf, IN const char *pBuffer, IN unsigned int size)  
{
    assert(pRingBuf && pBuffer);
    assert(size >= 0);

    unsigned int len = 0;
    /* pBuffer中剩余空间的长度 */
    /* 计算实际向缓冲区中写入数据的大小。如果想要写入的数据len大于缓冲区中的空闲空间size - in + out，
     * 则只填充满缓冲空间。因为是循环缓冲区，所以其空闲空间有两部分：从in到缓冲空间的末尾,即
     * [in,buffer end]和缓冲空间的起始位置到out, 即[buffer start,out]
     */
    size = MIN(size, pRingBuf->size - pRingBuf->in + pRingBuf->out);    
    /* 当前in位置到pBuffer末尾的长度(判断[in,buffer end]这部分空间是否足够写入数据) */
    len  = MIN(size, pRingBuf->size - (pRingBuf->in & (pRingBuf->size - 1)));
    /* 首先复制数据到[in，pBuffer的末尾](向[in,buffer end]这部分空间写入数据) */
    memcpy((char *)pRingBuf->pBuffer + (pRingBuf->in & (pRingBuf->size - 1)), pBuffer, len);  
    /* 复制剩余的数据（如果有）到[pBuffer的起始位置,...](如果数据还没有写完，则向[buffer start,out]这部分空间写入数据) */  
    memcpy(pRingBuf->pBuffer, pBuffer + len, size - len);
    /* 直接加，不做模运算。当溢出时，从pBuffer的开始位置重新开始 */
    pRingBuf->in += size;
    return size;
}  

/* 移动环形缓冲区的in或者out指针(谨慎操作) */
static void __ring_buffer_move_point(IN pRingBuffer pRingBuf, IN unsigned int in_offset, IN unsigned int out_offset)
{
    assert(pRingBuf);

    if (in_offset)
    {
        pRingBuf->in += in_offset;
    }

    if (out_offset)
    {
        pRingBuf->out += out_offset;
    }
}

/*@breif ring_buffer_init 初始化缓冲区
 *@param[in] pBuffer 为缓冲区分配的地址
 *@param[in] size 缓冲区大小
 *@param[in] resolve_size 缓冲区保留空间大小(必须小于size)
 *@param[in] pLock 缓冲区的锁指针
 *@return pRingBuffer 缓冲区结构体指针
 *@       NULL表示分配失败
 */
pRingBuffer ring_buffer_init(IN void *pBuffer, IN unsigned int size, IN unsigned int resolve_size, IN pthread_mutex_t *pLock)  
{  
    assert(pBuffer && pLock);
    assert(resolve_size >= 0);
    assert(size > resolve_size);
    
    pRingBuffer pRingBuf = NULL;
    unsigned int new_size = 0;

    /* 之所以要将size转化为2^n, 是因为这样可以转化为与运算，
     * 如：pRingBuffer->in % pRingBuffer->size可以转化为pRingBuffer->in&(pRingBuffer->size–1)
     */
    if (!IS_POWER_OF_2(size))  
    {  
        new_size = RoundUpToPowerOfTwo(size);
        printf("Size of buffer is %d, change to be power of 2 is %d\n", size, new_size);
        size = new_size;
    }
    
    /* 为结构体分配空间 */
    pRingBuf = (pRingBuffer)malloc(sizeof(stuRingBuffer));  
    if (!pRingBuf)
    {  
        fprintf(stderr,"Failed to malloc memory, reason: %s", strerror(errno));
        return pRingBuf;  
    }
    memset(pRingBuf, 0, sizeof(stuRingBuffer));
    pRingBuf->pBuffer = pBuffer;
    pRingBuf->size = size;
    pRingBuf->resolve_size = resolve_size;
    pRingBuf->in = 0;
    pRingBuf->out = 0;
    pRingBuf->pLock = pLock;
    
    return pRingBuf;  
} 

/*@breif ring_buffer_reset 复位环形缓冲区
 *@param[in] pRingBuf 缓冲区结构体指针
 *@return void
 */
void ring_buffer_reset(IN pRingBuffer pRingBuf)
{
    if (pRingBuf)
    {
        pthread_mutex_lock(pRingBuf->pLock);  
        pRingBuf->in = pRingBuf->out = 0;
        pthread_mutex_unlock(pRingBuf->pLock);  
    }
}

/*@breif ring_buffer_free 释放环形缓冲所有内存
 *@param[in] pRingBuf 缓冲区结构体指针
 *@return void
 *@note   该函数会释放环形缓冲区结构体的内存和环形缓冲区的总内存
 *        所以，只要调用该接口，关于环形缓冲区的所有内存都会释放
 *        后面不需要再调用free接口进行释放，否则会出现段错误
 */
void ring_buffer_free(IN pRingBuffer pRingBuf)  
{  
    if (pRingBuf)
    {  
        if (pRingBuf->pBuffer)  
        {
            /* 释放环形缓冲区的总内存 */
            free(pRingBuf->pBuffer);  
            pRingBuf->pBuffer = NULL;  
        }
        /* 释放分配的结构体内存 */
        free(pRingBuf);  
        pRingBuf = NULL;  
    }  
}

/*@breif ring_buffer_len 当前缓冲区里面数据长度
 *@param[in] pRingBuf 缓冲区结构体指针
 *@return 当前缓冲区里面数据长度
 *        = 0 表示环形缓冲区为空 
 *        = pRingBuf->size 表示环形缓冲区满
 */
unsigned int ring_buffer_len(IN const pRingBuffer pRingBuf)  
{  
    unsigned int len = 0;  
    pthread_mutex_lock(pRingBuf->pLock);  
    len = __ring_buffer_len(pRingBuf);  
    pthread_mutex_unlock(pRingBuf->pLock);  
    return len;  
}  

/*@breif ring_buffer_get 从缓冲区获取数据
 *@param[in]  pRingBuf 缓冲区结构体指针
 *@param[out] pBuffer 从缓冲区里面获取到的数据
 *@param[in] size 期望能够获取的数据长度
 *@return 返回的是实际取到的数据长度
 *        = 0 没有获取到数据
 *        > 0 获取到数据的大小
 */
unsigned int ring_buffer_get(IN pRingBuffer pRingBuf, OUT void *pBuffer, IN unsigned int size)  
{  
    unsigned int ret = 0;  
    pthread_mutex_lock(pRingBuf->pLock);  
    ret = __ring_buffer_get(pRingBuf, pBuffer, size);  
    
    if (pRingBuf->in == pRingBuf->out)  //buffer中没有数据  
    {
        pRingBuf->in = pRingBuf->out = 0;
    }
    pthread_mutex_unlock(pRingBuf->pLock);  
    return ret;  
} 

/*@breif ring_buffer_put 向缓冲区塞数据
 *@param[in] pRingBuf 缓冲区结构体指针
 *@param[in] pBuffer 塞入数据的句柄指针
 *@param[in] size 期望能够塞入的数据长度
 *@return 返回实际保存到缓冲区中的数据长度
 */
unsigned int ring_buffer_put(IN pRingBuffer pRingBuf, IN void *pBuffer, IN unsigned int size)  
{  
    unsigned int ret = 0;
    pthread_mutex_lock(pRingBuf->pLock);  
    ret = __ring_buffer_put(pRingBuf, pBuffer, size);
    pthread_mutex_unlock(pRingBuf->pLock);
    return ret;
}

#if 1

/*--------------------- 以下为新添修改接口，用于帧的输入和获取，以及相关异常情况处理 -----------------------*/

#define RB_INFO(format, args...) do{printf("\033[1;35m[RING_BUFFER][%d]\033[m" format, __LINE__, ##args);}while(0)
#define RB_DBG(format, args...)  do{printf("\033[1;32m[RING_BUFFER][%d]\033[m" format, __LINE__, ##args);}while(0)
#define RB_ERR(format, args...)  do{printf("\033[1;31m[RING_BUFFER][%d]\033[m" format, __LINE__, ##args);}while(0)

/* 私有帧头信息 */
typedef struct tagPrivateFrameHeader
{
    unsigned int        DataPacketStartCode;   // 固定为0xB7010000
    unsigned int        FrameNo;               // 媒体数据包的序号(音视频独立编号)
    unsigned char       FrameType;
    unsigned char       byLoadType; // 媒体数据包的信息,具体含义依赖于帧类型(H264:0x00,H265:        0x04,MJPEG:0x05)
    unsigned int        ShowTime;             // 表示媒体数据包相对于上一帧的偏差时间(频率90Khz)
    unsigned long       u32RealTimeSec;
    unsigned long       u32RealTimeUSec;
    unsigned short      uReserved;
    unsigned int        nOverloadLen;        // 表示媒体数据包的数据长度
}__attribute__((packed)) PRIVATEFRAMEHEADER, *LPPRIVATEFRAMEHEADER;

#define FRAME_HEAD_LENGTH   (28)

/*@breif my_ring_buffer_put 重新定义写缓冲区操作
 *@param[in]  pRingBuf 缓冲区结构体指针
 *@param[in] pBuffer 塞入数据的句柄指针
 *@param[in] size 期望能够塞入的数据长度
 *@note 考虑到缓冲区满的情况,如果满，则in指针拖着out指针跑
 */
int my_ring_buffer_put(IN pRingBuffer pRingBuf, IN const char *pBuffer, IN unsigned int size)  
{
    int is_error_exit = 0;
    unsigned int have_put_size = 0;
    unsigned int buff_rest_size = 0;

    pthread_mutex_lock(pRingBuf->pLock);  

    /* 如果实际写缓冲区的数据长度have_put_size不等于想要写入的数据长度size，
       则表明缓冲区已经满了，需要重新定位并丢帧(此处的丢帧是利用pRingBuf->out实现)
       注意: 丢数据和写数据的单位是一帧，即操作的单位必须是完整的一帧数据，所以
            不管是否找到I帧，都必须要将一帧数据写完整
    */
    buff_rest_size = __ring_buffer_rest_len(pRingBuf);
    if (size > buff_rest_size)
    {
        //RB_DBG("------RingBuffer is full![in:%d, out:%d] RingBuffer rest size is %d, put frame size is %d------\n", 
        //        pRingBuf->in, pRingBuf->out, have_put_size, size);
        pthread_mutex_unlock(pRingBuf->pLock);
        return 0;
    }

    while((have_put_size=__ring_buffer_put(pRingBuf, pBuffer, size)) != size)
    {
        int is_first_frame = 1;
        unsigned int have_get_size = 0;
        PRIVATEFRAMEHEADER RawHead;
        
        RB_DBG("------RingBuffer is full[%d!=%d] pos[in:%u, out:%u, sub:%u], need to drop some frame------\n", 
                have_put_size, size, pRingBuf->in, pRingBuf->out, pRingBuf->in-pRingBuf->out);

        /* 找到当前读指针位置获取一帧数据,判断是否为I帧,如果是则直接定位到帧头,否则继续寻找 */
        for (;;)
        {
            memset(&RawHead, 0, FRAME_HEAD_LENGTH);
            /* 首先获取帧头，校验帧头长度和帧校验码 */
            have_get_size = __ring_buffer_get(pRingBuf, &RawHead, FRAME_HEAD_LENGTH);
            if (have_get_size == FRAME_HEAD_LENGTH)
            {
                if (0xB7010000 == RawHead.DataPacketStartCode)
                {
                    /* 丢弃首帧，直接从第二帧开始找，如果找到I帧，则定位到帧头 */
                    if (is_first_frame)
                    {
                        /* 跳过帧数据 */
                        __ring_buffer_move_point(pRingBuf, 0, RawHead.nOverloadLen);
                        have_get_size = RawHead.nOverloadLen;
                        is_first_frame = 0;
                        continue;
                    }

                    /* 如果找到I帧 */
                    if (0x1 == RawHead.FrameType)
                    {                
                        /* 则重新定位到I帧帧头, out指针后退一个帧头长度，指向当前I帧首地址 */
                        // pRingBuf->out -= FRAME_HEAD_LENGTH;
                        __ring_buffer_move_point(pRingBuf, 0, -FRAME_HEAD_LENGTH);
                        break;
                    }
                    else
                    {
                        /* 如果没找到I帧，则跳过该帧数据(注意：只是帧数据，帧头在前面已经跳过了)，继续寻找下一个 */
                        __ring_buffer_move_point(pRingBuf, 0, RawHead.nOverloadLen);
                    }
                }
                else
                {
                    /* 出错异常 */
                    RB_ERR("Checksum frame head fail(%#x!=0xB7010000) in:%d, out:%d!\n",
                           RawHead.DataPacketStartCode, pRingBuf->in, pRingBuf->out);
                    is_error_exit = 1;
                    break;
                }
                
                if (0 == __ring_buffer_len(pRingBuf))
                {
                    RB_INFO("I Frame didn't been found! buffer is empty!\n");
                    break;
                }
            }
            else
            {
                /* 出错异常 */
                RB_ERR("Get frame head err! have_get_size %d != %d!\n", have_get_size, FRAME_HEAD_LENGTH);
                is_error_exit = 1;
                break;
            }
        }
        
        /* 源数据buffer指针定位到后面位置，将剩余的一帧数据写完整 */
        pBuffer = pBuffer + have_put_size;
        size -= have_put_size;
    }
    
    /* 出错异常，清空缓冲区 */
    if (is_error_exit)
    {
        RB_ERR("Warning: Put some frame data error! need to clean up ring buffer!\n");
        pRingBuf->in = pRingBuf->out = 0;
    }
    pthread_mutex_unlock(pRingBuf->pLock);
    
    return 0;
}

/*@breif my_ring_buffer_get 重新定义读缓冲区操作
 *@param[in] pRingBuf 缓冲区结构体指针
 *@param[out] pDataBuffer 获取数据的句柄指针
 *@param[out] pFrameHead 获取帧头信息的句柄指针
 */
int my_ring_buffer_get(IN pRingBuffer pRingBuf, OUT char *pDataBuffer, OUT LPPRIVATEFRAMEHEADER pFrameHead)  
{
    int is_error_exit = 0;
    unsigned int have_get_size = 0;

    pthread_mutex_lock(pRingBuf->pLock);  

    if (__ring_buffer_len(pRingBuf))
    {
        /* 获取数据的时候，首先获取帧头，检查帧头长度，校验帧校验码 */
        have_get_size = __ring_buffer_get(pRingBuf, pFrameHead, FRAME_HEAD_LENGTH);
        if (have_get_size == FRAME_HEAD_LENGTH)
        {
            if (0xB7010000 == pFrameHead->DataPacketStartCode)
            {
                /* 获取实际的帧数据并检查帧数据长度 */
                have_get_size = __ring_buffer_get(pRingBuf, pDataBuffer, pFrameHead->nOverloadLen);
                if (have_get_size != pFrameHead->nOverloadLen)
                {
                    RB_ERR("Get some frame data lenth error! have_get_size %d != %d!\n", have_get_size, pFrameHead->nOverloadLen);      
                    is_error_exit = 1;
                }
            }
            else
            {
                RB_ERR("Checksum frame head error! %#x != 0xB7010000!\n", pFrameHead->DataPacketStartCode);      
                is_error_exit = 1;
            }
        }
        else
        {
            RB_ERR("Get frame head error! have_get_size %d != 28!\n", have_get_size);      
            is_error_exit = 1;
        }
    }
    if (pRingBuf->in == pRingBuf->out)  //buffer中没有数据  
    {
        pRingBuf->in = pRingBuf->out = 0;
    }
    if (is_error_exit)
    {
        RB_ERR("Warning: Get some frame data error! need to clean up ring buffer!\n");        
        pRingBuf->in = pRingBuf->out = 0;
        have_get_size = 0;
    }
    
    pthread_mutex_unlock(pRingBuf->pLock);
    
    return have_get_size;
}

/*@breif my_ring_buffer_get_framehead 获取帧头且抛出当前的帧
 *@param[in] pRingBuf 缓冲区结构体指针
 *@param[out] pFrameHead 获取帧头信息的句柄指针
 *@return 成功返回帧数据的大小
 *@note 调用该函数之后，获取帧头信息，但是out指针已经移动了一帧数据的大小(帧头+帧数据)
 *@     成功则返回当前帧数据大小
 */
int my_ring_buffer_get_framehead(IN pRingBuffer pRingBuf, OUT LPPRIVATEFRAMEHEADER pFrameHead)  
{
    int is_error_exit = 0;
    unsigned int have_get_size = 0;

    pthread_mutex_lock(pRingBuf->pLock);  

    if (__ring_buffer_len(pRingBuf))
    {
        /* 获取数据的时候，首先获取帧头，检查帧头长度，校验帧校验码 */
        have_get_size = __ring_buffer_get(pRingBuf, pFrameHead, FRAME_HEAD_LENGTH);
        if (have_get_size == FRAME_HEAD_LENGTH)
        {
            if (0xB7010000 == pFrameHead->DataPacketStartCode)
            {
                /* 移动OUT指针，跳过帧数据 */
                __ring_buffer_move_point(pRingBuf, 0, pFrameHead->nOverloadLen);
                /* 为了尽量兼容之前逻辑，成功直接返回帧数据 */
                have_get_size = pFrameHead->nOverloadLen;
            }
            else
            {
                RB_ERR("Checksum frame head error! %#x != 0xB7010000!\n", pFrameHead->DataPacketStartCode);      
                is_error_exit = 1;
            }
        }
        else
        {
            RB_ERR("Get frame head error! have_get_size %d != 28!\n", have_get_size);      
            is_error_exit = 1;
        }
    }
    if (pRingBuf->in == pRingBuf->out)  //buffer中没有数据  
    {
        pRingBuf->in = pRingBuf->out = 0;
    }
    if (is_error_exit)
    {
        RB_ERR("Warning: Get some frame data error! need to clean up ring buffer!\n");        
        pRingBuf->in = pRingBuf->out = 0;
        have_get_size = 0;
    }
    
    pthread_mutex_unlock(pRingBuf->pLock);
    
    return have_get_size;
}

/*@breif my_ring_buffer_index_point 对于保留空间进行处理
 *@param[in] pRingBuf 缓冲区结构体指针
 *@note 在每一次触发get操作的时候需要执行一次进行判断
 */
void my_ring_buffer_index_point(IN pRingBuffer pRingBuf)
{
    unsigned int have_get_size = 0;
    PRIVATEFRAMEHEADER framehead;

    pthread_mutex_lock(pRingBuf->pLock);
    
    RB_DBG("------Start to reIndex the Out Pointer [in:%u out:%u resolve_size:%u]------\n", pRingBuf->in, pRingBuf->out, pRingBuf->resolve_size);

    /* 如果buffer剩余空间小于了保留空间的差值，则需要重新定位out指针 */
    while(pRingBuf->size - pRingBuf->in + pRingBuf->out < pRingBuf->resolve_size)
    {
        /* 获取数据的时候，首先获取帧头，检查帧头长度，校验帧校验码 */
        have_get_size = __ring_buffer_get(pRingBuf, &framehead, FRAME_HEAD_LENGTH);
        if (have_get_size == FRAME_HEAD_LENGTH)
        {
            if (0xB7010000 == framehead.DataPacketStartCode)
            {
                /* 移动OUT指针，跳过帧数据 */
                __ring_buffer_move_point(pRingBuf, 0, framehead.nOverloadLen);
            }
            else
            {
                RB_ERR("Checksum frame head error! %#x != 0xB7010000!\n", framehead.DataPacketStartCode);      
                break;
            }
        }
        else
        {
            RB_ERR("Get frame head error! have_get_size %d != 28!\n", have_get_size);      
            break;
       }
    }

    pthread_mutex_unlock(pRingBuf->pLock);
}

/*@breif my_ring_buffer_index_time 根据时间重新定位RingBuffer的IN指针
 *@param[in] pRingBuf 缓冲区结构体指针
 *@param[in] index_real_time_sec 需要定位的时间戳(秒)
 *@note 触发预录的时间，需要判断(该接口设计不合理，现遗弃)
 */
void my_ring_buffer_index_time(IN pRingBuffer pRingBuf, IN unsigned long index_real_time_sec)
{
    unsigned int have_get_size = 0;
    PRIVATEFRAMEHEADER framehead;

    for(;;)
    {
        /* 获取一帧数据 */
        have_get_size = my_ring_buffer_get_framehead(pRingBuf, &framehead);
        if (framehead.nOverloadLen == have_get_size)
        {
            if (0xB7010000 == framehead.DataPacketStartCode)
            {
                /* 只有是I帧才进行比较 */
                if (0x1 == framehead.FrameType)
                {
                    /* 比较当前需要定位的时间和当前环形缓冲区里面最老的时间 */
                    if (index_real_time_sec <= framehead.u32RealTimeSec)
                    {
#if 1
                        /* 重新定位OUT指针到前一帧，保留I帧数据(避免出现报警录像文件出现花屏) */
                        pthread_mutex_lock(pRingBuf->pLock);
                        __ring_buffer_move_point(pRingBuf, 0, -(sizeof(framehead)+framehead.nOverloadLen));
                        pthread_mutex_unlock(pRingBuf->pLock);
#endif
                        /* 如果缓冲区最老的时间超过了需要定位的时间，则直接定位，不需要丢帧 */
                        RB_INFO("Found nearest i frame time stamp [%ld <= %ld], break it!\n", index_real_time_sec, framehead.u32RealTimeSec);
                        RB_INFO("RingBuffer len %d bytes\n", ring_buffer_len(pRingBuf));
                        break;
                    }
                }
                /* 否则，丢掉该帧，重新读取下一帧进行比较 */
            }
            else
            {
                RB_ERR("Checksum frame head error! %#x != 0xB7010000!\n", framehead.DataPacketStartCode);      
                break;
            }
        }
        else
        {
            //RB_INFO("Ring buffer is empty!\n");
            break;
        }
    }
}
#endif 

#endif // end of __RING_BUFFER_H__ 

