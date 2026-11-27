#ifndef __XRS_DATA_QUEUE_H_
#define __XRS_DATA_QUEUE_H_
#include <stdint.h>
#include <list>
#include <ctime>
#include <chrono>
#include <mutex>
#include <vector>
#include "CircleQueue.h"

using namespace std;

/* 视频帧 */
typedef enum 
{
	PktError            = 0x00,	
    PktIFrames          = 0x01, //sensor1 main I
    PktPFrames          = 0x02, //sensor1 main P
    PktMJPEG            = 0x03,
    PktBPFrames         = 0x20,
    PktBBPFrames        = 0x04,
    PktAudioFrames      = 0x08,
    PktQCIFIFrames      = 0x10, //sensor1 aux I
    PktQCIFPFrames      = 0x40, //sensor1 aux P
    PktBIFrames         = 0x90,
    PktBBIFrames        = 0xC0,
    PktSQCIFIFrames		= 0x11, //sensor1 hdaux I
    PktSQCIFPFrames		= 0x41, //sensor1 hdaux P
    PktSysHeader        = 0x80,
    PktSysHeaderAux		= 0x81,
    PktSysHeaderHDAux 	= 0x82,
    PktMotionDetection  = 0x83,
    PktAimDetection     = 0x84,
    PktPrivateData      = 0x85,
    PktIAFrames         = 0x86,//人形帧，就是一张jpg图
    PktIAFramesForStream = 0x86+0xff,   //供设备内部使用，用于区分智能事件帧与智能码流帧
    PktOrigImage        = 0x87,
	PktFourthIFrames	= 0x42,
	PktFourthPFrames	= 0x43,
	PktCaptureIFrames	= 0x46,
	PktCapturePFrames	= 0x47,
	PktAdvFaceFrames	= 0xA0,
    PktIACarFrames      = 0xA1,//车形帧，就是一张jpg图
    PktIAElectronicFenceFrames    = 0xA2,//电子围栏
	PktIACrossBoardDetectFrames   = 0xA3,//越界侦测
	PktIAOffDutyDetectFrames      = 0xA4,//离岗侦测
	PktIAPassengerFlowFrames      = 0xA5,//客流统计
    PktIAFireDetectFrames         = 0xA6,//火焰
    PktIAPassengerFlowHumanEventFrames = 0xA7,//客流统计人形事件
    PktIAVideoShelterDetectEventFrames = 0xA8,//视频遮挡检测事件
	PktIAFaceSnapshotEventFrames = 0xA9,//人脸抓拍检测事件
	PktIAMaskDetectEventFrames = 0xAA,//口罩检测事件
    PktIFramesForS1     = 0xB0, //sensor2 main I
    PktPFramesForS1     = 0xB1, //sensor2 main P
    PktQCIFIFramesForS1 = 0xB2, //sensor2 aux I
    PktQCIFPFramesForS1 = 0xB3, //sensor2 aux P
	PktFourthIFramesForS1 = 0xB4, //sensor2 mjpeg I
	PktFourthPFramesForS1 = 0xB5, //sensor2 mjpeg P
    PktIFramesForS2     = 0xB6, //sensor3 main I
    PktPFramesForS2     = 0xB7, //sensor3 main P
    PktQCIFIFramesForS2 = 0xB8, //sensor3 aux I
    PktQCIFPFramesForS2 = 0xB9, //sensor3 aux P
	PktFourthIFramesForS2 = 0xBA, //sensor3 mjpeg I
	PktFourthPFramesForS2 = 0xBB, //sensor3 mjpeg P
    PktIFramesForS3     = 0xBC, //sensor4 main I
    PktPFramesForS3     = 0xBD, //sensor4 main P
    PktQCIFIFramesForS3 = 0xBE, //sensor4 aux I
    PktQCIFPFramesForS3 = 0xBF, //sensor4 aux P
	PktFourthIFramesForS3 = 0xC0, //sensor4 mjpeg I
	PktFourthPFramesForS3 = 0xC1, //sensor4 mjpeg P
} FrameType_t;

typedef enum 
{
	MAIN_STREAM 	= 0,
	SUB_STREAM 		= 1,
	THIRD_STREAM 	= 2,
	CAPTURE_STREAM 	= 3,
	AUDIO_STREAM	= 4,
	STREAM_MAX
}FrameTypeIndex;

