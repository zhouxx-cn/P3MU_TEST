#include "PUB_Language.h"

#define DEBUG

#ifdef DEBUG
#define _debug(x...) do {printf("[debug][%s %d %s]", \
	__FILE__,__LINE__,__FUNCTION__);printf(x);} while (0)
#define _info(x...) do {printf("[info][%s %d %s]", \
	__FILE__,__LINE__,__FUNCTION__);printf(x);} while (0)
#define _error(x...) do {printf("[error][%s %d %s]", \
	__FILE__,__LINE__,__FUNCTION__);printf(x);} while (0)
#else
#define _debug(x...) do {;} while (0)
#define _info(x...) do {printf("[info][%s %d %s]", \
	__FILE__,__LINE__,__FUNCTION__);printf(x);} while (0)
#define _error(x...) do {printf("[error][%s %d %s]", \
	__FILE__,__LINE__,__FUNCTION__);printf(x);} while (0)
#endif

#define CUSTOMINFO_SECTION_SPEECH "PUB"
#define CUSTOMINFO_KEY_SPEECHLANGUAGE "SpeechLanguage"
#define EDVRCFG_SECTION_PUB "PUB"
#define EDVRCFG_KEY_NVRSETLANGUAGE "NvrSetLanguageType"
#define EDVRCFG_KEY_SPEECHLANGUAGE "SpeechLanguage"

static eSpeechLanguage s_vioceLanguage = eSpeechLanguage_Chinese; //设备端生效的语音类型
static int s_getcustomflag = 0; //只获取一次定制文件中的语言配置
static eSpeechLanguage s_customValue = eSpeechLanguage_Chinese; //custominfo.ini 中语言定制的值

//SystemLanguage[] 和 language_type 的元素都是按照eSystemLanguage枚举顺序排列
AUDIO_ALARM_LANGUAGE_TYPE language_type[37] = {
	{0, "Chinese","chinese", eSpeechLanguage_Chinese, eLanguage_Chinese},//简体中文
	{0, "English","english", eSpeechLanguage_English, eLanguage_English},//英语
	{0, "SouthKorean","korean", eSpeechLanguage_Num, eLanguage_Korean},//韩国
	{0, "Japanese","", eSpeechLanguage_Num,eLanguage_Japanese},//日语
	{0, "Italian","", eSpeechLanguage_Num, eLanguage_Italian},//意大利语
	{0, "Russian","chinese", eSpeechLanguage_Russian, eLanguage_Russian},//俄罗斯语
	{0, "Spanish","Spanish", eSpeechLanguage_Spanish, eLanguage_Spanish},//西班牙语
	{0, "Portuguese","portuguese", eSpeechLanguage_Portuguese, eLanguage_Portuguese},//葡萄牙语
	{0, "Persian","farsi", eSpeechLanguage_Persian, eLanguage_Persian},//波斯语
	{0, "Arabic","", eSpeechLanguage_Num, eLanguage_Arabic},//阿拉伯文
	{0, "Bulgarian","", eSpeechLanguage_Num, eLanguage_Bulgarian},//保加利亚语
	{0, "ChineseTW","", eSpeechLanguage_Num, eLanguage_Chinese_Traditional},//繁体中文
	{0, "Custom","", eSpeechLanguage_Num, eLanguage_Custom},//定制语言
	{0, "Croation","", eSpeechLanguage_Num, eLanguage_Croation},//克罗地亚语
	{0, "Czech","", eSpeechLanguage_Num, eLanguage_Czech },//捷克语
	{0, "Danish","", eSpeechLanguage_Num, eLanguage_Danish },//丹麦语
	{0, "Dutch","", eSpeechLanguage_Num, eLanguage_Dutch },//荷兰（德语）
	{0, "German","", eSpeechLanguage_Num, eLanguage_German },//德语
	{0, "Estonian","", eSpeechLanguage_Num, eLanguage_Estonian},//爱沙尼亚语
	{0, "Finnish","", eSpeechLanguage_Num, eLanguage_Finnish},//芬兰语
	{0, "French","", eSpeechLanguage_Num, eLanguage_French},//法语
	{0, "Greek","", eSpeechLanguage_Num, eLanguage_Greek},//希腊语
	{0, "Hebrew","", eSpeechLanguage_Num, eLanguage_Hebrew},//希伯来语
	{0, "Hungarian","", eSpeechLanguage_Num, eLanguage_Hungarian},//匈牙利语
	{0, "Latvian","", eSpeechLanguage_Num, eLanguage_Latvian},//拉脱维亚语
	{0, "Lithuanian","", eSpeechLanguage_Num, eLanguage_Lithuanian},//立陶宛语
	{0, "Norwegian","", eSpeechLanguage_Num, eLanguage_Norwegian},//挪威语
	{0, "Polish","", eSpeechLanguage_Num, eLanguage_Polish},//波兰语
	{0, "Romanian","", eSpeechLanguage_Num, eLanguage_Romanian},//罗马尼亚语
	{0, "Slovak","", eSpeechLanguage_Num, eLanguage_Slovak},//斯洛伐克语
	{0, "Slovenian","", eSpeechLanguage_Num, eLanguage_Slovenian},//斯洛维尼亚语
	{0, "Swedish","", eSpeechLanguage_Num, eLanguage_Swedish},//瑞典语
	{0, "Thai","thai", eSpeechLanguage_Thai, eLanguage_Thai},//泰语
	{0, "Turkish","turkish", eSpeechLanguage_Turkish, eLanguage_Turkish},//土耳其语
    {0, "Vietnamese","vietnamese", eSpeechLanguage_Vietnamese, eLanguage_Vietnamese},//越南语
    {0, "Indonesian","indonesian", eSpeechLanguage_Indonesian, eLanguage_Indonesian},//印尼语
    //{0, "NorthKorean","", eSpeechLanguage_KoreanLanguage, eLanguage_KoreanLanguage},//朝鲜语
};

