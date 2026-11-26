#ifndef __MP4_PLAYER_H__
#define __MP4_PLAYER_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MP4Player MP4Player;

/*
 * 与平台现有回放句柄保持同样的字段语义：
 *   szFilePath  当前 MP4 文件路径
 *   start_time  seek 目标绝对时间（秒），0 表示从文件头开始
 *   stop_time   回放停止绝对时间（秒），0 表示不限制
 *   priv        模块内部 MP4Player 指针，由模块维护
 */
typedef struct
{
    char         szFilePath[512];
    unsigned int start_time;
    unsigned int stop_time;
    unsigned int record_type;
    void        *priv;
} MP4PopHandle;

/* 初始化/销毁回放句柄（Destroy 会关闭内部文件和缓冲区） */
int  MP4PopHandle_Init(MP4PopHandle *h, const char *path,
                       unsigned int start_time, unsigned int stop_time);
void MP4PopHandle_Close(MP4PopHandle *h);

/* 直接打开 MP4 文件，失败返回 NULL */
MP4Player *MP4Player_Open(const char *path);
void       MP4Player_Close(MP4Player *p);

/*
 * seek 到相对文件首帧的时间点，定位到最近的 I 帧分片。
 * @param rel_us 相对文件开始时间(微秒)
 */
int MP4Player_SeekUS(MP4Player *p, uint64_t rel_us);

/*
 * 读取一帧，返回 Annex-B 裸流。
 * @param frame  输出内部缓冲区指针（下次调用前有效）
 * @param size   输出帧大小
 * @param is_key 输出是否关键帧
 * @param pts_us 输出相对文件首帧的显示时间(微秒)
 * @return 0=成功，1=文件结束，-1=读取/解析错误
 */
int MP4Player_ReadFrame(MP4Player *p, uint8_t **frame, int *size,
                        int *is_key, uint64_t *pts_us);

/*
 * 完全对齐平台模板的 MP4 逐帧回放接口。
 *
 * @return 0=成功取帧，-1=读取错误，1=文件读取完毕，2=文件不存在（直接跳过）
 * @note 返回 2 时不会对 p_frame_buffer 做任何写操作。
 */
int pop_one_frame_from_sector_MP4(MP4PopHandle *PoPhandle,
                                  int rec_type,
                                  char **p_frame_buffer,
                                  int *pSize,
                                  int *pFrametype,
                                  int *pKeyframe,
                                  unsigned int *pRealTimeSec,
                                  unsigned int *pRealTimeUSec,
                                  int *pNeedSeek,
                                  int *VipFrame);

#ifdef __cplusplus
}
#endif

#endif /* __MP4_PLAYER_H__ */
