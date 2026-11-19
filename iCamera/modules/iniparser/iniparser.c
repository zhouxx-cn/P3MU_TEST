
/*-------------------------------------------------------------------------*/
/**
   @file    iniparser.c
   @author  N. Devillard
   @brief   Parser for ini files.
*/
/*--------------------------------------------------------------------------*/
/*---------------------------- Includes ------------------------------------*/
#include <ctype.h>
#include "iniparser.h"
#include "sys_banned.h"
#ifdef TEST_MEM_INFO
#define MODULE_NAME "iniparser"
#include "MemoryInterface.h"
//#include "TCreateObject.h"

//#include "InternalCommon.h"

#endif

/*---------------------------- 宏定义 -------------------------------------*/
#define ASCIILINESZ         (1024)      // 行的编码为ASCII，且每行最大为1024bytes
#define INI_INVALID_KEY     ((char*)-1)

/*---------------------------------------------------------------------------
                             私有模块
 ---------------------------------------------------------------------------*/
/*
 * 这些枚举表示每一行的解析状态(仅供内部使用)
 */
typedef enum _line_status_
{
    LINE_UNPROCESSED = 0, /* 未处理 */
    LINE_ERROR,           /* 错误 */
    LINE_EMPTY,           /* 空 */
    LINE_COMMENT,         /* 注释 */
    LINE_SECTION,         /* 段 */
    LINE_VALUE            /* 值 */
} line_status ;

/*-------------------------------------------------------------------------*/
/**
  @brief    转化字符串为小写
  @param    要转换的字符串
  @param    输出缓冲区
  @param    输出缓冲区的大小
  @return   成功返回指向输出字符串的指针，失败返回NULL

  最多len-1个输入字符串的元素被转换为小写字符
 */
