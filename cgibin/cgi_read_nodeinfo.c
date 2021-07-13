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


enum emuJsonType
{
	JSON_String = 0,
	JSON_Number,
	JSON_ERROR
};

typedef struct strJsonEle
{
	char item[16];
	char data[64];
	enum emuJsonType type;
};



void networkdata(cJSON *node, int get)
{
	if (1 == get)
	{
		cJSON_AddStringToObject(node, "ip", "192.168.2.8");
		cJSON_AddStringToObject(node, "netmask", "255.255.255.0");
		cJSON_AddStringToObject(node, "gateway", "192.168.2.8");
		cJSON_AddStringToObject(node, "mac", "B8:8E:DF:00:44:D8");
		cJSON_AddNumberToObject(node, "dhcpswitch", 1);
		cJSON_AddNumberToObject(node, "dhcpswitchreadonly", 0);
		cJSON_AddNumberToObject(node, "dhcplinkflag", 1);
		cJSON_AddStringToObject(node, "dhcpstart", "192.168.8.100");
		cJSON_AddStringToObject(node, "dhcpend", "192.168.8.200");
		cJSON_AddStringToObject(node, "dhcpgateway", "192.168.2.8");
		cJSON_AddStringToObject(node, "dhcpmask", "255.255.0.0");
		cJSON_AddStringToObject(node, "dhcpdns", "192.168.2.8");
		cJSON_AddNumberToObject(node, "wifistatus", 1);
		cJSON_AddStringToObject(node, "wifiapssid", "meshnode8");
		cJSON_AddStringToObject(node, "wifiappasswd", "87654321");
	}

	return;
}

int main()
{
	cJSON * root =  cJSON_CreateObject();
	cJSON * dev = cJSON_CreateObject();
	char *printtext;

	printf("Access-Control-Allow-Origin:*i\n");
	printf("Content-Type:text/json;charset=utf-8\n\n");

	cJSON_AddItemToObject(root, "results", dev);
	cJSON_AddStringToObject(root, "status", "success");
	cJSON_AddNumberToObject(dev, "devtype", 1);

	networkdata(dev, 1);
	printf("%s\n\n", printtext = cJSON_Print(root));
	    
	free(printtext);
	cJSON_Delete(dev);
    cJSON_Delete(root);

	return 0;
}
