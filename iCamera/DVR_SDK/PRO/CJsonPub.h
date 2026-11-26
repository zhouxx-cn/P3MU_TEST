#ifndef __CJSON_PUB_H__
#define __CJSON_PUB_H__

#include <stdio.h>
#include "CProManager.h"
#include "LogManager.h"
#include "DEVICEVERSION.h"

#define FORMAT_ERR_MESSAGE      "JSON Format Error!"
#define CMD_INFO_ERR_MESSAGE    "CmdInfo Error!"
#define RESPOND_JSON_ERR        "Respond Json Error!"
#define TEST_MODE_FAIL          "TestMode Set Error!"
#define SUCCESS_MESSAGE         "Successful!"

#define JSON_ERROR(format...)            LOG_E(LOG_MODULE_JSONSYS, format)
#define JSON_WRAN(format...)             LOG_W(LOG_MODULE_JSONSYS, format)
#define JSON_INFO(format...)             LOG_I(LOG_MODULE_JSONSYS, format)
#define JSON_DEBUG(format...)            LOG_D(LOG_MODULE_JSONSYS, format)
#define JSON_HEX(desc, data, len)        LOG_H(LOG_MODULE_JSONSYS, desc, data, len)

int Json_Version(char *nBuffer, int nBufferLenth);
int CJsonPubRespond(char *nBuffer, int nBufferLenth, int nStatus, int nTestType);

#endif

