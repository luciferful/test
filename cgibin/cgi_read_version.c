
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


static long int file_len_get(FILE *fp)
{
    long int flen;
    fseek(fp,0L,SEEK_SET);  /* 定位到文件开头 */
    fseek(fp,0L,SEEK_END);  /* 定位到文件末尾 */
    flen=ftell(fp);         /* 得到文件大小 */
    fseek(fp,0L,SEEK_SET);  /* 定位到文件开头 */
    return flen;
}

int Get_firmware_Name(int *name)
{
	FILE *fp = NULL;
	char *buf;
	long int len;
	long int i;
    int tmpbuf[4];
	char *start;

	system("version | grep -n Firmware > /tmp/version.txt");
	
	fp = fopen("/tmp/version.txt", "rb");
	if (fp == NULL)
	{
		printf("%s: no version is read!\n", __func__);
		return 1;
	}
	
	len = file_len_get(fp);
	buf = malloc(len);
	if (buf == NULL)
	{
		printf("malloc memory error!\n");
		fclose(fp);
		return 2;
	}
	
	fread(buf, 1, len, fp);
	
	for(i = 0; i < len; i++)
	{
		start = &buf[i];
		if (0 == memcmp(start, "Firmware Version: ", 18))
			break;
	}
	
	if (i >= len)
	{
		free(buf);
		return 3;
	}

	i += 19;
    sscanf(&buf[i], "%d.%d.%d", &name[0], &name[1], &name[2]);
	free(buf);
	return 0;
}

int Get_license_status()
{
	int ret = 0;
	long int len = 0;
	FILE *fp = NULL;
	char *buf;
	fp = fopen("/sys/devices/virtual/net/bat0/mesh/lisence_state", "r");
	if (fp == NULL)
    {
        printf("%s: no license status is read!\n", __func__);
        return 1;
    }
	
	len = file_len_get(fp);
    buf = malloc(len);
    if (buf == NULL)
    {
        printf("malloc memory error!\n");
        fclose(fp);
        return 2;
    }

    fread(buf, 1, len, fp);
    fclose(fp);
	
	if (0 == memcmp("enable", buf, 6))
		ret = 1;
	
	free(buf);
	return ret;
}

int Get_uid_string(char *uidbuf)
{
    FILE *fp = NULL;
    char idx_reorder[32] = {13,10,23,2,4,22,30,12,26,17,11,9,14,28,19,16,31,1,21,18,20,6,8,15,27,7,0,29,3,24,5,25};
    char idx_reorder2[32] = {16,24,26,17,11,13,10,19,22,27,7,0,29,5,25,9,14,28,31,1,23,2,12,4,30,21,18,20,6,8,15,3};
	char *buf;
	long int len;
	long int i;

    system("cat /sys/kernel/debug/batman_adv/bat0/uid > /tmp/uid.txt");
    fp = fopen("/tmp/uid.txt", "rb");
	if (fp == NULL)
	{
		printf("%s: no version is read!\n", __func__);
		return 1;
	}

    len = file_len_get(fp);
	buf = malloc(len);
	if (buf == NULL)
	{
		printf("malloc memory error!\n");
		fclose(fp);
		return 2;
	}
	
	fread(buf, 1, len, fp);
    fclose(fp);

    for(i = 0; i < 32; i++) {
        uidbuf[2*i+0] = buf[idx_reorder[i]];
        uidbuf[2*i+1] = buf[idx_reorder2[i]];
	}
	
	free(buf);
    
    return 0;
}

/* example
{
	"error":	0,
	"status":	"success",
	"date":	"2021-5-28 0:40:13",
	"results":	{
		"appname":	"Server",
		"versionnum":	"ht04 v2.3.7.97",
		"verisontype":	"alpha",
		"versiondata":	"Mar 30 2021 13:39:57",
		"versioncode":	2,
		"firmwarenum":	"2.3.8",
		"licenseflag":	0,
		"uid":	"4b75ddb5866bf1b47e2aadc25ec117c4",
		"mac":	"B8:8E:DF:00:A3:99",
		"devtype":	4
	}
}
*/

int main()
{
    cJSON * root =  cJSON_CreateObject();
    cJSON * item =  cJSON_CreateObject();
    char *printtext;

    int ver[3];
	int license_flag = 0;
    int ret;
    int name[4];
    char versionnum[32];
    char firmwarenum[32];
    char uidbuf[65];

    Get_uid_string(uidbuf);
    uidbuf[64] = '\0';
    ret = Get_firmware_Name(name);
	license_flag = Get_license_status();

    if (name[0] > 1)    name[0] = name[0] - 1;


    sprintf(versionnum, "%s:%d.%d", "Mesh", name[0], name[1]+name[2]);
    sprintf(firmwarenum, "%c%d.%d", 'v', name[0], name[1]+name[2]);

	printf("Access-Control-Allow-Origin:*i\n");
	printf("Content-Type:text/json;charset=utf-8\n\n");


    cJSON_AddNumberToObject(root, "error", 0);
    cJSON_AddStringToObject(root, "status", "success");
    cJSON_AddStringToObject(root, "date", "N/A");
    cJSON_AddItemToObject(root, "results", item);
    cJSON_AddStringToObject(item, "appname", "HnxWebServer");
    cJSON_AddStringToObject(item, "versionnum", versionnum);
    cJSON_AddStringToObject(item, "verisontype", "Release");
    cJSON_AddStringToObject(item, "versiondata", "N/A");
    cJSON_AddStringToObject(item, "versioncode", "N/A");
    cJSON_AddStringToObject(item, "firmwarenum", firmwarenum);
    cJSON_AddNumberToObject(item, "licenseflag", license_flag);
    cJSON_AddStringToObject(item, "uid", uidbuf);
    cJSON_AddStringToObject(item, "mac", "0");
    cJSON_AddStringToObject(item, "devtype", "N/A");


    printf("%s\n\n", printtext = cJSON_Print(root));
    
    free(printtext);
    //cJSON_Delete(item);
    cJSON_Delete(root);
    
	return 0;
}
