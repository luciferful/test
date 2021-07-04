/*************************************************************************
	> File Name: cig_common.c
	> Created Time: Sun Jul  4 09:53:01 2021
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


long int file_len_get(FILE *fp)
{
    long int flen;
    fseek(fp,0L,SEEK_SET);  /* 定位到文件开头 */
    fseek(fp,0L,SEEK_END);  /* 定位到文件末尾 */
    flen=ftell(fp);         /* 得到文件大小 */
    fseek(fp,0L,SEEK_SET);  /* 定位到文件开头 */
    return flen;
}

char * read_and_malloc_device_file(char * filename, int* ret)
{
	FILE *fp;
	long int len;
	char *buf;
	fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		printf("%s: no version is read!\n", __func__);
		*ret = 1;
		return NULL;
	}
	
	len = file_len_get(fp);
	buf = malloc(len);
	if (buf == NULL)
	{
		printf("malloc memory error!\n");
		fclose(fp);
		*ret = 2;
		return NULL;
	}
	
	fread(buf, 1, len, fp);
	fclose(fp);
	*ret = 0;
	return buf;
}


