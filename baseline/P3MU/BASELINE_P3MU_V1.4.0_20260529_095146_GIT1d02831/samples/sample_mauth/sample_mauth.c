#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "mauth.h"

#ifdef __SUPPORT_FUNC_INSTRUMENT__
void __cyg_profile_func_enter(void *this_fn, void *call_site) {}
void __cyg_profile_func_exit(void *this_fn, void *call_site) {}
#endif

int main(int argc, char **argv)
{
    int Ret = 0;
    MAUTH_DEV_ATTR_S DevAttr = {0};

    Ret = MAuth_Dev_Open();
    if (Ret < 0)
    {
        MAUTH_TRACE_ERR("MAuth_Dev_Open Failed with %d.\n", Ret);
        return -1;
    }

    Ret = MAuth_Dev_GetDevAttr(0, &DevAttr);
    if (Ret < 0)
    {
        MAUTH_TRACE_ERR("MAuth_Dev_GetDevAttr Failed with %d.\n", Ret);
        return -1;
    }

    DevAttr.CustomID = 0xFF;
    snprintf(DevAttr.AuthCode, LICENSE_CODE_MAX_LEN, "AUTH-CODE");
    DevAttr.BoardID = 0xFFEE;
    DevAttr.Function = 0xAA;
    snprintf(DevAttr.SerialNum, MAX_SERIAL_NUM_LEN, "SERIAL-NUMBER");
    snprintf(DevAttr.DIDInfo.szDid, MAX_DID_LEN, "XRS-DID-DATA");
    DevAttr.DIDInfo.szLen = strlen(DevAttr.DIDInfo.szDid);
    DevAttr.DIDInfo.szType = 1;
    snprintf(DevAttr.BoardTypeName, BOARD_TYPE_NAME_LEN, "IPG-5950PCS-AI");
 
    Ret = MAuth_Dev_SetDevAttr(0, &DevAttr);
    if (Ret < 0)
    {
        MAUTH_TRACE_ERR("MAuth_Dev_GetDevAttr Failed with %d.\n", Ret);
        return -1;
    }

    memset(&DevAttr, 0, MAUTH_DEV_ATTR_S_LEN);
    Ret = MAuth_Dev_GetDevAttr(0, &DevAttr);
    if (Ret < 0)
    {
        MAUTH_TRACE_ERR("MAuth_Dev_GetDevAttr Failed with %d.\n", Ret);
        return -1;
    }

    printf("CRC32 = %X\n", DevAttr.Crc32);
    printf("DevAttr.CustomID = %X\n", DevAttr.CustomID);
    printf("DevAttr.BoardID = %X\n", DevAttr.BoardID);
    printf("DevAttr.BoardTypeName = %s\n", DevAttr.BoardTypeName);
    printf("DevAttr.Function = %X\n", DevAttr.Function);
    printf("DevAttr.AuthCode = %s\n", DevAttr.AuthCode);
    printf("DevAttr.SerialNum = %s\n", DevAttr.SerialNum);
    printf("DevAttr.DIDInfo.szType = %d\n", DevAttr.DIDInfo.szType);
    printf("DevAttr.DIDInfo.szLen = %d\n", DevAttr.DIDInfo.szLen);
    printf("DevAttr.DIDInfo.szDid = %s\n", DevAttr.DIDInfo.szDid);

    // USER
    MAUTH_USR_DATA_S UsrAttr = { 0 };

    Ret = MAuth_Dev_GetUsrDat(0, &UsrAttr);
    if (Ret < 0)
    {
        MAUTH_TRACE_ERR("MAuth_Dev_GetUsrDat Failed with %d.\n", Ret);
        return -1;
    }

    UsrAttr.Type = 0xAA;
    UsrAttr.Size += 1;
    strcpy(UsrAttr.Data, "USR DATA!");

    Ret = MAuth_Dev_SetUsrDat(0, &UsrAttr);
    if (Ret < 0)
    {
        MAUTH_TRACE_ERR("MAuth_Dev_SetUsrDat Failed with %d.\n", Ret);
        return -1;
    }

    memset(&UsrAttr, 0, MAUTH_USR_DATA_S_LEN);

    Ret = MAuth_Dev_GetUsrDat(0, &UsrAttr);
    if (Ret < 0)
    {
        MAUTH_TRACE_ERR("MAuth_Dev_GetUsrDat Failed with %d.\n", Ret);
        return -1;
    }
    printf("=======================================\n");
    printf("USR-DATA type = %X, size = %d, data = %s\n", UsrAttr.Type, UsrAttr.Size, UsrAttr.Data);
    printf("=======================================\n");

    MAuth_Dev_Close(0);

    return 0;
}