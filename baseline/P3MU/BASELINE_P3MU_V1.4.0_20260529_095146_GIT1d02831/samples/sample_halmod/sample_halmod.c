#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include "halmod_types.h"
#include "xrsdef.h"
#include "xrsstd.h"
#include "halmod.h"

#ifdef __SUPPORT_FUNC_INSTRUMENT__
void __cyg_profile_func_enter(void *this_fn, void *call_site) {}
void __cyg_profile_func_exit(void *this_fn, void *call_site) {}
#endif

static void _Test_Get_Cmd(char* pCmdStr)
{
    SINT32 i = 0;

    while ('\n' != (pCmdStr[i] = getchar()))
    {
        i++;
    }
}

static int _Test_AlarmOut(HALMOD_ALARM_STAT_E stat)
{
    HALMOD_ALARM_ATTR_S AlarmAttr = {0};

    AlarmAttr.Chn = 0;
    AlarmAttr.Type = HALMOD_ALARM_TYPE_OUT;
    AlarmAttr.Stat = stat;
    printf("[ALARM] Set AlarmOut(%d) Stat(%d).\n", AlarmAttr.Chn, AlarmAttr.Stat);
    if (HalMod_Set_AlarmAttr(0, AlarmAttr) < 0)
    {
        HALMOD_TRACE_ERR("HalMod_Set_AlarmAttr failed!\n");
        return -1;
    }

#if 1
    AlarmAttr.Chn = 1;
    AlarmAttr.Type = HALMOD_ALARM_TYPE_OUT;
    AlarmAttr.Stat = stat;
    printf("[ALARM] Set AlarmOut(%d) Stat(%d).\n", AlarmAttr.Chn, AlarmAttr.Stat);
    if (HalMod_Set_AlarmAttr(0, AlarmAttr) < 0)
    {
        HALMOD_TRACE_ERR("HalMod_Set_AlarmAttr failed!\n");
        return -1;
    }
#endif
    return 0;
}

static int _Test_AlarmIn()
{
    HALMOD_ALARM_ATTR_S AlarmAttr = {0};

    AlarmAttr.Chn = 0;
    AlarmAttr.Type = HALMOD_ALARM_TYPE_IN;
    if (HalMod_Get_AlarmAttr(0, &AlarmAttr) < 0)
    {
        HALMOD_TRACE_ERR("HalMod_Get_AlarmAttr failed!\n");
        return -1;
    }
    printf("[ALARM] Get AlarmIn(%d) Stat(%d).\n", AlarmAttr.Chn, AlarmAttr.Stat);

    return 0;
}

static int _Test_KeyCall()
{
    HALMOD_KEYCALL_ATTR_S KeyCallAttr = {0};

    if (HalMod_Get_KeyCallAttr(0, &KeyCallAttr) < 0)
    {
        HALMOD_TRACE_ERR("HalMod_Get_KeyCallAttr failed!\n");
        return -1;
    }

    printf("[KEYCALL] Get Stat(%d) Type(%d).\n", KeyCallAttr.Stat, KeyCallAttr.Type);

    if (HalMod_Get_KeyCallAttr(1, &KeyCallAttr) < 0)
    {
        HALMOD_TRACE_ERR("HalMod_Get_KeyCallAttr failed!\n");
        return -1;
    }

    printf("[KEYCALL] Get Stat(%d) Type(%d).\n", KeyCallAttr.Stat, KeyCallAttr.Type);
    
    return 0;
}

static int _Test_Light(LIGHT_TYPE_E type, UINT8 data, UINT16 freq_khz, UINT16 freq_hz, HALMOD_LIGHT_ACTION_E action)
{
    HALMOD_LIGHT_ATTR_S LightAttr = {0};

    printf("[LIGHT] Set Light(%d) data(%d) freq_khz(%d) freq_hz(%d) Action(%d)\n", type, data, freq_khz, freq_hz, action);

    LightAttr.Chn = 0;
    LightAttr.Type = type;
    LightAttr.FrqKHZ = freq_khz;
    LightAttr.FrqHZ = freq_hz;
    LightAttr.Data = data;
    LightAttr.Action = action;

    if (HalMod_Set_LightAttr(0, LightAttr) < 0)
    {
        HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
        return -1;
    }

    return 0;
}

