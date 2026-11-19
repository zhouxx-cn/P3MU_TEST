#!/bin/sh

# 检查参数个数
if [ $# -ne 1 ]; then
    echo "SetCallType.sh Err!!"
    exit 1
fi

# 根据参数执行不同配置
case "$1" in
    0)
        # GPIO方式
        echo "keycall_type 0" > /proc/xrs/hwconf
        echo "keycall_adc_chn -1" > /proc/xrs/hwconf
        echo "keycall_gpio 13,1,1,-1" > /proc/xrs/hwconf

        echo "keycall2_type 0" > /proc/xrs/hwconf
        echo "keycall2_adc_chn -1" > /proc/xrs/hwconf
        echo "keycall2_gpio 22,1,1,-1" > /proc/xrs/hwconf

        echo reset > /proc/xrs/misc
        ;;
    1)
        # ADC方式
        echo "keycall_type 1" > /proc/xrs/hwconf
        echo "keycall_adc_chn 1" > /proc/xrs/hwconf
        echo "keycall_gpio 13,1,1,-1" > /proc/xrs/hwconf

        echo "keycall2_type 0" > /proc/xrs/hwconf
        echo "keycall2_adc_chn -1" > /proc/xrs/hwconf
        echo "keycall2_gpio -1,1,1,-1" > /proc/xrs/hwconf

        echo reset > /proc/xrs/misc
        ;;
    *)
        echo "Invalid argument: $1. Use 0 or 1."
        exit 1
        ;;
esac

exit 0