/**
 * @description: 获取设备端生效的语音类型
 * @return {*}
 * @param {eSpeechLanguage} *peLanguage: IPC语言枚举
 */
int PUB_GetLanguageFromCfg(eSpeechLanguage *peLanguage)
{
    *peLanguage = s_vioceLanguage;
    return 0;
}

/**
 * @description: 获取分区支持的语言类型
 * @return {*}
 * @param {char} *dirName：分区语音文件名
 * @param {int} *result： 1 支持，0 不支持
 */
int PUB_GetDeviceSupportLanguageType(char *dirName, int *result)
{
	DIR *dir = NULL;
	struct dirent *ent = NULL;
	char dirPath[128] = {0};

    //组装文件路径
	snprintf(dirPath, sizeof(dirPath), "%s/%s", SPEENC_LANGUAGE_PATH, dirName);

	dir = opendir(dirPath);
	if(dir != NULL)
	{
        //遍历目录下所有文件
		while((ent = readdir(dir)) != NULL)
		{
			//主要找到当前语言目录下有.g711u或.amr的普通文件则认为当前设备支持该种语言
			if(DT_REG == ent->d_type && (strstr(ent->d_name, ".g711u") || strstr(ent->d_name, ".amr")))
			{
				*result =  1;
			}
            else
            {
                *result =  0;
            }
		}
        closedir(dir);
	}
	return 0;
}

/**
 * @description: 从配置文件中获取语言类型
 * @return {*}
 */
