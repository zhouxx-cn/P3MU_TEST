/**
 * @file    ui_res.c
 * @brief   ui_res 资源工具函数实现。
 */
#include "ui_res.h"

const char *ui_res_battery_symbol(int percent)
{
    if (percent >= 90) return UI_ICO_BATTERY_FULL;
    if (percent >= 65) return UI_ICO_BATTERY_3;
    if (percent >= 40) return UI_ICO_BATTERY_2;
    if (percent >= 15) return UI_ICO_BATTERY_1;
    return UI_ICO_BATTERY_EMPTY;
}
