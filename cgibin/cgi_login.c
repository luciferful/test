
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


int main()
{
	cJSON * root =  cJSON_CreateObject();
	char *printtext;
	int len;
	int  param_num;
	char buf[256];
	char name[16];
	char password[16];

	printf("Access-Control-Allow-Origin:*i\n");
	printf("Content-Type:text/json;charset=utf-8\n\n");
	len = strlen(getenv("QUERY_STRING"));
	
	if ((len > 255) || (len==-1))
	{
		cJSON_AddNumberToObject(root, "str_len", len);
        cJSON_AddStringToObject(root, "string", "NULL");
		cJSON_AddStringToObject(root, "status", "ERROR");
    	cJSON_AddStringToObject(root, "check", "ERROR");
	}
	else
	{
		sscanf(getenv("QUERY_STRING"), "%s", buf);
		// usrflag=2&as=dklsjdi&df=sd
		sscanf(buf, "%*[^=]=%d%*[^=]=%[^&]%*[^=]=%s", &param_num, name, password);
		if ((0 == memcmp(name, "king", 4)) && (0 == memcmp(password, "tt", 2)))
		{
			cJSON_AddStringToObject(root, "status", "success");
        	cJSON_AddStringToObject(root, "check", "success");
		}
		else
		{
			cJSON_AddStringToObject(root, "status", "fail");
        	cJSON_AddStringToObject(root, "check", "fail");
		}
		cJSON_AddStringToObject(root, "name", name);
		cJSON_AddStringToObject(root, "password", password);
		cJSON_AddNumberToObject(root, "str_len", len);
		cJSON_AddStringToObject(root, "string", buf);
	}
	printf("%s\n\n", printtext = cJSON_Print(root));
	free(printtext);
	cJSON_Delete(root);

}
