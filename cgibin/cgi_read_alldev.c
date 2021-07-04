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

int read_device_json(cJSON ** root)
{
	int ret;
	char *text = (char*)NULL;
	cJSON *out = *root;
	
	cJSON *result = cJSON_CreateObject();
	cJSON *devlistArry = cJSON_CreateArray();
	cJSON *dev;

	cJSON_AddItemToObject(out, "results", result);
	cJSON_AddItemToObject(result, "devlist", devlistArry);
	
	text = read_and_malloc_device_file("devinfo.dat", &ret);
	if (text == NULL)
		return ret;
	else {
		int count, i;
		cJSON *vis = NULL;
		cJSON *json = cJSON_Parse(text);
		if (json == NULL)
		{
			free(text);
			return -2;
		}

		/* read json structure success, release the buf */
		free(text);
		vis = cJSON_GetObjectItem(json, "vis");
		if (vis == NULL)
			return -3;

		/* vis number equal node number */
		count = cJSON_GetArraySize(vis);
		for(i = 0; i < count; i++)
		{
			cJSON *tmp;
			tmp = cJSON_GetArrayItem(vis, i);
			if (tmp != NULL)
			{
				cJSON *Prim = NULL;
				cJSON *data = NULL;
				Prim = cJSON_GetObjectItem(tmp, "Primary");
				if (Prim != NULL)
				{
					dev = cJSON_CreateObject();
					cJSON_AddItemToArray(devlistArry, dev);
					data = cJSON_GetObjectItem(Prim, "Name");
					if (data != NULL)
						cJSON_AddStringToObject(dev, "byname", data->valuestring);
					data = cJSON_GetObjectItem(Prim, "Ipv4");
					if (data != NULL)
						cJSON_AddStringToObject(dev, "ip", data->valuestring);
					data = cJSON_GetObjectItem(Prim, "Noise");
					if (data != NULL)
						cJSON_AddNumberToObject(dev, "noise", data->valueint);
					cJSON_AddNumberToObject(dev, "devtype", 4);
				}
				
			}
		}
		cJSON_Delete(json);
	}
	return 0;	
}


int main()
{
	cJSON * root =  cJSON_CreateObject();
	char *printtext;
	int ret;

	printf("Access-Control-Allow-Origin:*i\n");
	printf("Content-Type:text/json;charset=utf-8\n\n");

	ret = read_device_json(&root);
	if (ret == 0)
		cJSON_AddStringToObject(root, "status", "success");
	else
		cJSON_AddStringToObject(root, "status", "failed");
	printf("%s\n\n", printtext = cJSON_Print(root));
	free(printtext);

    cJSON_Delete(root);
	return 0;
}




