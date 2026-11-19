
#ifndef __DRV_MX2208A_H__
#define __DRV_MX2208A_H__

#define MX2208A_CTL_IRCUT _IOW('H', 0x1, struct mx2208a_data)
#define MX2208A_CTL_STEP  _IOW('H', 0x2, struct mx2208a_data)

#define TEST_MX2208A_GPIO
#ifdef TEST_MX2208A_GPIO
#define MX2208A_TEST_GPIO_HIGH _IO('H', 0x1001)
#define MX2208A_TEST_GPIO_LOW  _IO('H', 0x1002)
#endif

typedef struct mx2208a_data
{
    unsigned char data;
} MX2208A_DATA_S;

#endif // #ifndef __MX2208A_H__