static int _Test_TSensor(void)
{
	int nIdx = 0;
	int nValid = 0;
	int szTmperature[8] = {0};
	HALMOD_TSENSOR_ATTR_S TSensorAttr = {0};
	
	TSensorAttr.Enable = 1;
	TSensorAttr.Mode = 0;
	TSensorAttr.CircleTime = 30;
	HalMod_Set_TSensorAttr(0, TSensorAttr);

	memset(&TSensorAttr, 0, sizeof(TSensorAttr));
	HalMod_Get_TSensorAttr(0, &TSensorAttr);
	printf("Enable: %d, Mode: %d, CircleTime: %d\n", TSensorAttr.Enable, TSensorAttr.Mode, TSensorAttr.CircleTime);

	HalMod_Get_TSensorTemperature(0, szTmperature, &nValid);
	for (nIdx = 0; nIdx < nValid; nIdx++) {
		printf("Temp[%d]: %d\n", nIdx, szTmperature[nIdx]);
	}
	
	TSensorAttr.Enable = 1;
	TSensorAttr.Mode = 1;
	TSensorAttr.CircleTime = 30;
	HalMod_Set_TSensorAttr(0, TSensorAttr);

	memset(&TSensorAttr, 0, sizeof(TSensorAttr));
	HalMod_Get_TSensorAttr(0, &TSensorAttr);
	printf("Enable: %d, Mode: %d, CircleTime: %d\n", TSensorAttr.Enable, TSensorAttr.Mode, TSensorAttr.CircleTime);

	usleep(420 * 1000);		// 设置完模式后，初次需等待底层循环采集数据完成（30 * 2 * 7 ms）
	
	HalMod_Get_TSensorTemperature(0, szTmperature, &nValid);
	for (nIdx = 0; nIdx < nValid; nIdx++) {
		printf("Temp[%d]: %d\n", nIdx, szTmperature[nIdx]);
	}

	return 0;
}

static int _Print_BatteryInfo(void)
{
	HALMOD_BATTERY_INFO_S stBatteryInfo = {0};
	
	if (HalMod_Get_BatteryInfo(0, &stBatteryInfo) < 0)
	{
        HALMOD_TRACE_ERR("HalMod_Get_BatteryInfo failed!\n");
        return -1;
	}

	if (stBatteryInfo.Support)
	{
		printf("[Battery] Capaciyt: %d, Status: %d, Voltage: %d, Time2Empty: %d, Technology: %d, Health: %d\n",
			stBatteryInfo.Capacity, stBatteryInfo.Status, stBatteryInfo.VoltageNow,
			stBatteryInfo.TimeToEmptyNow, stBatteryInfo.Technology, stBatteryInfo.Health);
	}
	else
	{
		HALMOD_TRACE_ERR("HalMod_Get_BatteryInfo Result Invalid!\n");
        return -1;
	}
	
	return 0;
}