int PUB_GetLanguageTypeFromCfg()
{
    int ret = 0;
    eSpeechLanguage value = eSpeechLanguage_Chinese;

    if(0 == s_getcustomflag)
    {
        //获取custominfo.ini 中语言值
        CConfig *cfg = CConfig::NewInstance();
        if (cfg)
        {
            ret = cfg->LoadFile(CUSTOM_CONFIG_FILE);
            if (0 == ret)
            {
                s_getcustomflag = 1;
                value = (eSpeechLanguage)(cfg->GetValue(CUSTOMINFO_SECTION_SPEECH, CUSTOMINFO_KEY_SPEECHLANGUAGE, (int)eSpeechLanguage_Chinese));
            }
            CConfig::ReleaseInstance(cfg);
        }
    }

    if (0 == value)
    {
        //语言未被定制，查看后端配置文件是否配置
        s_customValue = eSpeechLanguage_Chinese;
        CConfig *pCfg = CConfig::GetInstance();
        if(pCfg)
        {
            value = (eSpeechLanguage)(pCfg->GetValue(EDVRCFG_SECTION_PUB, EDVRCFG_KEY_SPEECHLANGUAGE, (int)eSpeechLanguage_Chinese));
        }
    }
    else
    {
        //语言被定制
        s_customValue = value;
    }

    //默认中、英、泰三国语言通用。
    if (eSpeechLanguage_Chinese != value && eSpeechLanguage_English != value && eSpeechLanguage_Thai != value)
    {
        s_vioceLanguage = eSpeechLanguage_Chinese;
    }
    else
    {
        s_vioceLanguage = value;
    }

    //存在语音定制文件信息，直接从文件中获取语言类型
    if (access(CUSTOM_ALL_SPEECH_TAR_INFO_FILE, F_OK) == 0)
    {
        CConfig *cfg = CConfig::NewInstance();
        if (cfg)
        {
            ret = cfg->LoadFile(CUSTOM_ALL_SPEECH_TAR_INFO_FILE);
            if (0 == ret)
            {
                if (cfg->CheckKeyExist(CUSTOMINFO_SECTION_SPEECH, CUSTOMINFO_KEY_SPEECHLANGUAGE))
                {
                    s_vioceLanguage = (eSpeechLanguage)cfg->GetValue(CUSTOMINFO_SECTION_SPEECH, CUSTOMINFO_KEY_SPEECHLANGUAGE, eSpeechLanguage_Chinese);
                    OPEN_LOG_W("There is voice customization information: [%d]", s_vioceLanguage);
                }
            }
            CConfig::ReleaseInstance(cfg);
        }
    }

	return 0;
}

/**
 * @description: 语言功能初始化
 * @return {*}
 */
int PUB_LanguageInit()
{
	for(unsigned int i = 0; i < sizeof(language_type) / sizeof(language_type[0]); i++)
	{
		if(strlen(language_type[i].m_bDirName) <= 0)
		{
			continue;
		}

		PUB_GetDeviceSupportLanguageType(language_type[i].m_bDirName, &(language_type[i].m_bVaild));
	}

    PUB_GetLanguageTypeFromCfg();

    //设置语言
    ZWSDK_PresetConfig sPresetConfig = {};
    ZWSys_GetPresetConfig(&sPresetConfig);
    sPresetConfig.bLangFlag = 1;
    sPresetConfig.eLang = s_vioceLanguage;
	ZWSys_SetPresetConfig (sPresetConfig);

    return 0;
}

/**
 * @description: 检查是否支持 nvr_language 指向的语言
 * @return {*}
 * @param {eSystemLanguage} nvr_language 目标语言
 */
int PUB_CheckLanguageType(eSpeechLanguage nvr_language)
{
	for(unsigned int i = 0; i < sizeof(language_type) / sizeof(language_type[0]); i++)
    {
        if((nvr_language == language_type[i].m_bIpcLanguageType) && (language_type[i].m_bVaild))
        {
            return 0;
        }
    }

	return eErrorUnsupportedLanguageType;
}

/**
 * @description: 获取定制文件中的语言类型
 * @return {*}
 * @param {eSpeechLanguage} *value :获取到的定制语言类型
 */
int PUB_GetLanguageCustomValue(eSpeechLanguage *value)
{
    *value = s_customValue;
	return 0;
}

/**
 * @description: 将语言类型设置到配置文件
 * @return {*}
 * @param {eSpeechLanguage} value: 需要设置的语言类型
 */
int PUB_SetLanguageType(eSpeechLanguage value)
{
	int ret = 0;
	CConfig *pCfg = CConfig::GetInstance();
	if(pCfg)
	{
        ret = pCfg->SetValue(EDVRCFG_SECTION_PUB, EDVRCFG_KEY_NVRSETLANGUAGE, (int)value);
	}

    if(0 == ret)
    {
        s_vioceLanguage = value;
        ZWSys_SetSpeechLanType(value);
    }

    return ret;
}

/**
 * @description: 获取设备当前的语言类型
 * @return {*}
 * @param {HY_DVR_VOICE_BROADCAST_LANGUAGE} *pcfg
 */
int PUB_GetVoiceLanguage(HY_DVR_VOICE_BROADCAST_LANGUAGE *pcfg)
{
    pcfg->bValid = 1;

    PUB_changetoHiLanguage_str(s_vioceLanguage,pcfg->strLanguage,sizeof(pcfg->strLanguage));

	return 0;
}

