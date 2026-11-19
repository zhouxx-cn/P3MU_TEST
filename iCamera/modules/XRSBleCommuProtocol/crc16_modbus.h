#ifndef _CRC16_MODBUS_H_
#define _CRC16_MODBUS_H_

#include <stdint.h>

#define HIG_UINT16(a)   ( ((a)>>8) & 0xFF )
#define LOW_UINT16(a)   ( (a) & 0xFF )

#define HIG_UINT8(a)    ( ((a)>>4) & 0x0F )
#define LOW_UINT8(a)    ( (a) & 0x0F )

#ifdef __cplusplus
extern "C" {
#endif

uint16_t Crc16Modbus(uint8_t *ptr, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif  /* crc16_modbus.h */

