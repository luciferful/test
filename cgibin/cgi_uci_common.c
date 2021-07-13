

#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <net/if.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "cJSON.h"
#include "cJSON_Utils.h"
#include "cgi_common.h"
#include "cgi_uci_common.h"

/**
 * @brief 获取配置信息
 * @param  section	配置段名称
 * @param  option 	配置项
 * @param  pdata  	获取的配置内容
 * @param  plen  	获取的配置内容长度
 * @return int 		成功返回UCI_OK, 失败返回其它值
 */
int config_get(char *filename, char *section, char *option, unsigned char *pdata, unsigned short *plen)
{
	int ret = UCI_OK;
	struct uci_context *ctx = NULL;
	struct uci_section *s = NULL;
	struct uci_package * pkg = NULL;
	const char *value;
 
	ctx = uci_alloc_context(); 	// 申请一个UCI上下文.
	if (!ctx) {
		printf("%s: uci_alloc_context = NULL!\n", __func__);
		return UCI_ERR_MEM;
	}
 
	ret = uci_load(ctx, filename, &pkg);		// 加载并解析配置文件
	if(ret != UCI_OK)
    {
		printf("%s: uci_load = %d!\n", __func__, ret);
        uci_free_context(ctx);
        return ret;
    }
 
	printf("%s: pkg->e.name = %s, section = %s\n", __func__, pkg->e.name, section);
	s = uci_lookup_section(ctx, pkg, section);
	if(s != NULL)
	{
		printf("%s: uci_lookup_section = %d!\n", __func__, ret);
		if (NULL != (value = uci_lookup_option_string(ctx, s, option)))
		{//			pdata = (unsigned char *)strdup(value);
			strncpy(pdata, value, 100);
			*plen = strlen(pdata);
		}
		else
		{
			uci_unload(ctx, pkg);
			uci_free_context(ctx);
			ctx = NULL;
			return UCI_ERR_NOTFOUND;
		}
	}
	else
	{
		printf("%s: uci_lookup_section = NULL!\n", __func__);
		uci_unload(ctx, pkg);
		uci_free_context(ctx);
		ctx = NULL;
		return UCI_ERR_NOTFOUND;
	}
 
	uci_unload(ctx, pkg);
	uci_free_context(ctx);
	ctx = NULL;
	return ret;
}
/**
 * @brief 设置配置信息
 * @param  section 	配置段名称
 * @param  option  	配置项
 * @param  pdata  	获取的配置内容
 * @param  plen    	获取的配置内容长度
 * @return int		成功返回UCI_OK, 失败返回其它值
 */
int config_set(char *filename, char *section, char *option, unsigned char *pdata, unsigned short *plen){
	struct uci_context *ctx = NULL;
	struct uci_package * pkg = NULL;
	struct uci_element *e;
	int ret = UCI_OK;
 
	ctx = uci_alloc_context();
	if (!ctx) {
		return UCI_ERR_MEM;
	}
 
	struct uci_ptr ptr ={
				.package = filename,
				.section = section,
				.option = option,
				.value = pdata,
			};
 
	ret = uci_set(ctx, &ptr);	//写入配置
    if(ret != UCI_OK)
    {
        uci_free_context(ctx);
        return ret;
    }
 
	ret = uci_save(ctx, ptr.p);	//保存更改
    if(ret != UCI_OK)
    {
        uci_free_context(ctx);
        return ret;
    }
 
    ret = uci_commit(ctx, &ptr.p, false);	//提交更改
    if(ret != UCI_OK)
    {
        uci_free_context(ctx);
        return ret;
    }
 
	// system("/etc/init.d/network restart");	//配置应用示例
 
	uci_free_context(ctx);
    return ret;
}
/**
 * @brief 获取配置项值
 * @param  o      	配置项
 * @param  out   	获取的配置内容
 * @return int		成功返回UCI_OK, 失败返回其它值
 */
static int uci_get_value(struct uci_option *o, char *out){
	struct uci_element *e;
    const char *delimiter = " ";	//值为列表时的分隔符
	bool sep = false;
 
	switch(o->type) {
	case UCI_TYPE_STRING:
        strcpy(out, o->v.string);
		break;
	case UCI_TYPE_LIST:
		uci_foreach_element(&o->v.list, e) {
            if(sep)
                strcat(out, delimiter);
            strcat(out, e->name);
			sep = true;
		}
		break;
	default:
		return UCI_ERR_INVAL;
		break;
	}
 
    return UCI_OK;
}
/**
 * @brief 获取uci配置项
 * @param  arg	获取该参数下的值
 *         eg: gateway.@interface[0]
 *             gateway.interface0.serverport
 * @param  out	获取的值存储区
 * @return int 	成功返回UCI_OK, 失败返回其它值
 */
int uci_get_str(const char *arg, char *out){
    struct uci_context *ctx;
    struct uci_element *e;
	struct uci_ptr ptr;
	int ret = UCI_OK;
    char *name = NULL;
 
    if(arg == NULL || out == NULL) return UCI_ERR_INVAL;
    name = strdup(arg);
    if(name == NULL) return UCI_ERR_INVAL;
 
    ctx = uci_alloc_context();
	if (!ctx) {
        free(name);
		return UCI_ERR_MEM;
	}
 
    if (uci_lookup_ptr(ctx, &ptr, name, true) != UCI_OK) {
        uci_free_context(ctx);
        free(name);
		return UCI_ERR_NOTFOUND;
	}
 
    if(UCI_LOOKUP_COMPLETE & ptr.flags)
    {
        e = ptr.last;
        switch(e->type)
        {
            case UCI_TYPE_SECTION:
                ret = UCI_ERR_INVAL;
            break;
            case UCI_TYPE_OPTION:
                ret = uci_get_value(ptr.o, out);
            break;
            default:
                ret = UCI_ERR_NOTFOUND;
            break;
        }
    }
    else
        ret = UCI_ERR_NOTFOUND;
 
    uci_free_context(ctx);
    free(name);
    return ret;
}
/**
 * @brief 设置uci配置项 , 保存并且提交更改到文件
 * @param  arg	设置参数
 *         eg: gateway.@interface[0]=wifi-iface
 *             gateway.interface0.serverip=10.99.20.100
 *             gateway.interface0.serverport=8000
 * @return int 	成功返回UCI_OK, 失败返回其它值
 */
int uci_set_str(const char *arg){
    struct uci_context *ctx;
    struct uci_element *e;
	struct uci_ptr ptr;
	int ret = UCI_OK;
    char *name = NULL;
 
    if(arg == NULL) return UCI_ERR_INVAL;
    name = strdup(arg);
    if(name == NULL) return UCI_ERR_MEM;
 
    ctx = uci_alloc_context();
	if (!ctx) {
        free(name);
		return UCI_ERR_MEM;
	}
 
    if (uci_lookup_ptr(ctx, &ptr, name, true) != UCI_OK) {
        uci_free_context(ctx);
        free(name);
		return UCI_ERR_NOTFOUND;
	}
 
    ret = uci_set(ctx, &ptr);
    if(ret != UCI_OK)
    {
        uci_free_context(ctx);
        free(name);
        return ret;
    }
 
    ret = uci_save(ctx, ptr.p);
    if(ret != UCI_OK)
    {
        uci_free_context(ctx);
        free(name);
        return ret;
    }
 
    ret = uci_commit(ctx, &ptr.p, false);
    if(ret != UCI_OK)
    {
        uci_free_context(ctx);
        free(name);
        return ret;
    }
 
    uci_free_context(ctx);
    free(name);
    return ret;
}



