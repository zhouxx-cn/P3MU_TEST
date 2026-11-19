#include "CPubFunc.h"


extern "C" int mysystem(const char *cmdstring);

typedef struct taglink {
char *buf;
struct taglink *next;
} my_link, *LPmy_link;



/*@brief     StringExistInFile 查看文件中是否存在字符串
 *@param[in] string      欲查找的字符串
 *@param[in] stringSize  字符串大小(建议使用strlen(string)+1)
 *@param[in] fp          文件指针
 *@return    字符串存在返回1, 否则返回0
 */
int StringExistInFile(const char * string, int stringSize, FILE * fp) 
{
	int i=0, j=0, end=0;
	int bFound = 0;
	char *part = NULL;
    char *temp = NULL;
    
	part = (char *)malloc(stringSize*sizeof(char));	
	fseek(fp, 0L, SEEK_END);
	end = ftell(fp) - stringSize + 2;
    temp = part;

	while(i < end) 
	{
		j = 0;
		fseek(fp, (long)i++, SEEK_SET);
		fgets(part, stringSize, fp);
		while(*part) 
		{
			if(*string == *part) 
			{
				j++;
				string++;
				part++;    // 注意此处的指针已经自增
				continue;
			}
            bFound = 0;
			break;
		}
		if(j == stringSize - 1) 
		{
			bFound = 1;
			break;
		}else{
			string -= j;
			part -= j;
		}
	}
    /* 文件指针重新定位到头 */
    fseek(fp, 0, SEEK_SET);

    /* 不可以释放part，因为part已经跳到其他地址了 */
	free(temp);
	temp = NULL;

	return bFound;
}


/*@brief      GetIniSectionItem 获取ini文件键值内容
 *@param[in]  FileName ini文件路径 
 *@param[in]  Section  中括号引用的字段
 *@param[in]  Item     预查找的键
 *@param[out] Item     获取到的值
 *@return    成功返回0，错误返回-1
 *@note      当项目在文件中有多个时，返回第一个项目的值(包括段和键)
 * 			 支持缺省，如果没有找到该段或者键值，则直接返回NULL
 * 			 支持等号空格，返回字符串会原样输出(可以包括空格)
 * 			 允许不同段下面的键值不同
 *           不支持注释
 * 			 仅支持全字匹配，如下ini配置文件:
 * 			 ABD12345=123
 * 			 ABD1=TEST
 * 			 在查找的时候，如果查找的键值为ABD1，则得到的值为123，而不是TEST
 */
int GetIniSectionItem(const char* FileName, const char* Section, const char* Item, char* pValue, int valueSize) 
{ 
	char *value, Sect[30], c; 
	char linebuf[1024]; 
	FILE *inifp; 
	char fileBuff = 0;
    /* 以上三个指针都不可以为空 */
    assert(FileName);
    assert(Section);
    assert(Item);
        
	strncpy(Sect, "[",sizeof(Sect) - 1); 
	strncat(Sect, Section,sizeof(Sect) - 2); 
	strncat(Sect, "]", sizeof(Sect) - strlen(Sect) - 1); 

	if((inifp=fopen(FileName,"r")) == NULL) 
	{
        pValue = NULL;
        return -1;
	} 

    /* 检查文件里面是否存在段和键 */
    if (!StringExistInFile(Section, strlen(Section)+1, inifp) || !StringExistInFile(Item, strlen(Item)+1, inifp))
    {
        printf("Not Found %s or %s\n", Section, Item);
		fclose(inifp);
        pValue = NULL;
        return -1;
    }

	while(fread(&fileBuff, 1, 1, inifp) > 0) 
	{ 
		c = fileBuff;
		fileBuff = 0;
		if(c == '[')
		{ 
			ungetc(c, inifp); 
			fgets(linebuf, 1024, inifp); 
			if(strstr(linebuf, Sect)) 
			{ 
				// while((c=fgetc(inifp))!='[' && c!=EOF) 
				while(fread(&fileBuff, 1, 1, inifp) > 0) 
				{ 
					c = fileBuff;
					fileBuff = 0;
					if(c != '[')
					{
						ungetc(c, inifp); 
						fgets(linebuf, 1024, inifp); 
						if(strstr(linebuf, Item)) 
						{ 
							if((value=strchr(linebuf,'=')) != NULL) 
							{ 
								value++; 
								fclose(inifp);
								if(*value == '\n')
								{
									pValue = NULL;
									return -1;
								}
								strncpy(pValue, value, valueSize - 1);
								return 0;
							} 
						} 
					}
					else
					{
						break;
					}
				} 
				if(feof(inifp)) 
				{ 
					break; 
				} 
				ungetc(c, inifp); 
			} 
		} 
		else 
		{ 
			ungetc(c, inifp); 
			fgets(linebuf, 1024, inifp); 
		} 
	} 
	fclose(inifp); 

    return 0;
} 