/**
 * @description: 设置语言类型
 * @return {*}
 * @param {HY_DVR_VOICE_BROADCAST_LANGUAGE} *pcfg
 */
int PUB_SetVoiceLanguage(HY_DVR_VOICE_BROADCAST_LANGUAGE *pcfg)
{
	int ret = 0;
    eSpeechLanguage value = eSpeechLanguage_Chinese;

	PUB_GetLanguageCustomValue(&value);
    if(eSpeechLanguage_Chinese != value)
    {
        //语言被定制了
        MPU_ERR("Language has been customed!");
        return eErrorLanguageTypeIsCustomized;
    }

    PUB_changetoIpcLanguage(pcfg->strLanguage, &value);

    //语言未被定制,检查IPC是否支持目标语言
	ret = PUB_CheckLanguageType(value);
	if(0 == ret)
	{
		//IPC支持目标语言，设置目标语言
        PUB_SetLanguageType(value);
	}

	return ret;
}

/**
 * @description: 根据IPC语言枚举，获取协议语言字符串
 * @return {*}
 * @param {eSpeechLanguage} ipc_language：IPC语言枚举
 * @param {char*} hi_language：协议语言字符串
 * @param {size_t} lenth: 字符串长度
 */
int PUB_changetoHiLanguage_str(eSpeechLanguage ipc_language, char *hi_language, size_t lenth)
{
	eSystemLanguage sys_language = eLanguage_Chinese;
	for(unsigned int i = 0; i < sizeof(language_type) / sizeof(language_type[0]); i++)
    {
        if(ipc_language == language_type[i].m_bIpcLanguageType)
        {
            sys_language = language_type[i].m_bLanguageType;
            break;
        }
    }


    if (sys_language >= 0 && sys_language < sizeof(SystemLanguage) / sizeof(SystemLanguage[0]))
    {
        const char *language = SystemLanguage[(int)sys_language];
        strncpy(hi_language, language, lenth-1);
        hi_language[lenth-1] = '\0';
        return 0;
    }

	return -1;
}

/**
 * @description: 根据IPC语言枚举，获取协议语言枚举
 * @return {*}
 * @param {eSpeechLanguage} ipc_language：IPC语言枚举
 * @param {eSystemLanguage} *hi_language：协议语言枚举
 */
int PUB_changetoHiLanguage(eSpeechLanguage ipc_language, eSystemLanguage *hi_language)
{
	*hi_language = eLanguage_Chinese;
	for(unsigned int i = 0; i < sizeof(language_type) / sizeof(language_type[0]); i++)
    {
        if(ipc_language == language_type[i].m_bIpcLanguageType)
        {
            *hi_language = language_type[i].m_bLanguageType;
            break;
        }
    }

	return 0;
}

/**
 * @description: 协议语言枚举转换为IPC语言枚举
 * @return {*}
 * @param {eSystemLanguage} hi_language：协议语言枚举
 * @param {eSpeechLanguage} *ipc_language：IPC语言枚举
 */
int PUB_changetoIpcLanguage(char* hi_language, eSpeechLanguage *ipc_language)
{
	*ipc_language = eSpeechLanguage_Chinese;
	for(unsigned int i = 0; i < sizeof(language_type) / sizeof(language_type[0]); i++)
    {
        if(0 == memcmp(hi_language, SystemLanguage[i], strlen(hi_language)))
        {
            *ipc_language = language_type[i].m_bIpcLanguageType;
            break;
        }
    }

	return 0;
}

/**
 * @description: 根据custominfo.ini中的语言类型，获取语言类型名，给云升级组装URL
 * @return {*} res :当前custominfo.ini中 定制语言类型对应的 SystemLanguage 中的语言字符串
 */
const char* PUB_SpeechLanguage_enum2str()
{
	for (size_t i = 0; i < (int)(sizeof(language_type)/sizeof(language_type[0])); i++)
	{
        if(i <= sizeof(SystemLanguage)/sizeof(SystemLanguage[0]))
		{
		    if(s_customValue == language_type[i].m_bIpcLanguageType)
            {
                return SystemLanguage[i];
            }
		}
	}

    return SystemLanguage[0];
}