int main(int argc, char** argv)
{
    char CmdStr[8] = { 0 };
    SINT32 Ret = 0, nCmd = 0;
    HALMOD_LIGHT_ATTR_S LightAttr = { 0 };
    HALMOD_DEVINFO_ATTR_S DevInfo = { 0 };
    int CDStat = 0;
    HALMOD_DEF_KEY_ATTR_S DefKeyStat = {0};

    Ret = HalMod_Open(NULL);
    if (Ret < 0)
    {
        HALMOD_TRACE_ERR("HalMod_Open failed!\n");
        return -1;
    }

    Ret = HalMod_Start(0, 1);
    if (Ret < 0)
    {
        HALMOD_TRACE_ERR("HalMod_Start failed!\n");
        return -1;
    }

    while (1)
    {
        printf("Press q to quit!: \n"
            "\t1 - Get Chip ID.\n"
            "\t2 - Light IR & White Set Light Duty: 0, FrqKHZ: 5KHZ.\n"
            "\t3 - Light IR & White Set Light Duty: 50, FrqKHZ: 5KHZ.\n"
            "\t4 - Light IR & White Set Light Duty: 60, FrqKHZ: 8KHZ.\n"
            "\t5 - Light IR & White Set Light Duty: 100, FrqKHZ: 5KHZ.\n"
            "\t6 - Light IR & White Get Light Config.\n"
            "\t7 - Get Light CDS Stat.\n"
            "\t8 - Get System Reset Stat.\n"
            "\t9 - Set IRCut Mode DAY.\n"
            "\t10- Set IRCut Mode NIGHT.\n"
            "\t11- Set Alarm Out Low.\n"
            "\t12- Set Alarm Out High.\n"
            "\t13- Get Alarm In.\n"
            "\t14- Set Spk Enable 1.\n"
            "\t15- Set Spk Enable 0.\n"
            "\t16- Set LED POWER&NET&FLASH OFF.\n"
            "\t17- Set LED POWER&NET&FLASH ON.\n"
            "\t18- Get KEYCALL STAT.\n"
            "\t19- Set Wifi Power 1.\n"
            "\t20- Set Wifi Power 0.\n"
            "\t21- Get Wifi/4G Support, CDS Type.\n"
            "\t22- Set SD Card Power Disable(0).\n"
            "\t23- Set SD Card Power Enable(1).\n"
            "\t24- Get TSensor Temperature.\n"
            "\t25- Get Battery Info.\n"
            "\t26- Set Charge En & Rst 1.\n"
            "\t27- Set Charge En & Rst 0.\n"
            "\t28- Light BACK Set Light Duty: 0, FrqKHZ: 5KHZ.\n"
            "\t29- Light BACK Set Light Duty: 50, FrqKHZ: 5KHZ.\n"
            "\t30- Light BACK Set Light Duty: 100, FrqKHZ: 5KHZ.\n"
        );

        _Test_Get_Cmd(CmdStr);

        if ((CmdStr[0] == 'q') || (CmdStr[0] == 'Q'))
        {
            TRACE_DBG("get cmd to exit \n");
            break;
        }
        nCmd = atoi(CmdStr);
        TRACE_DBG("nCmd(%d)\n", nCmd);

        switch (nCmd)
        {
            // Get CHIP ID
            case 1:
                if (HalMod_Get_DevInfoAttr(0, &DevInfo) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Get_DevInfoAttr failed!\n");
                    goto out;
                }

                printf("ChipID: %s\n", DevInfo.ChipId);
                break;
            case 2:
                LightAttr.Type = LIGHT_TYPE_IR;
                LightAttr.Action = HALMOD_LIGHT_ACTION_OFF;
                LightAttr.Data = 0;
                LightAttr.FrqKHZ = 5;

                printf("[LIGHT] %s Action(%d) Duty(%d) Freq(%dKHZ).\n",
                    (LightAttr.Type == LIGHT_TYPE_IR) ? "IR" : "WHITE",
                    LightAttr.Action,
                    LightAttr.Data,
                    LightAttr.FrqKHZ);

                if (HalMod_Set_LightAttr(0, LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }

                if (HalMod_Set_LightAttr(1, LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }

                LightAttr.Type = LIGHT_TYPE_WHITE;
                LightAttr.Action = HALMOD_LIGHT_ACTION_OFF;
                LightAttr.Data = 0;
                LightAttr.FrqKHZ = 5;

                printf("[LIGHT] %s Action(%d) Duty(%d) Freq(%dKHZ).\n",
                    (LightAttr.Type == LIGHT_TYPE_IR) ? "IR" : "WHITE",
                    LightAttr.Action,
                    LightAttr.Data,
                    LightAttr.FrqKHZ);

                if (HalMod_Set_LightAttr(0, LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }
				
                if (HalMod_Set_LightAttr(1, LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }

                break;
            case 3:
                LightAttr.Type = LIGHT_TYPE_IR;
                LightAttr.Action = HALMOD_LIGHT_ACTION_ON;
                LightAttr.Data = 50;
                LightAttr.FrqKHZ = 5;

                printf("[LIGHT] %s Action(%d) Duty(%d) Freq(%dKHZ).\n",
                    (LightAttr.Type == LIGHT_TYPE_IR) ? "IR" : "WHITE",
                    LightAttr.Action,
                    LightAttr.Data,
                    LightAttr.FrqKHZ);

                if (HalMod_Set_LightAttr(0, LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }
				
                if (HalMod_Set_LightAttr(1, LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }

                LightAttr.Type = LIGHT_TYPE_WHITE;
                LightAttr.Action = HALMOD_LIGHT_ACTION_ON;
                LightAttr.Data = 50;
                LightAttr.FrqKHZ = 5;

                printf("[LIGHT] %s Action(%d) Duty(%d) Freq(%dKHZ).\n",
                    (LightAttr.Type == LIGHT_TYPE_IR) ? "IR" : "WHITE",
                    LightAttr.Action,
                    LightAttr.Data,
                    LightAttr.FrqKHZ);

                if (HalMod_Set_LightAttr(0, LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }
				
                if (HalMod_Set_LightAttr(1, LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }
				
                break;
            case 4:
                LightAttr.Type = LIGHT_TYPE_IR;
                LightAttr.Action = HALMOD_LIGHT_ACTION_ON;
                LightAttr.Data = 60;
                LightAttr.FrqKHZ = 8;

                printf("[LIGHT] %s Action(%d) Duty(%d) Freq(%dKHZ).\n",
                    (LightAttr.Type == LIGHT_TYPE_IR) ? "IR" : "WHITE",
                    LightAttr.Action,
                    LightAttr.Data,
                    LightAttr.FrqKHZ);

                if (HalMod_Set_LightAttr(0, LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }
				
                if (HalMod_Set_LightAttr(1, LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }

                LightAttr.Type = LIGHT_TYPE_WHITE;
                LightAttr.Action = HALMOD_LIGHT_ACTION_ON;
                LightAttr.Data = 60;
                LightAttr.FrqKHZ = 8;

                printf("[LIGHT] %s Action(%d) Duty(%d) Freq(%dKHZ).\n",
                    (LightAttr.Type == LIGHT_TYPE_IR) ? "IR" : "WHITE",
                    LightAttr.Action,
                    LightAttr.Data,
                    LightAttr.FrqKHZ);

                if (HalMod_Set_LightAttr(0, LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }
				
                if (HalMod_Set_LightAttr(1, LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }
				
                break;
            case 5:
                LightAttr.Type = LIGHT_TYPE_IR;
                LightAttr.Action = HALMOD_LIGHT_ACTION_ON;
                LightAttr.Data = 100;
                LightAttr.FrqKHZ = 5;

                printf("[LIGHT] %s Action(%d) Duty(%d) Freq(%dKHZ).\n",
                    (LightAttr.Type == LIGHT_TYPE_IR)?"IR":"WHITE",
                    LightAttr.Action,
                    LightAttr.Data,
                    LightAttr.FrqKHZ);

                if (HalMod_Set_LightAttr(0, LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }
				
                if (HalMod_Set_LightAttr(1, LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }

                LightAttr.Type = LIGHT_TYPE_WHITE;
                LightAttr.Action = HALMOD_LIGHT_ACTION_ON;
                LightAttr.Data = 100;
                LightAttr.FrqKHZ = 5;

                printf("[LIGHT] %s Action(%d) Duty(%d) Freq(%dKHZ).\n",
                    (LightAttr.Type == LIGHT_TYPE_IR) ? "IR" : "WHITE",
                    LightAttr.Action,
                    LightAttr.Data,
                    LightAttr.FrqKHZ);

                if (HalMod_Set_LightAttr(0, LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }
				
                if (HalMod_Set_LightAttr(1, LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }
				
                break;
            case 6:
                LightAttr.Type = LIGHT_TYPE_IR;

                if (HalMod_Get_LightAttr(0, &LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }

                printf("[LIGHT] %s Action(%d) Duty(%d) Freq(%dKHZ).\n",
                    (LightAttr.Type == LIGHT_TYPE_IR) ? "IR" : "WHITE",
                    LightAttr.Action,
                    LightAttr.Data,
                    LightAttr.FrqKHZ);
					
                if (HalMod_Get_LightAttr(1, &LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }

                printf("[LIGHT] %s Action(%d) Duty(%d) Freq(%dKHZ).\n",
                    (LightAttr.Type == LIGHT_TYPE_IR) ? "IR2" : "WHITE2",
                    LightAttr.Action,
                    LightAttr.Data,
                    LightAttr.FrqKHZ);

                LightAttr.Type = LIGHT_TYPE_WHITE;

                if (HalMod_Get_LightAttr(0, &LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }

                printf("[LIGHT] %s Action(%d) Duty(%d) Freq(%dKHZ).\n",
                    (LightAttr.Type == LIGHT_TYPE_IR) ? "IR" : "WHITE",
                    LightAttr.Action,
                    LightAttr.Data,
                    LightAttr.FrqKHZ);

                if (HalMod_Get_LightAttr(1, &LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }

                printf("[LIGHT] %s Action(%d) Duty(%d) Freq(%dKHZ).\n",
                    (LightAttr.Type == LIGHT_TYPE_IR) ? "IR2" : "WHITE2",
                    LightAttr.Action,
                    LightAttr.Data,
                    LightAttr.FrqKHZ);

                break;
            case 7:
                if (HalMod_Get_LSensorStat(0, &CDStat) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Get_LSensorStat failed!\n");
                    goto out;
                }
                printf("[LIGHT0] LSensorStat = %d.\n", CDStat);
				
                if (HalMod_Get_LSensorStat(1, &CDStat) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Get_LSensorStat failed!\n");
                    goto out;
                }
                printf("[LIGHT1] LSensorStat = %d.\n", CDStat);
                break;
            case 8:
                if (HalMod_Get_DefKeyAttr(0, &DefKeyStat) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Get_DefKeyAttr failed!\n");
                    goto out;
                }
                printf("[LIGHT] SysRstStat = %d.\n", DefKeyStat.Stat);
                break;
            case 9:
                if (HalMod_Set_IrCut_Mode(0, HALMOD_IRCUT_MODE_DAY) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_IrCut_Mode Mode(DAY) failed!\n");
                    goto out;
                }
#if 0
                if (HalMod_Set_IrCut_Mode(1, HALMOD_IRCUT_MODE_DAY) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_IrCut_Mode Mode(DAY) failed!\n");
                    goto out;
                }
#endif
                break;
            case 10:
                if (HalMod_Set_IrCut_Mode(0, HALMOD_IRCUT_MODE_NIGHT) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_IrCut_Mode Mode(NIGHT) failed!\n");
                    goto out;
                }
#if 0
                if (HalMod_Set_IrCut_Mode(1, HALMOD_IRCUT_MODE_NIGHT) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_IrCut_Mode Mode(NIGHT) failed!\n");
                    goto out;
                }
#endif
                break;
            case 11:
                _Test_AlarmOut(HALMOD_ALARM_STAT_LOW);
                break;
            case 12:
                _Test_AlarmOut(HALMOD_ALARM_STAT_HIGH);
                break;
            case 13:
                _Test_AlarmIn();
                break;
            case 14:
                if (HalMod_Set_SpeakerStat(0, 1) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_SpeakerStat Mode(1) failed!\n");
                    goto out;
                }
                break;
            case 15:
                if (HalMod_Set_SpeakerStat(0, 0) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_SpeakerStat Mode(0) failed!\n");
                    goto out;
                }
                break;
            case 16:
                _Test_Light(LIGHT_TYPE_POWER, 0, 0, 0, HALMOD_LIGHT_ACTION_OFF);
                _Test_Light(LIGHT_TYPE_NET, 0, 0, 0, HALMOD_LIGHT_ACTION_OFF);
                _Test_Light(LIGHT_TYPE_FLASH, 0, 0, 0, HALMOD_LIGHT_ACTION_OFF);
                break;
            case 17:
                _Test_Light(LIGHT_TYPE_POWER, 100, 0, 2, HALMOD_LIGHT_ACTION_ON);
                _Test_Light(LIGHT_TYPE_NET, 50, 0, 2, HALMOD_LIGHT_ACTION_ON);
                _Test_Light(LIGHT_TYPE_FLASH, 100, 0, 0, HALMOD_LIGHT_ACTION_ON);
                break;
            case 18:
                _Test_KeyCall();
                break;
            case 19:
                if (HalMod_Set_WifiPower(0, 1) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_WifiPower Mode(1) failed!\n");
                    goto out;
                }
                break;
            case 20:
                if (HalMod_Set_WifiPower(0, 0) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_WifiPower Mode(0) failed!\n");
                    goto out;
                }
                break;
            case 21:
                {
                    int support = 0;
                    HALMOD_WIFI_INFO_S wifiInfo = {0};
                    if (HalMod_Get_WifiInfo(0, &wifiInfo))
                    {
                        HALMOD_TRACE_ERR("HalMod_Get_WifiSupport failed!\n");
                        goto out;
                    }
                    HALMOD_TRACE_INFO("wifi support = %d, model = %s, gen = %#x, freq = %s %s\n", 
                        wifiInfo.Support, wifiInfo.Model, wifiInfo.Gen, HALMOD_WIFI_SUPPORT_5G(wifiInfo.Freq)?"5G":"", HALMOD_WIFI_SUPPORT_2_4G(wifiInfo.Freq)?"2.4G":"");

                    if (HalMod_Get_4GSupport(0, &support))
                    {
                        HALMOD_TRACE_ERR("HalMod_Get_4GSupport failed!\n");
                        goto out;
                    }
                    HALMOD_TRACE_INFO("4G support = %d\n", support);

                    if (HalMod_Get_BtSupport(0, &support))
                    {
                        HALMOD_TRACE_ERR("HalMod_Get_BtSupport failed!\n");
                        goto out;
                    }
                    HALMOD_TRACE_INFO("BT support = %d\n", support);

                    if (HalMod_Get_CdsType(0, &support))
                    {
                        HALMOD_TRACE_ERR("HalMod_Get_CdsType failed!\n");
                        goto out;
                    }
                    HALMOD_TRACE_INFO("CDS TYPE = %s\n", support?"ADC":"GPIO");
                }
                break;
            case 22:
                HalMod_Set_SdPower(0, 0);
            break;
            case 23:
                HalMod_Set_SdPower(0, 1);
            break;
			case 24:
				_Test_TSensor();
            break;
			case 25:
				_Print_BatteryInfo();
				break;
            case 26:
                HalMod_Set_ChargeEnable(0,1);
                HalMod_Set_ChargeReset(0,1);
                break;
            case 27:    
                HalMod_Set_ChargeEnable(0,0);
                HalMod_Set_ChargeReset(0,0);
                break;
            case 28:
                LightAttr.Type = LIGHT_TYPE_BACK;
                LightAttr.Action = HALMOD_LIGHT_ACTION_OFF;
                LightAttr.Data = 0;
                LightAttr.FrqKHZ = 5;

                printf("[LIGHT] %s Action(%d) Duty(%d) Freq(%dKHZ).\n",
                    (LightAttr.Type == LIGHT_TYPE_IR) ? "IR" : "WHITE",
                    LightAttr.Action,
                    LightAttr.Data,
                    LightAttr.FrqKHZ);

                if (HalMod_Set_LightAttr(0, LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }
                break;
            case 29:
                LightAttr.Type = LIGHT_TYPE_BACK;
                LightAttr.Action = HALMOD_LIGHT_ACTION_ON;
                LightAttr.Data = 50;
                LightAttr.FrqKHZ = 5;

                printf("[LIGHT] %s Action(%d) Duty(%d) Freq(%dKHZ).\n",
                    (LightAttr.Type == LIGHT_TYPE_IR) ? "IR" : "WHITE",
                    LightAttr.Action,
                    LightAttr.Data,
                    LightAttr.FrqKHZ);

                if (HalMod_Set_LightAttr(0, LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }
                break;
            case 30:
                LightAttr.Type = LIGHT_TYPE_BACK;
                LightAttr.Action = HALMOD_LIGHT_ACTION_ON;
                LightAttr.Data = 100;
                LightAttr.FrqKHZ = 5;

                printf("[LIGHT] %s Action(%d) Duty(%d) Freq(%dKHZ).\n",
                    (LightAttr.Type == LIGHT_TYPE_IR) ? "IR" : "WHITE",
                    LightAttr.Action,
                    LightAttr.Data,
                    LightAttr.FrqKHZ);

                if (HalMod_Set_LightAttr(0, LightAttr) < 0)
                {
                    HALMOD_TRACE_ERR("HalMod_Set_LightAttr failed!\n");
                    goto out;
                }
                break;
            default:
                printf("unsupported nCmd, %d\n", nCmd);
                break;
        }
    }

out:
    Ret = HalMod_Stop(0);
    if (Ret < 0)
    {
        HALMOD_TRACE_ERR("HalMod_Stop failed!\n");
        return -1;
    }

    Ret = HalMod_Close(0);
    if (Ret < 0)
    {
        HALMOD_TRACE_ERR("HalMod_Close failed!\n");
        return -1;
    }

    return 0;
}
