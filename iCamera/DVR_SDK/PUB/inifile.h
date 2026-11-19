/*
 * @file inifile.h
 * @brief initialization file read and write API
 *  -size of the ini file must less than 16K
 *  -after '=' in key value pair, can not support empty char. this would not like WIN32 API
 *  -support comment using ';' prefix
 *  -can not support multi line
 * @author FanCong
 * @date 2016-11-22
 * @version 0.1
 */

#ifndef INI_FILE_H_
#define INI_FILE_H_

#ifdef __cplusplus
extern "C"
{

#endif

/*
 *@brief read string in initialization file
 * retrieves a string from the specified section in an initialization file
 *@param section [in] name of the section containing the key name
 *@param key [in] name of the key pairs to value
 *@param value [in] pointer to the buffer that receives the retrieved string
 *@param size [in] size of result's buffer
 *@param default_value [in] default value of result
 *@param file [in] path of the initialization file
 *@return 1 : read success 
 *@       0 : read failure
 */
int read_profile_string(const char *section, const char *key, char *value, int size, const char *default_value, const char *file);

/*
 *@brief read int value in initialization file\n
 * retrieves int value from the specified section in an initialization file
 *@param section [in] name of the section containing the key name
 *@param key [in] name of the key pairs to value
 *@param default_value [in] default value of result
 *@param file [in] path of the initialization file
 *@return profile int value,if read fail, return default value
 */
int read_profile_int(const char *section, const char *key, int default_value, const char *file);

/*
 * @brief write a profile string to a ini file
 * @param section [in] name of the section,can't be NULL and empty string
 * @param key [in] name of the key pairs to value, can't be NULL and empty string
 * @param value [in] profile string value
 * @param file [in] path of ini file
 * @return 1 : success
 * @       0 : failure
 */
int write_profile_string(const char *section, const char *key, const char *value, const char *file);

/*
 * @brief write a profile string to a ini file
 * @param section [in] name of the section,can't be NULL and empty string
 * @param key [in] name of the key pairs to value, can't be NULL and empty string
 * @param value [in] profile int value
 * @param file [in] path of ini file
 * @return 1 : success
 * @	   0 : failure
 */
int write_profile_int(const char *section, const char *key, int value, const char *file);
#ifdef __cplusplus
}; //end of extern "C" {
#endif

#endif //end of INI_FILE_H_

