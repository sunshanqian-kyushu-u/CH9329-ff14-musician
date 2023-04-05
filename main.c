#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "cJSON.h"

#define MS_STRING_LENGTH_MAX 1000

/******************************************************************
 * license      : MIT
 * name         : main.c
 * author       : ssq
 * edition	    : v1.0
 * describe     : use CH9329 to simulate HID and play in ff14
 * how to use   : ./musicianApp xxx.ms
 *                the xxx.ms is self-defined json music score file
 * other        : 
 * log          : v1.0 2023/4/4 created
 ******************************************************************/

/*
 * @description     : main function
 * @param - argc 	: as above
 * @param - argv 	: as above
 * @return 			: 0 succeed; other failed
 */
int main(int argc, char *argv[])
{
    int ret;
	int ms_fd, driver_fd;
	int ms_string_length;
	char *ms_filename, *driver_filename;
	struct stat ms_stat;
	char ms_string[MS_STRING_LENGTH_MAX];
	char *ms_content;

	// check parameter
	if(argc != 2){
		printf("Error: Wrong command line!\r\n");
		goto error_command_line;
	}

	ms_filename = argv[1];
	// get music score file length
	ret = stat(ms_filename, &ms_stat);
	if(ret != 0){
		printf("Error: Can't find file %s!\r\n", ms_filename);
		goto error_find_ms;
	}
	// printf("the length of file is %d\r\n", ms_stat.st_size);

	//check ms
	if(ms_stat.st_size > MS_STRING_LENGTH_MAX){
		printf("Error: File %s is too big!\r\n", ms_filename);
		goto error_check_size;
	}

	// open ms
	ms_fd = open(ms_filename, O_RDONLY);
	if(ms_fd < 0){
		printf("Error: Can't open file %s\r\n", ms_filename);
		goto error_open_ms;
	}

	// read ms
	ret = read(ms_fd, ms_string, ms_stat.st_size);
	if(ret < 0){
		printf("Error: Can't read file %s\r\n", ms_filename);
		goto error_read_ms;
	}
	// printf("the json string is:\r\n%s\r\n", ms_string);

	// close ms
	ret = close(ms_fd);
	if(ret < 0){
		printf("Error: Can't close file %s\r\n", ms_filename);
		goto error_close_ms;
	}	

	// // parse json ms file
	cJSON* cjson = cJSON_Parse(ms_string);
	if(cjson == NULL){
		printf("Error: Can't parse ms string!\r\n");
		goto error_parse_ms_string;
	}
	ms_content = cJSON_GetObjectItem(cjson,"content")->valuestring;
	// printf("The content is %s\r\n", ms_content);
	
	return 0;

error_parse_ms_string:
error_close_ms:
error_read_ms:
	// did not close ms
error_open_ms:
error_check_size:
error_find_ms:
error_command_line:
	return -1;

	// int fd, retvalue;
	// char *filename;
	// char readbuf[100], writebuf[100];

	// if(argc != 3){
	// 	printf("Error Usage!\r\n");
	// 	return -1;
	// }

	// filename = argv[1];

	// /* 打开驱动文件 */
	// fd  = open(filename, O_RDWR);
	// if(fd < 0){
	// 	printf("Can't open file %s\r\n", filename);
	// 	return -1;
	// }

	// if(atoi(argv[2]) == 1){ /* 从驱动文件读取数据 */
	// 	retvalue = read(fd, readbuf, 50);
	// 	if(retvalue < 0){
	// 		printf("read file %s failed!\r\n", filename);
	// 	}else{
	// 		/*  读取成功，打印出读取成功的数据 */
	// 		printf("read data:%s\r\n",readbuf);
	// 	}
	// }

	// if(atoi(argv[2]) == 2){
 	// /* 向设备驱动写数据 */
	// 	memcpy(writebuf, usrdata, sizeof(usrdata));
	// 	retvalue = write(fd, writebuf, 50);
	// 	if(retvalue < 0){
	// 		printf("write file %s failed!\r\n", filename);
	// 	}
	// }

	// /* 关闭设备 */
	// retvalue = close(fd);
	// if(retvalue < 0){
	// 	printf("Can't close file %s\r\n", filename);
	// 	return -1;
	// }
}
