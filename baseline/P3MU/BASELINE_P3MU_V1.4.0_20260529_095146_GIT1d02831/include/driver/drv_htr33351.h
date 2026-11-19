
#ifndef __DRV_HTR33351_H__
#define __DRV_HTR33351_H__

#define HTR33351_CTL_ALL_RESET			_IOW('P', 0x1, struct htr33351_data)
#define HTR33351_CTL_GET_PIN_IO			_IOW('P', 0x2, struct htr33351_data)
#define HTR33351_CTL_SET_PIN_IO			_IOW('P', 0x3, struct htr33351_data)
#define HTR33351_CTL_GET_PIN_VAL		_IOW('P', 0x4, struct htr33351_data)
#define HTR33351_CTL_SET_PIN_VAL		_IOW('P', 0x5, struct htr33351_data)

typedef struct htr33351_data
{
    unsigned int pin;					// 0 ~ 15
    union {
		unsigned int dir;				// 0：输出，1：输入
		unsigned int val;
	} memb;
} HTR33351_DATA_S;

#endif // #ifndef __DRV_HTR33351_H__
