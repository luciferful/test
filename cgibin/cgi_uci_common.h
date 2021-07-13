


#include "uci.h"
/**
 * @brief 获取配置信息
 * @param  section  配置段名称
 * @param  option   配置项
 * @param  pdata    获取的配置内容
 * @param  plen     获取的配置内容长度
 * @return int      成功返回UCI_OK, 失败返回其它值
 */
int config_get(char *filename, char *section, char *option, unsigned char *pdata, unsigned short *plen);


/**
 * @brief 设置配置信息
 * @param  section  配置段名称
 * @param  option   配置项
 * @param  pdata    获取的配置内容
 * @param  plen     获取的配置内容长度
 * @return int      成功返回UCI_OK, 失败返回其它值
 */
int config_set(char *filename, char *section, char *option, unsigned char *pdata, unsigned short *plen);

/**
 * @brief 获取配置项值
 * @param  o        配置项
 * @param  out      获取的配置内容
 * @return int      成功返回UCI_OK, 失败返回其它值
 */
static int uci_get_value(struct uci_option *o, char *out);

/**
 * @brief 获取uci配置项
 * @param  arg  获取该参数下的值
 *         eg: gateway.@interface[0]
 *             gateway.interface0.serverport
 * @param  out  获取的值存储区
 * @return int  成功返回UCI_OK, 失败返回其它值
 */
int uci_get_str(const char *arg, char *out);

/**
 * @brief 设置uci配置项 , 保存并且提交更改到文件
 * @param  arg  设置参数
 *         eg: gateway.@interface[0]=wifi-iface
 *             gateway.interface0.serverip=10.99.20.100
 *             gateway.interface0.serverport=8000
 * @return int  成功返回UCI_OK, 失败返回其它值
 */
int uci_set_str(const char *arg);
