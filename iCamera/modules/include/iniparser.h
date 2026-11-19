
/*-------------------------------------------------------------------------*/
/**
   @file    iniparser.h
   @author  N. Devillard
   @brief   Parser for ini files.
*/
/*--------------------------------------------------------------------------*/

#ifndef _INIPARSER_H_
#define _INIPARSER_H_

/*---------------------------------------------------------------------------
                                Includes
 ---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
/* 必要的头文件#include "iniparser.h"
 * 使用的时候，只需要定义指针:
   dictionary  *   ini ;
   const char  *   s ;
   调用如下接口:
   1. ini = iniparser_load(ini_name); // 不需要额外打开文件
   2. s = iniparser_getstring(ini, "section:key", NULL);
   3. iniparser_freedict(ini);
   即可，其他情况同上。
 */

#include <string.h>

/*
 * The following #include is necessary on many Unixes but not Linux.
 * It is not needed for Windows platforms.
 * Uncomment it if needed.
 */
/* #include <unistd.h> */

#include "dictionary.h"

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*/
/**
  @brief    获取dictionary对象的section个数
  @param    d   Dictionary to examine
  @return   int Number of sections found in dictionary

  This function returns the number of sections found in a dictionary.
  The test to recognize sections is done on the string stored in the
  dictionary: a section name is given as "section" whereas a key is
  stored as "section:key", thus the test looks for entries that do not
  contain a colon.

  This clearly fails in the case a section name contains a colon, but
  this should simply be avoided.

  This function returns -1 in case of error.
 */
/*--------------------------------------------------------------------------*/

int iniparser_getnsec(const dictionary * d);


/*-------------------------------------------------------------------------*/
/**
  @brief    获取dictionary对象的第n个section的名字
  @param    d   Dictionary to examine
  @param    n   Section number (from 0 to nsec-1).
  @return   Pointer to char string

  This function locates the n-th section in a dictionary and returns
  its name as a pointer to a string statically allocated inside the
  dictionary. Do not free or modify the returned string!

  This function returns NULL in case of error.
 */
/*--------------------------------------------------------------------------*/

const char * iniparser_getsecname(const dictionary * d, int n);


/*-------------------------------------------------------------------------*/
/**
  @brief    保存dictionary对象一个section到file
  @param    d   Dictionary to dump
  @param    f   Opened file pointer to dump to
  @return   void

  This function dumps a given dictionary into a loadable ini file.
  It is Ok to specify @c stderr or @c stdout as output files.
 */
/*--------------------------------------------------------------------------*/

void iniparser_dump_ini(const dictionary * d, FILE * f);

/*-------------------------------------------------------------------------*/
/**
  @brief    保存一个字典的段到ini文件
  @param    d   Dictionary to dump
  @param    s   Section name of dictionary to dump
  @param    f   Opened file pointer to dump to
  @return   void

  This function dumps a given section of a given dictionary into a loadable ini
  file.  It is Ok to specify @c stderr or @c stdout as output files.
 */
/*--------------------------------------------------------------------------*/

void iniparser_dumpsection_ini(const dictionary * d, const char * s, FILE * f);

/*-------------------------------------------------------------------------*/
/**
  @brief    保存dictionary对象到file
  @param    d   Dictionary to dump.
  @param    f   Opened file pointer to dump to.
  @return   void

  这个函数打印字典条目到文件里面，一行一个元素，支持标准C stderr和C stdout输出，
  一般用在调试使用。
 */
/*--------------------------------------------------------------------------*/
void iniparser_dump(const dictionary * d, FILE * f);

/*-------------------------------------------------------------------------*/
/**
  @brief    获取dictionary对象某个section下的key个数
  @param    d   Dictionary to examine
  @param    s   Section name of dictionary to examine
  @return   段里面的键key的个数
 */
/*--------------------------------------------------------------------------*/
int iniparser_getsecnkeys(const dictionary * d, const char * s);

/*-------------------------------------------------------------------------*/
/**
  @brief    获取dictionary对象某个section下所有的key
  @param    d    Dictionary to examine
  @param    s    Section name of dictionary to examine
  @param    keys Already allocated array to store the keys in
  @return   The pointer passed as `keys` argument or NULL in case of error

  该接口是为了获取dictionary对象某个section下所有的key
  参数keys应该为一个指针数组，在调用的时候指定大小size

  每个返回的字符型的二级指针指向一个dictionary的字符串，不要去free或者修改它们
 */
