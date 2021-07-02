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

/* 
{
	"error":	0,
	"status":	"success",
	"date":	"2018-5-29 22:42:49",
	"results":	{
		"devlist":	[{
				"ip":	"192.168.2.1",
				"byname":	"1#",
				"noise":	-105,
				"devtype":	4
			}, {
				"ip":	"192.168.2.3",
				"byname":	"3#",
				"noise":	-103,
				"devtype":	4
			}]
	}
}
*/

int main()
{
	cJSON * root =  cJSON_CreateObject();
	cJSON * ret = cJSON_CreateObject();
	cJSON * devArray = cJSON_CreateArray();
	cJSON * dev0 = cJSON_CreateObject();
	cJSON * dev1 = cJSON_CreateObject();

	char *printtext;

	printf("Access-Control-Allow-Origin:*i\n");
	printf("Content-Type:text/json;charset=utf-8\n\n");
	
	cJSON_AddStringToObject(root, "status", "success");
	cJSON_AddNumberToObject(root, "error", 0);

	cJSON_AddItemToObject(root, "results", ret);
	cJSON_AddItemToObject(ret, "devlist", devArray);
	
	cJSON_AddStringToObject(dev0, "ip", "192.168.2.10");
	cJSON_AddStringToObject(dev1, "ip", "192.168.2.12");
	cJSON_AddStringToObject(dev0, "byname", "10");
	cJSON_AddStringToObject(dev1, "byname", "12");
	cJSON_AddNumberToObject(dev0, "noise", -100);
	cJSON_AddNumberToObject(dev1, "noise", -103);
	cJSON_AddNumberToObject(dev0, "devtype", 4);
	cJSON_AddNumberToObject(dev1, "devtype", 4);
	
	cJSON_AddItemToArray(devArray, dev0);
	cJSON_AddItemToArray(devArray, dev1);
	
	printf("%s\n\n", printtext = cJSON_Print(root));
	    
	free(printtext);

	//cJSON_Delete(dev1);		cJSON_Delete(dev0);
//	cJSON_Delete(devArray);
//	cJSON_Delete(ret);
    cJSON_Delete(root);

	return 0;
}
