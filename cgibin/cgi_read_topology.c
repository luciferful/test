/*************************************************************************
	> File Name: cgibin/cgi_read_topology.c
	> Created Time: Sun Jul  4 22:20:49 2021
 ************************************************************************/

/* {
	"error":	0,
	"status":	"success",
	"date":	"1970-1-1 0:13:59",
	"results":	{
		"topology":	[{
			"srcdev":	{
				"ip":	"192.168.2.1",
				"byname":	"1#",
				"noise":	-105
			},
			"desdev":	{
				"ip":	"192.168.2.3",
				"byname":	"3#",
				"noise":	-99
			},
			"snr":	42
			}, {                                  
            "srcdev":       {              
                "ip":   "192.168.2.3",
                "byname":       "3#", 
                "noise":        -99   
            },                            
            "desdev":       {           
            "ip":   "192.168.2.1",
            "byname":       "1#", 
            "noise":        -105  
            },                          
             "snr":  45                     
        }]
	}
}
*/


#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <net/if.h>
#include <netinet/in.h>
#include <signal.h>
#include <stddef.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "cJSON_Utils.h"

struct STR_DevList
{
	char ip[128][18];
	int noise[128];
	int cnt;
};


static int dev_find_noise(struct STR_DevList *pdev, char *ip)
{
	int i;
	int num = pdev->cnt;
	for(i = 0; i < num; i++)
	{
		if (0 == memcmp(pdev->ip[i], ip, strlen(pdev->ip[i])))
			break;
	}

	if (i >= num)
		return -125;
	else
		return pdev->noise[i];
}

static int dev_print_info(struct STR_DevList *pdev)
{
	int i;
	int num = pdev->cnt;
	for(i = 0; i < num; i++)
		printf("%d: ip=%s, noise=%d\n", i, pdev->ip[i], pdev->noise[i]);
	return;
}
struct STR_DevList devlistinfo;

