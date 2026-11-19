
#ifndef __DRV_HE2866_H__
#define __DRV_HE2866_H__

#define HE2866_CTL_IRCUT _IOW('H', 0x1, struct he2866_data)
#define HE2866_CTL_STEP  _IOW('H', 0x2, struct he2866_data)

#define TEST_HE2866_GPIO
#ifdef TEST_HE2866_GPIO
#define HE2866_TEST_GPIO_HIGH _IO('H', 0x1001)
#define HE2866_TEST_GPIO_LOW  _IO('H', 0x1002)
#endif

typedef struct he2866_data
{
    unsigned char data;
} HE2866_DATA_S;

#endif // #ifndef __HE2866_H__