/* 获取当前使用的网卡名称 */
int GetCurrNetCardType(char *ifname, int strSize)
{
    FILE *fp = NULL;    
    char tmp[16] = {0};

    if (NULL == ifname)
    {
        printf("NULL POINTER EXCEPTION!\n"); 
        return -1;
    }

    fp = fopen("/.effect_net_card", "r");
    if (NULL == fp)
    {
        perror("fopen");
        printf("Fail to open /.effect_net_card!\n");
        strncpy(ifname, "eth0", strSize - 1);
    }
    else
    {
        fgets(tmp, sizeof(tmp), fp);
        // 当前有效的的网卡(0:unknown 1:eth0 2:eth1 3:wlan 4:3G 5:4G 6:5G)
        if (3 == atoi(tmp))
        {
            strncpy(ifname, "wlan0", strSize - 1);
        }
        else
        {
            strncpy(ifname, "eth0",strSize - 1);
        }
        fclose(fp);
        fp = NULL;
    }
    return -1;
}

//功能:在文件长度为insertPoint的地方插入一段内容content，copyPoint后面的内容
//拷贝到content后面。insertPoint  <= copyPoint,一行小于1024个字节
int insertDateToFile(const char *FileName, long insertPoint, long copyPoint, const char * content)
{
	char buff[1024];
	LPmy_link head=NULL, tail=NULL, newTag=NULL;
	int len;
	FILE *fp=NULL;
	
	if(copyPoint < insertPoint)
		return -1;
	
	fp = fopen(FileName, "r");
	if (!fp)
		return 1;
	
	while (!feof(fp))
	{
		memset(buff, 0, sizeof (buff));
		if(insertPoint == ftell(fp))
		{
			fseek(fp, copyPoint, SEEK_SET);
			strncpy(buff, content, sizeof(buff) - 1);
		}
		else
			fgets(buff, sizeof (buff), fp);
		
		newTag = (LPmy_link)malloc(sizeof(my_link));
		if(NULL == newTag)
		{
			printf("malloc fail!\n");
			goto Free;
		}
		if(NULL == head)
		{
			head = tail = newTag;
		}
		else
			{
				tail->next = newTag;
				tail = newTag;
			}
		tail->next = NULL;
		len = strlen(buff);//根据一行长度开辟空间
		tail->buf = (char *)malloc(len + 1);
		if(NULL == tail->buf)
		{
			printf("malloc fail!\n");
			goto Free;                                                          
		}
		memset(tail->buf, 0, (len + 1));
		memcpy(tail->buf, buff, len);
	}

	if (fp)
		fclose(fp);
	
	fp = fopen(FileName, "w");
	if (!fp)
		goto Free;
	
	while(NULL != head)
	{
		newTag = head->next;
		fputs(head->buf, fp);
		if(NULL != head->buf)
			free(head->buf);
		free(head);
		head = newTag;
	}
	fflush(fp);
	fclose(fp);
	
	return 0;
Free:
	while(NULL != head)
	{
		newTag = head->next;
		if(NULL != head->buf)
			free(head->buf);
		free(head);
		head = newTag;
	}
	return -1;
}


/**************************************************************************
* 函数名称：int test_lite_big()
* 功能描述： 获取大小段模式。
* 输入参数： 无
* 输出参数： 无
* 返 回 值：     小端返回1，大端返回0
* 其它说明： 无
* 修改日期    版本号     修改人	     修改内容
* -----------------------------------------------
* 08/02/21	     V1.0	    cwf	      创建
**************************************************************************/
int test_lite_big()
{
	typedef union
	{
		unsigned char s1;
		int s2;
	} v;

	v lite;

	lite.s2 = 1;
	return lite.s1;
}

