#ifndef __DRV_DEVINFO_H__
#define __DRV_DEVINFO_H__

typedef enum hwid_type
{
    HWID_TYPE_INVALID = 0,
    HWID_TYPE_GPIO,
    HWID_TYPE_ADC,
    HWID_TYPE_ENV,
    HWID_TYPE_BUT,
} HWID_TYPE_E;

#endif // __DEVINFO_H__