static int read_device_json(cJSON ** root)
{
	int ret = 0;	
	char *text = (char*)NULL;
	cJSON *out = *root;

	cJSON *Prim = NULL;
	cJSON *Neigh = NULL;
	cJSON *data = NULL;
	cJSON *tmp = NULL;
	cJSON *dstdev = NULL;
	cJSON *srcdev = NULL;

	int rel_flag = 0;

	cJSON *result = cJSON_CreateObject();
	cJSON *devlistArry = cJSON_CreateArray();
	cJSON *dev = NULL;

	cJSON_AddStringToObject(out, "status", "failed");
	cJSON_AddItemToObject(out, "results", result);
	cJSON_AddItemToObject(result, "devlist", devlistArry);

	memset(&devlistinfo, 0, sizeof(devlistinfo));

	printf("%s: Entry !\n", __func__);
	text = read_and_malloc_device_file("devinfo.dat", &ret);
	if (text == NULL)
	{
		printf("%s: read file error! ret = %d\n", __func__, ret);
		return -1;
	}
	else 
	{
		int count, i;
		cJSON *vis = NULL;
		cJSON *json = NULL;
		
		printf("%s: read file success!\n%08x\n", __func__, (int)text);
		json = cJSON_Parse(text);
		if (json == NULL)
		{
			printf("%s: parse text error!\n", __func__);
			free(text);
			return -2;
		}
		printf("%s: parse json success!\n", __func__);
		free(text);
		printf("%s:Success read file\n", __func__);
		vis = cJSON_GetObjectItem(json, "vis");
		if (vis == NULL)
			return -3;
		count = cJSON_GetArraySize(vis);

		/* loop for get all dev noise */
		for(i = 0; i < count; i++)
		{
			tmp = cJSON_GetArrayItem(vis, i);
			if (tmp != NULL)
			{
				Prim = cJSON_GetObjectItem(tmp, "Primary");
				if (Prim != NULL)
				{
					data = cJSON_GetObjectItem(Prim, "Noise");
					if (data != NULL)
						devlistinfo.noise[devlistinfo.cnt] = data->valueint;

					data = cJSON_GetObjectItem(Prim, "Ipv4");
					if (data != NULL)
					{
						int tmp_len = strlen(data->valuestring);
						memcpy(&devlistinfo.ip[devlistinfo.cnt][0], data->valuestring, tmp_len);
						devlistinfo.ip[devlistinfo.cnt][tmp_len] = '\0';
					}
					devlistinfo.cnt++;
				}
			}
		}
		printf("%s: get all dev success!\n", __func__);

		dev_print_info(&devlistinfo);

		/* loop for generate topology */
		for(i = 0; i < count; i++)
		{
			tmp = cJSON_GetArrayItem(vis, i);
			if (tmp != NULL)
			{
				int neigh_num, j;
				dstdev = cJSON_CreateObject();
				/* get the dest node */
				Prim = cJSON_GetObjectItem(tmp, "Primary");
				if (Prim != NULL)
				{
					data = cJSON_GetObjectItem(Prim, "Ipv4");
					if (data != NULL)
						cJSON_AddStringToObject(dstdev, "ip", data->valuestring);
					data = cJSON_GetObjectItem(Prim, "Name");
					if (data != NULL)
						cJSON_AddStringToObject(dstdev, "byname", data->valuestring);
					data = cJSON_GetObjectItem(Prim, "Noise");
					if (data != NULL)
						cJSON_AddNumberToObject(dstdev, "noise", data->valueint);

					Neigh = cJSON_GetObjectItem(tmp, "neighbors");
					if (Neigh != NULL)
					{
						neigh_num = cJSON_GetArraySize(Neigh);
						for(j = 0; j < neigh_num; j++)
						{
							cJSON *neigh_tmp;
							cJSON *dev;
							neigh_tmp = cJSON_GetArrayItem(Neigh, j);
							if (neigh_tmp != NULL)
							{
								data = cJSON_GetObjectItem(neigh_tmp, "ip");
								if (data != NULL)
								{
									srcdev = cJSON_CreateObject();
									cJSON_AddStringToObject(srcdev, "ip", data->valuestring);
									cJSON_AddNumberToObject(srcdev, "noise", dev_find_noise(&devlistinfo, data->valuestring));
								}
								data = cJSON_GetObjectItem(neigh_tmp, "byname");
								if (data != NULL)
									cJSON_AddStringToObject(srcdev, "byname", data->valuestring);
							}

							dev = cJSON_CreateObject();
							cJSON_AddItemToArray(devlistArry, dev);
							cJSON_AddItemToObject(dev, "srcdev", srcdev);
							cJSON_AddItemToObject(dev, "desdev", dstdev);
							rel_flag = 1;
							data = cJSON_GetObjectItem(neigh_tmp, "snr");
							if (data != NULL)
									cJSON_AddStringToObject(dev, "snr", data->valuestring);
						}
					}		
				}
			}

			/* if rel_flag equal 0, which means desdev is not related with root, so delete it manual */
			if (rel_flag == 0)
				cJSON_Delete(dstdev);
			else
				rel_flag = 0;
		}
		cJSON_Delete(json);
	}
	
}

