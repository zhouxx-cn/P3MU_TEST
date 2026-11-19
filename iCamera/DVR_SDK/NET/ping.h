#ifndef __PING_H__
#define __PING_H__

#ifdef __cplusplus
extern "C" {
#endif

/*@ brief ping 函数实现Ping命令(支持绑定网卡)
 *@ param[in] ifname 针对的网卡名字
 *@ param[in] host_addr 主机名或者IP地址
 *@ return 1 ping成功; 0 ping失败 -1:错误
 */
int ping(const char *ifname, const char *host_addr);

/*@ brief ping 函数实现Ping命令(支持绑定网卡)
 *@ param[in] ifname 针对的网卡名字
 *@ param[in] host_addr 主机名或者IP地址
 *@ return 1 ping成功; 0 ping失败 -1:错误
 */
int myping(const char *ifname, const char *host_addr);


#ifdef __cplusplus
}
#endif

#endif // end of __PING_H__

