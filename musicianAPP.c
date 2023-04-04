#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <cJSON.h>

/***************************************************************
 * license      : MIT
 * name         : musicianApp.c
 * author       : ssq
 * edition	    : v1.0
 * describe     : use CH9329 to simulate HID and play in ff14
 * how to use   : ./musicianApp xxx.ms
 *                the xxx.ms is self-defined json music score file
 * other        : 
 * log          : v1.0 2023/4/4 created
 ***************************************************************/

/*
 * @description     : main function
 * @param - argc 	: as above
 * @param - argv 	: as above
 * @return 			: 0 succeed; other failed
 */
int main(int argc, char *argv[])
{
    int ret = 0;
    int fd = 0;








	int fd, retvalue;
	char *filename;
	char readbuf[100], writebuf[100];

	if(argc != 3){
		printf("Error Usage!\r\n");
		return -1;
	}

	filename = argv[1];

	/* 打开驱动文件 */
	fd  = open(filename, O_RDWR);
	if(fd < 0){
		printf("Can't open file %s\r\n", filename);
		return -1;
	}

	if(atoi(argv[2]) == 1){ /* 从驱动文件读取数据 */
		retvalue = read(fd, readbuf, 50);
		if(retvalue < 0){
			printf("read file %s failed!\r\n", filename);
		}else{
			/*  读取成功，打印出读取成功的数据 */
			printf("read data:%s\r\n",readbuf);
		}
	}

	if(atoi(argv[2]) == 2){
 	/* 向设备驱动写数据 */
		memcpy(writebuf, usrdata, sizeof(usrdata));
		retvalue = write(fd, writebuf, 50);
		if(retvalue < 0){
			printf("write file %s failed!\r\n", filename);
		}
	}

	/* 关闭设备 */
	retvalue = close(fd);
	if(retvalue < 0){
		printf("Can't close file %s\r\n", filename);
		return -1;
	}

	return 0;
}