/*--------------------------------------------------------------------------*/
const char ** iniparser_getseckeys(const dictionary * d, const char * s, const char ** keys);


/*-------------------------------------------------------------------------*/
/**
  @brief    返回dictionary对象的section:key对应的字串值
  @param    d       Dictionary to search
  @param    key     Key string to look for
  @param    def     Default value to return if key not found.
  @return   pointer to statically allocated character string

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the pointer passed as 'def' is returned.
  The returned char pointer is pointing to a string allocated in
  the dictionary, do not free or modify it.
 */
/*--------------------------------------------------------------------------*/
const char * iniparser_getstring(const dictionary * d, const char * key, const char * def);

/*-------------------------------------------------------------------------*/
/**
  @brief    返回idictionary对象的section:key对应的整形值
  @param    d Dictionary to search
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   integer

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.

  Supported values for integers include the usual C notation
  so decimal, octal (starting with 0) and hexadecimal (starting with 0x)
  are supported. Examples:

  - "42"      ->  42
  - "042"     ->  34 (octal -> decimal)
  - "0x42"    ->  66 (hexa  -> decimal)

  Warning: the conversion may overflow in various ways. Conversion is
  totally outsourced to strtol(), see the associated man page for overflow
  handling.

  Credits: Thanks to A. Becker for suggesting strtol()
 */
/*--------------------------------------------------------------------------*/
int iniparser_getint(const dictionary * d, const char * key, int notfound);

/*-------------------------------------------------------------------------*/
/**
  @brief    返回dictionary对象的section:key对应的双浮点值
  @param    d Dictionary to search
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   double

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.
 */
/*--------------------------------------------------------------------------*/
double iniparser_getdouble(const dictionary * d, const char * key, double notfound);

/*-------------------------------------------------------------------------*/
/**
  @brief    返回dictionary对象的section:key对应的布尔值
  @param    d Dictionary to search
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
int iniparser_getboolean(const dictionary * d, const char * key, int notfound);


/*-------------------------------------------------------------------------*/
/**
  @brief    设置dictionary对象的某个section:key的值
  @param    ini     Dictionary to modify.
  @param    entry   Entry to modify (entry name)
  @param    val     New value to associate to the entry.
  @return   int     0 if Ok, -1 otherwise.

  If the given entry can be found in the dictionary, it is modified to
  contain the provided value. If it cannot be found, the entry is created.
  It is Ok to set val to NULL.
 */
/*--------------------------------------------------------------------------*/
int iniparser_set(dictionary * ini, const char * entry, const char * val);


/*-------------------------------------------------------------------------*/
/**
  @brief    删除dictionary对象中某个section:key
  @param    ini     Dictionary to modify
  @param    entry   Entry to delete (entry name)
  @return   void

  If the given entry can be found, it is deleted from the dictionary.
 */
/*--------------------------------------------------------------------------*/
void iniparser_unset(dictionary * ini, const char * entry);

/*-------------------------------------------------------------------------*/
/**
  @brief    判断dictionary对象中是否存在某个section:key
  @param    ini     Dictionary to search
  @param    entry   Name of the entry to look for
  @return   integer 1 if entry exists, 0 otherwise

  Finds out if a given entry exists in the dictionary. Since sections
  are stored as keys with NULL associated values, this is the only way
  of querying for the presence of sections in a dictionary.
 */
/*--------------------------------------------------------------------------*/
int iniparser_find_entry(const dictionary * ini, const char * entry) ;

/*-------------------------------------------------------------------------*/
/**
  @brief    解析一个ini文件，返回一个已经分配的字典对象
  @param    ininame Name of the ini file to read.
  @return   Pointer to newly allocated dictionary

  This is the parser for ini files. This function is called, providing
  the name of the file to be read. It returns a dictionary object that
  should not be accessed directly, but through accessor functions
  instead.

  The returned dictionary must be freed using iniparser_freedict().
 */
/*--------------------------------------------------------------------------*/
dictionary * iniparser_load(const char * ininame);

/*-------------------------------------------------------------------------*/
/**
  @brief    释放一个字典里面所有相关联的内存
  @param    d Dictionary to free
  @return   void

  释放一个ini dictionary对象所有相关的内存
  在dictionary对象上下文切换的时候，必须要调用该接口
 */
/*--------------------------------------------------------------------------*/
void iniparser_freedict(dictionary * d);

#ifdef __cplusplus
}
#endif

#endif