namespace xrs_circle_queue {
class User;
class DataQueue;
class DataNode
{
private:
    //!\brief 通道号
    int32_t s32Channel;
    //!\brief 码流
    int32_t s32Stream;
    //!\brief s32FrameType 帧类型
    int32_t s32FrameType;
    //!\brief 内存块地址
    uint8_t *pData;
    //!\brief 内存块大小
    int32_t s32DataSize;
    //!\brief 系统时间戳,unix timestamp
    int64_t s64Timestamp;
    //!\brief 时间戳，系统启动开始计时的绝对时间戳
    std::chrono::steady_clock::time_point tpSteady;
    //!\brief 队列句柄
    DataQueue *pQueue;
    //!\brief 序号
    int32_t s32Seq;
    //!\brief 引用计数
    int32_t s32RefCount;
public:
    DataNode(DataQueue * ptrQueue, uint8_t *pBuffer, int32_t s32Size):
        pData(pBuffer), s32DataSize(s32Size),pQueue(ptrQueue) {};
    DataNode(DataQueue *ptrQueue, CQ_DATA_S &stData, bool bCopy);
    ~DataNode() {};
    DataNode &operator=(const DataNode &refNode);
    operator CQ_DATA_S() const;
    bool IsBusy() const { return (s32RefCount > 0); };
    void AddRef() { s32RefCount++; };
    void Release() { (s32RefCount > 0)?s32RefCount--:0; };
    void FreeMem() { if (nullptr != pData) {free(pData); pData = nullptr;};};
    int32_t Size() const { return s32DataSize; };
    int32_t Seq() const { return s32Seq; };
	int32_t GetFrameType() const { return s32FrameType; };
    int32_t GetChannel() const { return s32Channel; };
    int32_t GetStream() const { return s32Stream; };
    std::chrono::steady_clock::time_point Time() const { return tpSteady; };
    int Check(CQ_USER_PARAM_S &refParam) 
    { 
        return ((s32Channel == refParam.s32Channel) && 
            (s32Stream == refParam.s32Stream));
    };
};

struct QueueStatus
{
    int32_t m_s32TotalAddNodes;
    int32_t m_s32TotalNodeSize;
    int32_t  m_s32UsedSpace;
    std::chrono::time_point<std::chrono::steady_clock> m_timeHead;
    std::chrono::time_point<std::chrono::steady_clock> m_timeTail;
    int32_t  m_s32SeqHead;
    int32_t  m_s32SeqTail;
	int32_t  m_s32FrameCount[STREAM_MAX];
};


class DataQueue
{
public:
    typedef list<DataNode> NodeList;
    DataQueue(int32_t s32Size, int32_t s32Duration, bool bCopyMode);
    ~DataQueue();
    int32_t AddNode(CQ_DATA_S &stData);
    int32_t GetNode(User *pUser, CQ_DATA_S &refData);
    int32_t ReleaseNode(User *pUser);
    bool IsBusy();
    User *AddUser(void);
    int32_t DeleteUser(User *pUser);
    int32_t DeleteUser(void);
    //!\brief 清空缓冲区，未被占用的节点移入 listFree 待释放，被占用的节点延迟释放
    int32_t Clear(NodeList &listFree);
    //!\brief 重置消费者读取进度，并释放其当前占用的节点
    int32_t FlushUser(User *pUser);
    //!\brief 查询缓冲区状态
    int32_t GetStatus(CQ_STATUS_S &refStatus);
    //!\brief 队列尾迭代器，用于重置消费者进度
    NodeList::iterator EndNode() { return m_listQueue.end(); };
    void lock() {m_mtxLock.lock();};
    void unlock() {m_mtxLock.unlock();};

private:
    int RequestSpace(int32_t s32Size);
    DataQueue::NodeList::iterator FindNode(int32_t s32Seq, const NodeList::iterator& itNode);
    bool IsExpired(int32_t s32Seq);
    int32_t GetNextNode(User *pUser, CQ_DATA_S &refData);
    //!\brief 按消费者的起始策略定位起始节点
    bool FindStartNode(User *pUser, DataQueue::NodeList::iterator &itNode);
    //!\brief 节点是否匹配消费者的通道/码流过滤条件
    bool MatchUser(User *pUser, int32_t s32Channel, int32_t s32Stream);
    //!\brief 更新各码流帧计数统计
    void UpdateFrameCount(int32_t s32FrameType, int32_t s32Delta);

private:
    int32_t  m_s32TotalSpace;
    int32_t  m_s32MaxDuration;
	unsigned int time_prev = 0;
    bool m_bCopyMode;
    NodeList m_listQueue;
    NodeList m_listExpired;
    QueueStatus m_objStats;
    std::mutex m_mtxLock;

    typedef std::vector<User *> UserVector;
    UserVector m_vecUser;
};

}
#endif