/*--------------------------------------------------------------------------*/
static const char * strlwc(const char * in, char *out, unsigned len)
{
    if (in==NULL || out == NULL || len==0)
    {
        return NULL;
    }

    unsigned i = 0;

    while (in[i] != '\0' && i < len-1)
    {
        out[i] = (char)tolower((int)in[i]);
        i++ ;
    }
    out[i] = '\0';
    
    return out ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    字符串拷贝
  @param    将要拷贝的字符串
  @return   指向一个新分配的字符串，需要使用free()进行释放

  这个函数是strdup()的替代品，用在不支持strdup()函数的系统里面
 */
/*--------------------------------------------------------------------------*/
static char * xstrdup(const char * s)
{
    if (NULL == s)
    {
        return NULL ;
    }
    
    char *t = NULL;
    size_t len = 0;
    
    len = strlen(s) + 1 ;
#ifndef TEST_MEM_INFO
    t = (char*) malloc(len) ;
#else
	t = (char *)MemoryMalloc(len, "iniparser", __FILE__, __FUNCTION__, __LINE__);
#endif
    if (t)
    {
        memcpy(t, s, len) ;
    }
    return t ;
}

/*-------------------------------------------------------------------------*/
/*
  @brief    删掉字符串前后的空白字符
  @param    要解析和删除的字符串
  @return   unsigned类型的新字符串
 */
/*--------------------------------------------------------------------------*/
unsigned strstrip(char * s)
{
    if (NULL == s)
    {
        return 0;
    }

    char *last = NULL ;
    char *dest = s;

    last = s + strlen(s);
    while (isspace((int)*s) && *s)
    {
        s++;
    }
    
    while (last > s)
    {
        if (!isspace((int)*(last-1)))
            break ;
        last -- ;
    }
    *last = (char)0;

    memmove(dest,s,last - s + 1);
    
    return (last - s);
}

/*-------------------------------------------------------------------------*/
/*
  @brief    获取该字典对象里面段的个数
  @param    d   字典对象
  @return   段的个数(失败返回-1)

  This function returns the number of sections found in a dictionary.
  The test to recognize sections is done on the string stored in the
  dictionary: a section name is given as "section" whereas a key is
  stored as "section:key", thus the test looks for entries that do not
  contain a colon.

  This clearly fails in the case a section name contains a colon, but
  this should simply be avoided.

 */
/*--------------------------------------------------------------------------*/
int iniparser_getnsec(const dictionary * d)
{
    if (NULL == d)
    {
        return -1;
    }
    
    int i = 0;
    int nsec = 0;

    for (i=0 ; i<d->size ; i++)
    {
        if (NULL == d->key[i])
        {
            continue;
        }
        if (NULL == strchr(d->key[i], ':'))
        {
            nsec ++;
        }
    }
    return nsec;
}

/*-------------------------------------------------------------------------*/
/*
    @brief    获取该字典对象里面某一个段值
    @param    d   字典对象
    @param    n   段号 (from 0 to nsec-1).
    @return   成功返回段值的字符串(已经分配空间)，错误返回空

    这个函数可以导出第N个段的段名，返回第N个段名的字符串指针，该指针已经在
    字典对象里面静态分配了空间，所以，不要释放或者修改返回的字符串! 
 */
/*--------------------------------------------------------------------------*/
const char * iniparser_getsecname(const dictionary * d, int n)
{
    if (NULL==d || n<0)
    {
        return NULL;
    }

    int i = 0;
    int foundsec = 0;

    for (i=0 ; i<d->size ; i++) 
    {
        if (d->key[i]==NULL)
        {
            continue;
        }
        if (strchr(d->key[i], ':')==NULL)
        {
            foundsec++;
            if (foundsec>n)
            {
                break;
            }
        }
    }
    
    if (foundsec<=n)
    {
        return NULL ;
    }
    
    return (d->key[i]);
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Dump a dictionary to an opened file pointer.
  @param    d   字典对象
  @param    f   打开的文件指针
  @return   void

  This function prints out the contents of a dictionary, one element by
  line, onto the provided file pointer. It is OK to specify @c stderr
  or @c stdout as output files. This function is meant for debugging
  purposes mostly.
 */
/*--------------------------------------------------------------------------*/
void iniparser_dump(const dictionary * d, FILE * f)
{
    if (d==NULL || f==NULL)
    {
        return ;
    }
    
    int i = 0;
    for (i=0; i<d->size; i++)
    {
        if (d->key[i]==NULL)
            continue;
        if (d->val[i]!=NULL)
        {
            fprintf(f, "[%s]=[%s]\n", d->key[i], d->val[i]);
        }
        else
        {
            fprintf(f, "[%s]=UNDEF\n", d->key[i]);
        }
    }
    return ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    保存一个字典对象到ini文件里面
  @param    d   字典对象
  @param    f   打开的文件指针
  @return   void

  This function dumps a given dictionary into a loadable ini file.
  It is Ok to specify @c stderr or @c stdout as output files.
 */
/*--------------------------------------------------------------------------*/
void iniparser_dump_ini(const dictionary * d, FILE * f)
{

    if (d==NULL || f==NULL)
    {
        return ;
    }
    
    int i = 0;
    int nsec = 0;
    const char * secname ;

    nsec = iniparser_getnsec(d);
    if (nsec < 1)
    {
        /* 如果没有段值，则保存所有的键值 */
        for (i=0; i<d->size; i++)
        {
            if (d->key[i]==NULL)
                continue ;
            fprintf(f, "%s = %s\n", d->key[i], d->val[i]);
        }
        return;
    }
    for (i=0 ; i<nsec ; i++)
    {
        secname = iniparser_getsecname(d, i) ;
        iniparser_dumpsection_ini(d, secname, f);
    }
    fprintf(f, "\n");
    return ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    保存一个字典段到ini文件里面
  @param    d   字典对象
  @param    s   段名
  @param    f   打开的文件指针
  @return   void

  This function dumps a given section of a given dictionary into a loadable ini
  file.  It is Ok to specify @c stderr or @c stdout as output files.
 */
/*--------------------------------------------------------------------------*/
void iniparser_dumpsection_ini(const dictionary * d, const char * s, FILE * f)
{
    if (d==NULL || f==NULL)
    {
        return ;
    }
    
    if (! iniparser_find_entry(d, s))
    {
        return ;
    }
    
    int  j = 0;
    int  seclen = 0;
    char keym[ASCIILINESZ+1] = {0};
    
    seclen  = (int)strlen(s);
    fprintf(f, "\n[%s]\n", s);
    snprintf(keym, sizeof(keym), "%s:", s);
    for (j=0 ; j<d->size ; j++)
    {
        if (d->key[j]==NULL)
            continue ;
        if (!strncmp(d->key[j], keym, seclen+1))
        {
            fprintf(f, "%-30s = %s\n", d->key[j]+seclen+1, d->val[j] ? d->val[j] : "");
        }
    }
    fprintf(f, "\n");
    return ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    获取一个段里面键值的个数
  @param    d   字典对象
  @param    s   段名
  @return   该段里面键值的个数
 */
/*--------------------------------------------------------------------------*/
int iniparser_getsecnkeys(const dictionary * d, const char * s)
{
    if (NULL == d)
    {
        return 0;
    }

    int  seclen = 0;
    int  nkeys  = 0;
    int  j = 0;
    
    char keym[ASCIILINESZ+1] = {0};
    
    if (! iniparser_find_entry(d, s)) 
        return nkeys;

    seclen = (int)strlen(s);
    snprintf(keym, sizeof(keym), "%s:", s);

    for (j=0 ; j<d->size ; j++)
    {
        if (d->key[j]==NULL)
            continue ;
        if (!strncmp(d->key[j], keym, seclen+1))
            nkeys++;
    }

    return nkeys;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    获取字典的段里面的所有键值
  @param    d   字典对象
  @param    s   段名
  @param    keys 指向键值的指针(已经分配了数组去保存键值)
  @return   The pointer passed as `keys` argument or NULL in case of error

  This function queries a dictionary and finds all keys in a given section.
  The keys argument should be an array of pointers which size has been
  determined by calling `iniparser_getsecnkeys` function prior to this one.

  Each pointer in the returned char pointer-to-pointer is pointing to
  a string allocated in the dictionary; do not free or modify them.
 */
/*--------------------------------------------------------------------------*/
const char ** iniparser_getseckeys(const dictionary * d, const char * s, const char ** keys)
{
    int i, j, seclen ;
    char keym[ASCIILINESZ+1] = {0};

    if (d==NULL || keys==NULL) return NULL;
    if (! iniparser_find_entry(d, s)) return NULL;

    seclen  = (int)strlen(s);
    snprintf(keym, sizeof(keym), "%s:", s);

    i = 0;

    for (j=0 ; j<d->size ; j++)
    {
        if (d->key[j]==NULL)
            continue ;
        if (!strncmp(d->key[j], keym, seclen+1))
        {
            keys[i] = d->key[j];
            i++;
        }
    }

    return keys;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    获取字典对象的section:key对应的字符串
  @param    d   字典对象
  @param    key 需要寻找的字符串(组成格式section:key)
  @param    def 未找到的默认值
  @return   指向一个静态分配的字符串

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the pointer passed as 'def' is returned.
  The returned char pointer is pointing to a string allocated in
  the dictionary, do not free or modify it.
 */
/*--------------------------------------------------------------------------*/
const char * iniparser_getstring(const dictionary * d, const char * key, const char * def)
{
    if (d==NULL || key==NULL)
        return def;

    const char * lc_key = NULL;
    const char * sval  = NULL;
    char tmp_str[ASCIILINESZ+1] = {0};

    lc_key = strlwc(key, tmp_str, sizeof(tmp_str));
    sval = dictionary_get(d, lc_key, def);
    return sval ;
}

/*-------------------------------------------------------------------------*/
/*
  @brief    获取字典对象的section:key对应的字符串，转化为整型
  @param    d   字典对象
  @param    key 需要寻找的字符串(组成格式section:key)
  @param    notfound Value to return in case of error
  @return   integer

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.

  Supported values for integers include the usual C notation
  so decimal, octal (starting with 0) and hexadecimal (starting with 0x)
  are supported. Examples:

  "42"      ->  42
  "042"     ->  34 (octal -> decimal)
  "0x42"    ->  66 (hexa  -> decimal)

  Warning: the conversion may overflow in various ways. Conversion is
  totally outsourced to strtol(), see the associated man page for overflow
  handling.

  Credits: Thanks to A. Becker for suggesting strtol()
 */
/*--------------------------------------------------------------------------*/
int iniparser_getint(const dictionary * d, const char * key, int notfound)
{
    const char * str ;

    str = iniparser_getstring(d, key, INI_INVALID_KEY);
    if (str==INI_INVALID_KEY) 
    {
        return notfound ;
    }
    return ((int)strtol(str, NULL, 0));
}

/*-------------------------------------------------------------------------*/
/**
  @brief    获取字典对象的section:key对应的字符串，转化为double型
  @param    d   字典对象
  @param    key 需要寻找的字符串(组成格式section:key)
  @param    notfound Value to return in case of error
  @return   double

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.
 */
/*--------------------------------------------------------------------------*/
double iniparser_getdouble(const dictionary * d, const char * key, double notfound)
{
    const char * str = NULL;

    str = iniparser_getstring(d, key, INI_INVALID_KEY);
    if (str == INI_INVALID_KEY)
    {
        return notfound ;
    }
    return atof(str);
}

/*-------------------------------------------------------------------------*/
/**
  @brief    获取字典对象的section:key对应的字符串，转化为bool型
  @param    d   字典对象
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   integer

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.

  A true boolean is found if one of the following is matched:

  - A string starting with 'y'
  - A string starting with 'Y'
  - A string starting with 't'
  - A string starting with 'T'
  - A string starting with '1'

  A false boolean is found if one of the following is matched:

  - A string starting with 'n'
  - A string starting with 'N'
  - A string starting with 'f'
  - A string starting with 'F'
  - A string starting with '0'

  The notfound value returned if no boolean is identified, does not
  necessarily have to be 0 or 1.
 */
/*--------------------------------------------------------------------------*/
int iniparser_getboolean(const dictionary * d, const char * key, int notfound)
{
    int ret = 0;
    const char * c = NULL;

    c = iniparser_getstring(d, key, INI_INVALID_KEY);
    if (c==INI_INVALID_KEY)
    {
        return notfound ;
    }
    
    if (c[0]=='y' || c[0]=='Y' || c[0]=='1' || c[0]=='t' || c[0]=='T')
    {
        ret = 1 ;
    }
    else if (c[0]=='n' || c[0]=='N' || c[0]=='0' || c[0]=='f' || c[0]=='F')
    {
        ret = 0 ;
    }
    else
    {
        ret = notfound ;
    }
    return ret;
}

/*-------------------------------------------------------------------------*/
/*
    @brief    判断字典对象里面是否存在某个section:key
    @param    ini   字典对象
    @param    entry   Name of the entry to look for
    @return   存在返回1，不存在返回0

    Finds out if a given entry exists in the dictionary. Since sections
    are stored as keys with NULL associated values, this is the only way
    of querying for the presence of sections in a dictionary.
 */
/*--------------------------------------------------------------------------*/
int iniparser_find_entry(const dictionary * ini, const char * entry)
{
    int found = 0;
    if (iniparser_getstring(ini, entry, INI_INVALID_KEY)!=INI_INVALID_KEY)
    {
        found = 1 ;
    }
    return found;
}

/*-------------------------------------------------------------------------*/
/*
    @brief    修改一个字典的entry(格式为section:key)
    @param    ini   字典对象
    @param    entry   需要修改的entry(格式为section:key)
    @param    val     entry的新值(可以为空)
    @return   0表示成功，-1表示失败

    如果字典里面的entry存在，则修改该entry；如果不存在，则添加该entry，
    val的值可以为空，表示清零该值
*/
/*--------------------------------------------------------------------------*/
int iniparser_set(dictionary * ini, const char * entry, const char * val)
{
    char tmp_str[ASCIILINESZ+1] = {0};
    return dictionary_set(ini, strlwc(entry, tmp_str, sizeof(tmp_str)), val) ;
}

/*-------------------------------------------------------------------------*/
/*
    @brief    删除一个字典的entry(格式为section:key)
    @param    ini   字典对象
    @param    entry   需要删除的entry(格式为section:key)
    @return   void

    如果字典里面存在该entry，则该entry将会被删除
 */
/*--------------------------------------------------------------------------*/
void iniparser_unset(dictionary * ini, const char * entry)
{
    char tmp_str[ASCIILINESZ+1] = {0};
    dictionary_unset(ini, strlwc(entry, tmp_str, sizeof(tmp_str)));
}

/*-------------------------------------------------------------------------*/
/*
    @brief    从ini文件里面导入一行数据
    @param    input_line  要导入的行(可以包括多行输入)
    @param    section     存放输出段的空间
    @param    key         存放输出键的空间
    @param    value       存放输出键值的空间
    @return   写入行的状态值(枚举)
 */
/*--------------------------------------------------------------------------*/
static line_status iniparser_line(
    const char * input_line,
    char * section,
    char * key,
    char * value)
{
    line_status sta = LINE_UNPROCESSED;
    char * line = NULL;
    size_t len = 0;

    line = xstrdup(input_line);
    len = strstrip(line);

    if (len<1)
    {
        /* Empty line */
        sta = LINE_EMPTY ;
    }
    /* 修改源码用以去掉注释的支持(2017年6月8日 13:37:02  fc) */
#if 0
    else if (line[0]=='#' || line[0]==';')
    {
        /* Comment line */
        sta = LINE_COMMENT ;
    }
#endif
    else if (line[0]=='[' && line[len-1]==']')
    {
        /* Section name */
        sscanf(line, "[%[^]]", section);
        strstrip(section);
        strlwc(section, section, len);
        sta = LINE_SECTION ;
    }
    else if (sscanf (line, "%[^=] = \"%[^\"]\"", key, value) == 2 || sscanf (line, "%[^=] = '%[^\']'",   key, value) == 2)
    {
        /* Usual key=value with quotes, with or without comments */
        strstrip(key);
        strlwc(key, key, len);
        /* Don't strip spaces from values surrounded with quotes */
        /*
         * sscanf cannot handle '' or "" as empty values
         * this is done here
         */
        if (!strcmp(value, "\"\"") || (!strcmp(value, "''")))
        {
            value[0]=0 ;
        }
        sta = LINE_VALUE ;
    }    
    /* 修改源码用以去掉注释的支持(2017年6月8日 13:37:02  fc) */
    else if (sscanf (line, "%[^=] = %[^\n]", key, value) == 2)
    {
        /* Usual key=value without quotes, with or without comments */
        strstrip(key);
        strlwc(key, key, len);
        strstrip(value);
        
        sta = LINE_VALUE ;
    }    
    /* 修改源码用以去掉注释的支持(2017年6月8日 13:37:02  fc) */
    else if ( /*sscanf(line, "%[^=] = %[;#]", key, value)==2 || */ sscanf(line, "%[^=] %[=]", key, value) == 2)
    {
        /*
         * Special cases:
         * key=
         * key=;
         * key=#
         */
        strstrip(key);
        strlwc(key, key, len);
        value[0]=0 ;
        sta = LINE_VALUE ;
    }
    else
    {
        /* Generate syntax error */
        sta = LINE_ERROR ;
    }

#ifndef TEST_MEM_INFO
    free(line);
#else
	MemoryFree(line);
#endif
    return sta ;
}

/*-------------------------------------------------------------------------*/
/*
    @brief    解析一个ini文件，返回一个已经分配的字典对象
    @param    ini文件
    @return   返回的字典对象指针

    这是一个INI文件解析器，提供一个可读的ini文件名给函数，返回一个字典对象，
    该对象不能直接访问，但是可以通过一些接口进行访问

    返回的字典对象在使用完毕之后需要使用iniparser_freedict()函数进行释放
 */
/*--------------------------------------------------------------------------*/
dictionary * iniparser_load(const char * ininame)
{
    if (NULL == ininame)
    {
        printf("NULL pointer exception!\n");
        return NULL;
    }

    FILE *in = NULL;

    char line    [ASCIILINESZ+1] = {0};
    char section [ASCIILINESZ+1] = {0};
    char key     [ASCIILINESZ+1] = {0};
    char tmp     [(ASCIILINESZ * 2) + 2] = {0};
    char val     [ASCIILINESZ+1] = {0};

    int  last = 0;
    int  len = 0;
    int  lineno = 0;
    int  errs = 0;

    dictionary *dict = NULL;

    if (NULL == (in=fopen(ininame, "r")))
    {
        fprintf(stderr, "iniparser: cannot open %s\n", ininame);
        return NULL ;
    }

    dict = dictionary_new(0);
    if (NULL == dict)
    {
        fclose(in);
        return NULL ;
    }

    while (fgets(line+last, ASCIILINESZ-last, in) != NULL)
    {
        lineno ++;
        len = (int)strlen(line)-1;
        if (len == 0)
            continue;

        /* 缓冲区溢出安全检查 */
        if (line[len]!='\n' && !feof(in))
        {
            fprintf(stderr, "iniparser: input line too long in %s (%d)\n", ininame, lineno);
            dictionary_del(dict);
            fclose(in);
            return NULL ;
        }

        /* 去除行尾的\n和空格 */
        while ((len>=0) && ((line[len]=='\n') || (isspace(line[len]))))
        {
            line[len]=0 ;
            len-- ;
        }
        
        /* 该行全部都是\n或者空格 */
        if (len < 0)
        { 
            len = 0;
        }
        
        /* 探测多行 */
        if (line[len]=='\\')
        {
            /* 多行值 */
            last = len ;
            continue ;
        }
        else
        {
            last = 0;
        }
        
        switch(iniparser_line(line, section, key, val))
        {
            case LINE_EMPTY:
            case LINE_COMMENT:
            break ;

            case LINE_SECTION:
                errs = dictionary_set(dict, section, NULL);
            break ;

            case LINE_VALUE:
                snprintf(tmp, sizeof(tmp), "%s:%s", section, key);
                errs = dictionary_set(dict, tmp, val) ;
            break ;

            case LINE_ERROR:
                fprintf(stderr, "iniparser: syntax error in %s (%d):\n", ininame, lineno);
                fprintf(stderr, "-> %s\n", line);
                errs++ ;
            break;

            default:
            break ;
        }
        memset(line, 0, ASCIILINESZ);
        last = 0;
        if (errs<0)
        {
            fprintf(stderr, "iniparser: memory allocation failure\n");
            break ;
        }
    }
    if (errs)
    {
        dictionary_del(dict);
        dict = NULL ;
    }
    fclose(in);
    return dict ;
}

/*-------------------------------------------------------------------------*/
/*
    @brief    释放所有的字典的内存空间
    @param    d  字典对象
    @return   void

    在使用完该字典对象之后，必须要调用该接口，否则会出现内存泄漏
 */
/*--------------------------------------------------------------------------*/
void iniparser_freedict(dictionary * d)
{
    dictionary_del(d);
}
