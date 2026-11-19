/**
 * @file    ui_input_app.c
 * @brief   输入源 2：APP 下发 JSON 网络指令 → 统一事件。
 *
 * @details
 *  APP 通过网络（P2P/局域网）下发 JSON，模拟"屏幕点击坐标"或"按键"，
 *  本文件解析后转成 ui_event_t，经 ui_msgq 投递，与物理按键复用同一套交互逻辑。
 *  目的：无实体拇指相机硬件时，用 IPC+APP 远程模拟点击，快速验证 UI 交互。
 *
 *  复用 DVR_SDK/NET/cJSON。支持指令：
 *    {"type":"key","code":"up|down|ok|back","act":"press|release"}
 *    {"type":"click","x":120,"y":200}     // 一次完整点击（按下+抬起）
 *    {"type":"press","x":120,"y":200}     // 仅按下
 *    {"type":"release","x":120,"y":200}   // 仅抬起
 */
#include "ui_input.h"
#include "ui_msgq.h"
#include "cJSON.h"
#include <strings.h>

static ui_key_code_t str_to_key(const char *s)
{
    if (!s) return UI_KEY_NONE;
    if (strcasecmp(s, "up")   == 0) return UI_KEY_UP;
    if (strcasecmp(s, "down") == 0) return UI_KEY_DOWN;
    if (strcasecmp(s, "ok")   == 0) return UI_KEY_OK;
    if (strcasecmp(s, "enter")== 0) return UI_KEY_OK;
    if (strcasecmp(s, "back") == 0) return UI_KEY_BACK;
    return UI_KEY_NONE;
}

static void post_point(int x, int y, int pressed)
{
    ui_event_t evt;
    evt.type = UI_EVT_POINT;
    evt.u.point.x = (int16_t)UI_CLAMP(x, 0, UI_SCREEN_W - 1);
    evt.u.point.y = (int16_t)UI_CLAMP(y, 0, UI_SCREEN_H - 1);
    evt.u.point.pressed = (uint8_t)(pressed ? 1 : 0);
    ui_msgq_post(&evt);
}

static void post_key(ui_key_code_t code, ui_key_act_t act)
{
    ui_event_t evt;
    evt.type = UI_EVT_KEY;
    evt.u.key.code = code;
    evt.u.key.act  = act;
    ui_msgq_post(&evt);
}

int ui_input_app_on_json(const char *json)
{
    cJSON *root, *jtype, *jcode, *jact, *jx, *jy;
    const char *type;
    int rc = UI_ERR;

    if (!json) {
        return UI_ERR_PARAM;
    }

    root = cJSON_Parse(json);
    if (!root) {
        UI_LOGW("app json parse failed: %s", json);
        return UI_ERR;
    }

    jtype = cJSON_GetObjectItem(root, "type");
    if (!cJSON_IsString(jtype)) {
        goto out;
    }
    type = jtype->valuestring;

    if (strcasecmp(type, "key") == 0) {
        jcode = cJSON_GetObjectItem(root, "code");
        jact  = cJSON_GetObjectItem(root, "act");
        ui_key_code_t code = str_to_key(cJSON_IsString(jcode) ? jcode->valuestring : NULL);
        if (code == UI_KEY_NONE) {
            goto out;
        }
        if (cJSON_IsString(jact) && strcasecmp(jact->valuestring, "release") == 0) {
            post_key(code, UI_KEY_ACT_RELEASE);
        } else if (cJSON_IsString(jact) && strcasecmp(jact->valuestring, "press") == 0) {
            post_key(code, UI_KEY_ACT_PRESS);
        } else {
            /* 未指明 act：发一次完整按下+抬起 */
            post_key(code, UI_KEY_ACT_PRESS);
            post_key(code, UI_KEY_ACT_RELEASE);
        }
        rc = UI_OK;
    } else if (strcasecmp(type, "click")  == 0 ||
               strcasecmp(type, "press")  == 0 ||
               strcasecmp(type, "release")== 0) {
        jx = cJSON_GetObjectItem(root, "x");
        jy = cJSON_GetObjectItem(root, "y");
        if (!cJSON_IsNumber(jx) || !cJSON_IsNumber(jy)) {
            goto out;
        }
        if (strcasecmp(type, "press") == 0) {
            post_point(jx->valueint, jy->valueint, 1);
        } else if (strcasecmp(type, "release") == 0) {
            post_point(jx->valueint, jy->valueint, 0);
        } else {
            /* click：按下后抬起，LVGL 会识别为一次点击 */
            post_point(jx->valueint, jy->valueint, 1);
            post_point(jx->valueint, jy->valueint, 0);
        }
        rc = UI_OK;
    } else {
        UI_LOGW("app json unknown type: %s", type);
    }

out:
    cJSON_Delete(root);
    return rc;
}
