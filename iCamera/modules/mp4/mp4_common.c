#include <stdlib.h>
#include <string.h>

#include "mp4_common.h"

int MP4Buf_Init(MP4Buf *b, size_t cap)
{
    if (!b)
    {
        return -1;
    }

    memset(b, 0, sizeof(*b));

    if (cap > 0)
    {
        b->data = (uint8_t *)malloc(cap);
        if (!b->data)
        {
            b->err = 1;
            return -1;
        }
        b->cap = cap;
    }

    return 0;
}

void MP4Buf_Free(MP4Buf *b)
{
    if (!b)
    {
        return;
    }

    free(b->data);
    b->data = NULL;
    b->size = 0;
    b->cap  = 0;
    b->err  = 0;
}

int MP4Buf_Reserve(MP4Buf *b, size_t extra)
{
    size_t need = 0;
    size_t new_cap = 0;
    uint8_t *p = NULL;

    if (!b || b->err)
    {
        return -1;
    }

    /* 防止 size + extra 溢出 */
    if (extra > (size_t)-1 - b->size)
    {
        b->err = 1;
        return -1;
    }

    need = b->size + extra;
    if (need <= b->cap)
    {
        return 0;
    }

    new_cap = (b->cap > 0) ? b->cap : 1024;
    while (new_cap < need)
    {
        if (new_cap > ((size_t)-1) / 2)
        {
            new_cap = need;
            break;
        }
        new_cap *= 2;
    }

    p = (uint8_t *)realloc(b->data, new_cap);
    if (!p)
    {
        b->err = 1;
        return -1;
    }

    b->data = p;
    b->cap  = new_cap;

    return 0;
}

int MP4Buf_Append(MP4Buf *b, const void *data, size_t len)
{
    if (!b || b->err || (!data && len > 0))
    {
        return -1;
    }

    if (MP4Buf_Reserve(b, len))
    {
        return -1;
    }

    if (len > 0)
    {
        memcpy(b->data + b->size, data, len);
        b->size += len;
    }

    return 0;
}

int MP4Buf_Put8(MP4Buf *b, uint8_t v)
{
    if (MP4Buf_Reserve(b, 1))
    {
        return -1;
    }

    b->data[b->size++] = v;
    return 0;
}

int MP4Buf_Put16(MP4Buf *b, uint16_t v)
{
    if (MP4Buf_Reserve(b, 2))
    {
        return -1;
    }

    mp4_wr16(b->data + b->size, v);
    b->size += 2;
    return 0;
}

int MP4Buf_Put24(MP4Buf *b, uint32_t v)
{
    if (MP4Buf_Reserve(b, 3))
    {
        return -1;
    }

    mp4_wr24(b->data + b->size, v);
    b->size += 3;
    return 0;
}

int MP4Buf_Put32(MP4Buf *b, uint32_t v)
{
    if (MP4Buf_Reserve(b, 4))
    {
        return -1;
    }

    mp4_wr32(b->data + b->size, v);
    b->size += 4;
    return 0;
}

int MP4Buf_Put64(MP4Buf *b, uint64_t v)
{
    if (MP4Buf_Reserve(b, 8))
    {
        return -1;
    }

    mp4_wr64(b->data + b->size, v);
    b->size += 8;
    return 0;
}

size_t MP4Buf_BeginBox(MP4Buf *b, const char type[4])
{
    size_t start = 0;

    if (!b || b->err || !type)
    {
        return 0;
    }

    start = b->size;

    /* 先占位 4 字节长度，结束时回填 */
    if (MP4Buf_Reserve(b, 8))
    {
        return start;
    }

    mp4_wr32(b->data + b->size, 0);
    memcpy(b->data + b->size + 4, type, 4);
    b->size += 8;

    return start;
}

int MP4Buf_EndBox(MP4Buf *b, size_t box_start)
{
    size_t box_size = 0;

    if (!b || b->err)
    {
        return -1;
    }

    if (box_start > b->size || b->size - box_start > 0xFFFFFFFFu)
    {
        b->err = 1;
        return -1;
    }

    box_size = b->size - box_start;
    if (box_size < 8)
    {
        b->err = 1;
        return -1;
    }

    mp4_wr32(b->data + box_start, (uint32_t)box_size);
    return 0;
}