static int cgi_parse_topology(cJSON ** root)
{
	int i, count, j, neigh_num, rel_flag, ret;
	char *text;
	cJSON *json = NULL;
	cJSON *vis = NULL;
	cJSON *tmp = NULL;
	cJSON *dstdev = NULL;
	cJSON *Prim = NULL;
	cJSON *srcdev = NULL;
	cJSON *data = NULL;
	cJSON *Neigh = NULL;
	cJSON *devlistArry = NULL;
	cJSON *result = NULL;
	cJSON *ipv4 = NULL;
	cJSON *byname = NULL;
	cJSON *noise = NULL;
	cJSON *neigh_tmp = NULL;
	cJSON *dev = NULL;

	text = read_and_malloc_device_file("devinfo.dat", &ret);
	if (text == NULL)
		return -1;

	json = cJSON_Parse(text);
	if (json == NULL)
	{
		free(text);
		return -2;
	}

	free(text);
	vis = cJSON_GetObjectItem(json, "vis");
	if (vis == NULL)
	{
		free(json);
		return -3;
	}

	result = cJSON_CreateObject();
	devlistArry = cJSON_CreateArray();
	cJSON_AddItemToObject(*root, "results", result);
	cJSON_AddItemToObject(result, "devlist", devlistArry);

	rel_flag = 0;

	count = cJSON_GetArraySize(vis);
	/* loop for get all dev noise */
	for(i = 0; i < count; i++)
	{
		tmp = cJSON_GetArrayItem(vis, i);
		if (tmp != NULL)
		{
			/* get the dest node */
			Prim = cJSON_GetObjectItem(tmp, "Primary");
			if (Prim != NULL)
			{
				ipv4 = cJSON_GetObjectItem(Prim, "Ipv4");
				byname = cJSON_GetObjectItem(Prim, "Name");
				noise = cJSON_GetObjectItem(Prim, "Noise");

				Neigh = cJSON_GetObjectItem(tmp, "neighbors");
				if (Neigh != NULL)
				{
					neigh_num = cJSON_GetArraySize(Neigh);
					for(j = 0; j < neigh_num; j++)
					{
						srcdev = cJSON_CreateObject();
						dstdev = cJSON_CreateObject();
						dev = cJSON_CreateObject();
						neigh_tmp = cJSON_GetArrayItem(Neigh, j);
						if (neigh_tmp != NULL)
						{
							data = cJSON_GetObjectItem(neigh_tmp, "ip");
							if (data != NULL)
							{
								
								cJSON_AddStringToObject(srcdev, "ip", data->valuestring);
								cJSON_AddNumberToObject(srcdev, "noise", dev_find_noise(&devlistinfo, data->valuestring));
							}
							data = cJSON_GetObjectItem(neigh_tmp, "byname");
							if (data != NULL)
								cJSON_AddStringToObject(srcdev, "byname", data->valuestring);

							if (ipv4 != NULL)
								cJSON_AddStringToObject(dstdev, "ip", ipv4->valuestring);
							if (byname != NULL)
								cJSON_AddStringToObject(dstdev, "byname", byname->valuestring);
							if (noise != NULL)
								cJSON_AddNumberToObject(dstdev, "noise", noise->valueint);

							cJSON_AddItemToArray(devlistArry, dev);
							cJSON_AddItemToObject(dev, "srcdev", srcdev);
							cJSON_AddItemToObject(dev, "desdev", dstdev);
							data = cJSON_GetObjectItem(neigh_tmp, "snr");
							if (data != NULL)
								cJSON_AddStringToObject(dev, "snr", data->valuestring);
						}
						else
						{
							cJSON_Delete(dev);
							cJSON_Delete(srcdev);
							cJSON_Delete(dstdev);
						}
					}
				}		
			}
		}
	}

	free(json);
	return 0;
}


static int cgi_parse_devlist()
{
	char *text;
	int count, i, tmp_len, ret;
	cJSON *vis = NULL;
	cJSON *json = NULL;
	cJSON *tmp = NULL;
	cJSON *Prim = NULL;
	cJSON *data = NULL;

	memset(&devlistinfo, 0, sizeof(devlistinfo));
	text = read_and_malloc_device_file("devinfo.dat", &ret);
	if (text == NULL)
		return -1;

	json = cJSON_Parse(text);
	if (json == NULL)
	{
		free(text);
		return -2;
	}

	vis = cJSON_GetObjectItem(json, "vis");
	if (vis == NULL)
	{
		free(json);
		free(text);
		return -3;
	}

	count = cJSON_GetArraySize(vis);
	/* loop for get all dev noise */
	for(i = 0; i < count; i++)
	{
		tmp = cJSON_GetArrayItem(vis, i);
		if (tmp != NULL)
		{
			Prim = cJSON_GetObjectItem(tmp, "Primary");
			if (Prim != NULL)
			{
				data = cJSON_GetObjectItem(Prim, "Noise");
				if (data != NULL)
					devlistinfo.noise[devlistinfo.cnt] = data->valueint;

				data = cJSON_GetObjectItem(Prim, "Ipv4");
				if (data != NULL)
				{
					tmp_len = strlen(data->valuestring);
					memcpy(&devlistinfo.ip[devlistinfo.cnt][0], data->valuestring, tmp_len);
					devlistinfo.ip[devlistinfo.cnt][tmp_len] = '\0';
				}
				devlistinfo.cnt++;
			}
		}
	}

	free(json);
	free(text);
	return 0;
}


int main()
{
	cJSON * root =  cJSON_CreateObject();
	char *printtext;

	printf("Access-Control-Allow-Origin:*i\n");
	printf("Content-Type:text/json;charset=utf-8\n\n");

	cJSON_AddStringToObject(root, "status", "success");
	
	//cgi_parse_devlist();
	//cgi_parse_topology(&root);
	
	//read_device_json(&root);
	
	printf("%s\n\n", printtext = cJSON_Print(root));
	free(printtext);


    cJSON_Delete(root);

	return 0;
}

