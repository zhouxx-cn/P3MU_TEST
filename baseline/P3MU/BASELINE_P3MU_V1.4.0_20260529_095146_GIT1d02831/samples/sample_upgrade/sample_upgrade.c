#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "upgrade.h"
#include "mysystem.h"

#ifdef __SUPPORT_FUNC_INSTRUMENT__
void __cyg_profile_func_enter(void *this_fn, void *call_site) {}
void __cyg_profile_func_exit(void *this_fn, void *call_site) {}
#endif

#define UPGRADE_BIN_FILE    "upgrade.bin"
#define SINGLE_BUFFER_SIZE  (16*1024*1024)

void Upgrade_Callback(SINT32 wParam, SINT32* plParam)
{
    printf("Process = %d\n", wParam);
}

static void _Test_Get_Cmd(char* pCmdStr)
{
    SINT32 i = 0;

    while ('\n' != (pCmdStr[i] = getchar()))
    {
        i++;
    }
}

static SINT32 _ReadBinToMemInfo(PUPGRADE_MEM_INFO_S pMemInfo)
{
    FILE* fp = NULL;
    long filesize = 0;
    int i = 0;

    fp = fopen(UPGRADE_BIN_FILE, "rb");
    if (NULL == fp)
    {
        UPGRADE_TRACE_ERR("fopen file(%s) failed!\n", UPGRADE_BIN_FILE);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    pMemInfo->Cnt = (filesize % SINGLE_BUFFER_SIZE) ? (filesize / SINGLE_BUFFER_SIZE + 1) : (filesize / SINGLE_BUFFER_SIZE);

    for (i = 0; i < pMemInfo->Cnt; i++)
    {
        pMemInfo->Mem[i].Size = (filesize - i * SINGLE_BUFFER_SIZE) > SINGLE_BUFFER_SIZE ? SINGLE_BUFFER_SIZE : (filesize - i * SINGLE_BUFFER_SIZE);
        pMemInfo->Mem[i].Addr = malloc(pMemInfo->Mem[i].Size);
        if (pMemInfo->Mem[i].Addr == NULL)
        {
            UPGRADE_TRACE_ERR("Malloc mem(%d) bytes failed!\n", pMemInfo->Mem[i].Size);
            goto err_out;
        }

        memset(pMemInfo->Mem[i].Addr, 0, pMemInfo->Mem[i].Size);

        if (fread(pMemInfo->Mem[i].Addr, pMemInfo->Mem[i].Size, 1, fp) != 1)
        {
            UPGRADE_TRACE_ERR("read file(%s) failed!\n", UPGRADE_BIN_FILE);
            goto err_out;
        }

        UPGRADE_TRACE_INFO("[MEM%d] Addr = %p, Size = %d.\n", i, pMemInfo->Mem[i].Addr, pMemInfo->Mem[i].Size);
    }

    UPGRADE_TRACE_INFO("Package  Magic(%s).\n", ((PUPGRADE_FILE_INFO_HEADER_S)(pMemInfo->Mem[0].Addr))->Magic);

    fclose(fp);

    return 0;

err_out:
    if (fp) fclose(fp);

    for (i = 0; i < pMemInfo->Cnt; i++)
    {
        if (pMemInfo->Mem[i].Addr)
        {
            free(pMemInfo->Mem[i].Addr);
            pMemInfo->Mem[i].Addr = NULL;
        }
    }

    return 0;
}

const char* mtdTbl = "/proc/mtd";
static SINT32 _Test_UpgradePack()
{
    FILE* fp = NULL;
    int idx = 0;
    char buf[1024] = {0};
    char cmd[1024] = {0};
    unsigned int partsize = 0, erasesize = 0;
    char partname[MAX_FILE_NAME_LEN] = {0};
    SINT32 Ret = 0;
    UPGRADE_PACK_INFO_S packInfo = {0};
    const char *base_dir = "/nfsroot";

    fp = fopen(mtdTbl, "r");
    if (NULL == fp) {
        UPGRADE_TRACE_ERR("fopen file(%s) failed!\n", mtdTbl);
        return -1;
    }

    while (!feof(fp)) {
        if (fgets(buf, sizeof(buf), fp) != NULL) {
            if (buf[0] != 'm' || buf[1] != 't' || buf[2] != 'd') {
                continue;
            }
            sscanf(buf, "mtd%d: %x %x \"%s\"", &idx, &partsize, &erasesize, partname);
            partname[strlen(partname) - 1] = '\0';
            printf("index(%d) size(%x) erasesize(%x) name(%s)\n", idx, partsize, erasesize, partname);
            snprintf(cmd, sizeof(cmd), "dd if=/dev/mtd%d of=%s/%s.bin", idx, base_dir, partname);
            printf("cmd: %s\n", cmd);
            mysystem(cmd);
        }
    }
    fclose(fp);

    // tar
    snprintf(cmd, sizeof(cmd), "cd %s;rm -rf ./tardir cfg.tar; mkdir tardir", base_dir);
    mysystem(cmd);
    snprintf(cmd, sizeof(cmd), "mkdir %s/tardir/etc", base_dir);
    mysystem(cmd);
    sprintf(cmd, "cp -v /mnt/mtd/etc/hosts %s/tardir/etc/hosts", base_dir);
    mysystem(cmd);
    sprintf(cmd, "cd %s/tardir;tar -cvf cfg.tar etc/;mv cfg.tar ../;cd ..", base_dir);
	mysystem(cmd);

    packInfo.packageType = PACKAGE_TYPE_RESUME;
    packInfo.checkType = CHECK_TYPE_CRC32;
    packInfo.encryptType = ENCRYPT_TYPE_NONE;
    packInfo.hwCodeMsk = 0xffffffff;
    strncpy(packInfo.pkgVer, "V1.0.0_20221212", strlen("V1.0.0_20221212"));
    strncpy(packInfo.pkgBaseVer, "V1.1.0_20241212", strlen("V1.1.0_20241212"));
    packInfo.boardTypeList.count = 1;
    strncpy(packInfo.boardTypeList.BoardTypeList[0], "TYPE_IPG-5950PCS-AI", strlen("TYPE_IPG-5950PCS-AI"));
    packInfo.imgCnt = 0;
    // uboot
    packInfo.imgList[packInfo.imgCnt].partType = PARTITION_TYPE_BOOT;
    packInfo.imgList[packInfo.imgCnt].upType = UPGRADE_TYPE_ERASURE;
    strncpy(packInfo.imgList[packInfo.imgCnt].imgPath, "uboot.bin", strlen("uboot.bin"));
    packInfo.imgCnt += 1;
    // env
    packInfo.imgList[packInfo.imgCnt].partType = PARTITION_TYPE_ENV;
    packInfo.imgList[packInfo.imgCnt].upType = UPGRADE_TYPE_ERASURE;
    strncpy(packInfo.imgList[packInfo.imgCnt].imgPath, "env.bin", strlen("env.bin"));
    packInfo.imgCnt += 1;
    // rootfs
    packInfo.imgList[packInfo.imgCnt].partType = PARTITION_TYPE_ROOTFS;
    packInfo.imgList[packInfo.imgCnt].upType = UPGRADE_TYPE_ERASURE;
    strncpy(packInfo.imgList[packInfo.imgCnt].imgPath, "rootfs.bin", strlen("rootfs.bin"));
    packInfo.imgCnt += 1;
    // tar
    if (packInfo.packageType == PACKAGE_TYPE_UPGRADE)
    {
        packInfo.imgList[packInfo.imgCnt].partType = PARTITION_TYPE_USR;
        packInfo.imgList[packInfo.imgCnt].upType = UPGRADE_TYPE_TAR;
        strncpy(packInfo.imgList[packInfo.imgCnt].imgPath, "cfg.tar", strlen("cfg.tar"));
        strncpy(packInfo.imgList[packInfo.imgCnt].dstPath, "/mnt/mtd", strlen("/mnt/mtd"));
        packInfo.imgCnt += 1;
    }

    Ret = Upgrade_ImgPack(&packInfo, "./upgrade.bin");
    if (Ret != 0) {
        printf("Upgrade_ImgPack fail.\n");
        return -1;
    }

    return 0;
}

SINT32 main(SINT32 argc, char** argv)
{
    char CmdStr[8] = {0};
    SINT32 RtVal = 0, nCmd = 0;
    UPGRADE_MEM_INFO_S MemInfo = {0};
    UPGRADE_KEY_S key = {0};

    while (1) {
        printf(
            "Press q to quit!: \n"
            "\t1-Check Upgrade Mem(Upgrade_Mem_CheckEx)\n"
            "\t2-Mem Upgrade (Upgrade_Mem_UpdateEx)\n"
            "\t3-Check Upgrade Mem(Upgrade_Mem_Check)\n"
            "\t4-Mem Upgrade (Upgrade_Mem_Update)\n"
            "\t5-Upgrade Pack (Upgrade_ImgPack)\n");

        _Test_Get_Cmd(CmdStr);

        if ((CmdStr[0] == 'q') || (CmdStr[0] == 'Q')) {
            TRACE_DBG("get cmd to exit \n");
            break;
        }
        nCmd = atoi(CmdStr);
        TRACE_DBG("nCmd(%d)\n", nCmd);
        if (nCmd != 5) {
            RtVal = _ReadBinToMemInfo(&MemInfo);
            if (RtVal < 0) {
                UPGRADE_TRACE_ERR("_ReadBinToMemInfo failed with %d!\n", RtVal);
                return -1;
            }

            RtVal = Upgrade_Open(Upgrade_Callback, "TYPE_IPG-5950PCS-AI");
            if (RtVal < 0) {
                UPGRADE_TRACE_ERR("Upgrade_Open failed with %d!\n", RtVal);
                return -1;
            }
        }

        switch (nCmd) {
            case 1:
                RtVal = Upgrade_Mem_CheckEx(0, &MemInfo, &key);
                if (RtVal < 0) {
                    UPGRADE_TRACE_ERR("Upgrade_Mem_CheckEx failed with %d!\n", RtVal);
                    goto out;
                }

                UPGRADE_TRACE_INFO("Upgrade_Mem_CheckEx Success, HWCode(%#x) Ver(%s) BaseVer(%s)\n", key.HwIdCode,
                                   key.UpgradeBaseVersion, key.UpgradePackageVersion);
                break;

            case 2:
                RtVal = Upgrade_Mem_UpdateEx(0, &MemInfo, NULL);
                if (RtVal < 0) {
                    UPGRADE_TRACE_ERR("Upgrade_Mem_UpdateEx failed with %d!\n", RtVal);
                    goto out;
                }
                UPGRADE_TRACE_INFO("Upgrade Success!\n");
                break;
            case 3:
                if (MemInfo.Cnt != 1) {
                    UPGRADE_TRACE_ERR("Upgrade Mem Saved Not in One Buffer, Use Upgrade_Mem_CheckEx!\n");
                    goto out;
                }

                RtVal = Upgrade_Mem_Check(0, MemInfo.Mem[0].Addr, &key);
                if (RtVal < 0) {
                    UPGRADE_TRACE_ERR("Upgrade_Mem_Check failed with %d!\n", RtVal);
                    goto out;
                }

                UPGRADE_TRACE_INFO("Upgrade_Mem_CheckEx Success, HWCode(%#x) Ver(%s) BaseVer(%s)\n", key.HwIdCode,
                                   key.UpgradeBaseVersion, key.UpgradePackageVersion);

                break;

            case 4:
                if (MemInfo.Cnt != 1) {
                    UPGRADE_TRACE_ERR("Upgrade Mem Saved Not in One Buffer, Use Upgrade_Mem_Update!\n");
                    goto out;
                }

                RtVal = Upgrade_Mem_Update(0, MemInfo.Mem[0].Addr, MemInfo.Mem[0].Size, NULL);
                if (RtVal < 0) {
                    UPGRADE_TRACE_ERR("Upgrade_Mem_Update failed with %d!\n", RtVal);
                    goto out;
                }

                UPGRADE_TRACE_INFO("Upgrade Success!\n");

                break;

            case 5:
                RtVal = _Test_UpgradePack();
                break;

            default:
                printf("unsupported nCmd, %d\n", nCmd);
                break;
        }
    }

out:
    Upgrade_Close(0);

    return 0;
}
