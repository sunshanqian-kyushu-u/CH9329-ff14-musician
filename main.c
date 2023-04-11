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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>

#include "cJSON.h"

#define MS_STRING_LENGTH_MAX 3000

#define TONE "tone"
#define TIE "tie"
#define BPM "bpm"
#define METER "meter"

#define DELAY_RELEASE 20000 // 20ms
#define DELAY_MAX 1000000	// 1s

#define SEND_KB_DATA_LENGTH 14

static const char SEND_KB_DATA_Q[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X01, 0X00, 0X14, 0X00, 0X00, 0X00, 0X00, 0X00, 0X21};
static const char SEND_KB_DATA_W[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X01, 0X00, 0X1A, 0X00, 0X00, 0X00, 0X00, 0X00, 0X27};
static const char SEND_KB_DATA_E[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X01, 0X00, 0X08, 0X00, 0X00, 0X00, 0X00, 0X00, 0X15};
static const char SEND_KB_DATA_R[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X01, 0X00, 0X15, 0X00, 0X00, 0X00, 0X00, 0X00, 0X22};
static const char SEND_KB_DATA_T[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X01, 0X00, 0X17, 0X00, 0X00, 0X00, 0X00, 0X00, 0X24};
static const char SEND_KB_DATA_Y[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X01, 0X00, 0X1C, 0X00, 0X00, 0X00, 0X00, 0X00, 0X29};
static const char SEND_KB_DATA_U[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X01, 0X00, 0X18, 0X00, 0X00, 0X00, 0X00, 0X00, 0X25};
static const char SEND_KB_DATA_1[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X01, 0X00, 0X1F, 0X00, 0X00, 0X00, 0X00, 0X00, 0X2C};
static const char SEND_KB_DATA_2[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X01, 0X00, 0X20, 0X00, 0X00, 0X00, 0X00, 0X00, 0X2D};
static const char SEND_KB_DATA_3[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X01, 0X00, 0X22, 0X00, 0X00, 0X00, 0X00, 0X00, 0X2F};
static const char SEND_KB_DATA_4[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X01, 0X00, 0X23, 0X00, 0X00, 0X00, 0X00, 0X00, 0X30};
static const char SEND_KB_DATA_5[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X01, 0X00, 0X24, 0X00, 0X00, 0X00, 0X00, 0X00, 0X31};

static const char SEND_KB_DATA_A[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X00, 0X00, 0X14, 0X00, 0X00, 0X00, 0X00, 0X00, 0X20};
static const char SEND_KB_DATA_S[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X00, 0X00, 0X1A, 0X00, 0X00, 0X00, 0X00, 0X00, 0X26};
static const char SEND_KB_DATA_D[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X00, 0X00, 0X08, 0X00, 0X00, 0X00, 0X00, 0X00, 0X14};
static const char SEND_KB_DATA_F[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X00, 0X00, 0X15, 0X00, 0X00, 0X00, 0X00, 0X00, 0X21};
static const char SEND_KB_DATA_G[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X00, 0X00, 0X17, 0X00, 0X00, 0X00, 0X00, 0X00, 0X23};
static const char SEND_KB_DATA_H[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X00, 0X00, 0X1C, 0X00, 0X00, 0X00, 0X00, 0X00, 0X28};
static const char SEND_KB_DATA_J[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X00, 0X00, 0X18, 0X00, 0X00, 0X00, 0X00, 0X00, 0X24};
static const char SEND_KB_DATA_6[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X00, 0X00, 0X1F, 0X00, 0X00, 0X00, 0X00, 0X00, 0X2B};
static const char SEND_KB_DATA_7[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X00, 0X00, 0X20, 0X00, 0X00, 0X00, 0X00, 0X00, 0X2C};
static const char SEND_KB_DATA_8[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X00, 0X00, 0X22, 0X00, 0X00, 0X00, 0X00, 0X00, 0X2E};
static const char SEND_KB_DATA_9[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X00, 0X00, 0X23, 0X00, 0X00, 0X00, 0X00, 0X00, 0X2F};
static const char SEND_KB_DATA_0[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X00, 0X00, 0X24, 0X00, 0X00, 0X00, 0X00, 0X00, 0X30};

static const char SEND_KB_DATA_Z[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X02, 0X00, 0X14, 0X00, 0X00, 0X00, 0X00, 0X00, 0X22};
static const char SEND_KB_DATA_X[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X02, 0X00, 0X1A, 0X00, 0X00, 0X00, 0X00, 0X00, 0X28};
static const char SEND_KB_DATA_C[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X02, 0X00, 0X08, 0X00, 0X00, 0X00, 0X00, 0X00, 0X16};
static const char SEND_KB_DATA_V[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X02, 0X00, 0X15, 0X00, 0X00, 0X00, 0X00, 0X00, 0X23};
static const char SEND_KB_DATA_B[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X02, 0X00, 0X17, 0X00, 0X00, 0X00, 0X00, 0X00, 0X25};
static const char SEND_KB_DATA_N[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X02, 0X00, 0X1C, 0X00, 0X00, 0X00, 0X00, 0X00, 0X2A};
static const char SEND_KB_DATA_M[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X02, 0X00, 0X18, 0X00, 0X00, 0X00, 0X00, 0X00, 0X26};
static const char SEND_KB_DATA_I[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X02, 0X00, 0X1F, 0X00, 0X00, 0X00, 0X00, 0X00, 0X2D};
static const char SEND_KB_DATA_O[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X02, 0X00, 0X20, 0X00, 0X00, 0X00, 0X00, 0X00, 0X2E};
static const char SEND_KB_DATA_P[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X02, 0X00, 0X22, 0X00, 0X00, 0X00, 0X00, 0X00, 0X30};
static const char SEND_KB_DATA_K[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X02, 0X00, 0X23, 0X00, 0X00, 0X00, 0X00, 0X00, 0X31};
static const char SEND_KB_DATA_L[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X02, 0X00, 0X24, 0X00, 0X00, 0X00, 0X00, 0X00, 0X32};
// expand
static const char SEND_KB_DATA_MM[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X02, 0X00, 0X0C, 0X00, 0X00, 0X00, 0X00, 0X00, 0X1A}; //shift+I
static const char SEND_KB_DATA_RELEASE[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X0C};
static const char SEND_KB_DATA_SHIFT[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X02, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X0E};
static const char SEND_KB_DATA_CTRL[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X01, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X0D};

/*
 * @description     			: report info while playing error
 * @param - release_flag		: release flag
 * @param - close_driver_flag	: close driver flag
 * @return 						: none
 */
void playing_error_info(int release_flag, int close_driver_flag){
	if(release_flag == 0 && close_driver_flag == 0){
		printf("[ERROR] Playing press error! Release key succeed! Close driver succeed!\r\n");
	}else if(release_flag == 1 && close_driver_flag == 0){
		printf("[ERROR] Playing press error! Release key failed! Close driver succeed!\r\n");
	}else if(release_flag == 0 && close_driver_flag == 1){
		printf("[ERROR] Playing press error! Release key succeed! Close driver failed!\r\n");
	}else if(release_flag == 1 && close_driver_flag == 1){
		printf("[ERROR] Playing press error! Release key failed! Close driver failed!\r\n");
	}
}

/*
 * @description     	: send cmd
 * @param - driver_fd	: driver fd
 * @param - ms_tone		: musical tone
 * @return 				: 0 for secceed -1 for failed
 * other				: q			...		u		1		...	5
 * 						: ctrl+q	...		ctrl+u	ctrl+2	...	ctrl+7
 * 						: a			...		j		6		...	0
 * 						: q			...		u		2		...	7
 * 						: z			...		m		i		opk	l
 * 						: shift+q	...		shift+u	shift+2	...	shift+7
 * 						: -			=
 * 						: shift+I	release
 */
int send_cmd(int driver_fd, char tone){
	switch (tone)
	{
	case 'q':
		if(write(driver_fd, SEND_KB_DATA_CTRL, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_Q, SEND_KB_DATA_LENGTH);
		}
	case 'w':
		if(write(driver_fd, SEND_KB_DATA_CTRL, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_W, SEND_KB_DATA_LENGTH);
		}
	case 'e':
		if(write(driver_fd, SEND_KB_DATA_CTRL, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_E, SEND_KB_DATA_LENGTH);
		}
	case 'r':
		if(write(driver_fd, SEND_KB_DATA_CTRL, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_R, SEND_KB_DATA_LENGTH);
		}
	case 't':
		if(write(driver_fd, SEND_KB_DATA_CTRL, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_T, SEND_KB_DATA_LENGTH);
		}
	case 'y':
		if(write(driver_fd, SEND_KB_DATA_CTRL, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_Y, SEND_KB_DATA_LENGTH);
		}
	case 'u':
		if(write(driver_fd, SEND_KB_DATA_CTRL, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_U, SEND_KB_DATA_LENGTH);
		}
	case '1':
		if(write(driver_fd, SEND_KB_DATA_CTRL, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_1, SEND_KB_DATA_LENGTH);
		}
	case '2':
		if(write(driver_fd, SEND_KB_DATA_CTRL, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_2, SEND_KB_DATA_LENGTH);
		}
	case '3':
		if(write(driver_fd, SEND_KB_DATA_CTRL, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_3, SEND_KB_DATA_LENGTH);
		}
	case '4':
		if(write(driver_fd, SEND_KB_DATA_CTRL, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_4, SEND_KB_DATA_LENGTH);
		}
	case '5':
		if(write(driver_fd, SEND_KB_DATA_CTRL, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_5, SEND_KB_DATA_LENGTH);
		}

	case 'a':
		return write(driver_fd, SEND_KB_DATA_A, SEND_KB_DATA_LENGTH);
	case 's':
		return write(driver_fd, SEND_KB_DATA_S, SEND_KB_DATA_LENGTH);
	case 'd':
		return write(driver_fd, SEND_KB_DATA_D, SEND_KB_DATA_LENGTH);
	case 'f':
		return write(driver_fd, SEND_KB_DATA_F, SEND_KB_DATA_LENGTH);
	case 'g':
		return write(driver_fd, SEND_KB_DATA_G, SEND_KB_DATA_LENGTH);
	case 'h':
		return write(driver_fd, SEND_KB_DATA_H, SEND_KB_DATA_LENGTH);
	case 'j':
		return write(driver_fd, SEND_KB_DATA_J, SEND_KB_DATA_LENGTH);
	case '6':
		return write(driver_fd, SEND_KB_DATA_6, SEND_KB_DATA_LENGTH);
	case '7':
		return write(driver_fd, SEND_KB_DATA_7, SEND_KB_DATA_LENGTH);
	case '8':
		return write(driver_fd, SEND_KB_DATA_8, SEND_KB_DATA_LENGTH);
	case '9':
		return write(driver_fd, SEND_KB_DATA_9, SEND_KB_DATA_LENGTH);
	case '0':
		return write(driver_fd, SEND_KB_DATA_0, SEND_KB_DATA_LENGTH);

	case 'z':
		if(write(driver_fd, SEND_KB_DATA_SHIFT, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_Z, SEND_KB_DATA_LENGTH);
		}
	case 'x':
		if(write(driver_fd, SEND_KB_DATA_SHIFT, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_X, SEND_KB_DATA_LENGTH);
		}
	case 'c':
		if(write(driver_fd, SEND_KB_DATA_SHIFT, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_C, SEND_KB_DATA_LENGTH);
		}
	case 'v':
		if(write(driver_fd, SEND_KB_DATA_SHIFT, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_V, SEND_KB_DATA_LENGTH);
		}
	case 'b':
		if(write(driver_fd, SEND_KB_DATA_SHIFT, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_B, SEND_KB_DATA_LENGTH);
		}
	case 'n':
		if(write(driver_fd, SEND_KB_DATA_SHIFT, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_N, SEND_KB_DATA_LENGTH);
		}
	case 'm':
		if(write(driver_fd, SEND_KB_DATA_SHIFT, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_M, SEND_KB_DATA_LENGTH);
		}
	case 'i':
		if(write(driver_fd, SEND_KB_DATA_SHIFT, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_I, SEND_KB_DATA_LENGTH);
		}
	case 'o':
		if(write(driver_fd, SEND_KB_DATA_SHIFT, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_O, SEND_KB_DATA_LENGTH);
		}
	case 'p':
		if(write(driver_fd, SEND_KB_DATA_SHIFT, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_P, SEND_KB_DATA_LENGTH);
		}
	case 'k':
		if(write(driver_fd, SEND_KB_DATA_SHIFT, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_K, SEND_KB_DATA_LENGTH);
		}
	case 'l':
		if(write(driver_fd, SEND_KB_DATA_SHIFT, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_L, SEND_KB_DATA_LENGTH);
		}

	// expand
	case '-':
		if(write(driver_fd, SEND_KB_DATA_SHIFT, SEND_KB_DATA_LENGTH) < 0){
			return -1;
		}else{
			return write(driver_fd, SEND_KB_DATA_MM, SEND_KB_DATA_LENGTH);
		}
	case '=':
		return write(driver_fd, SEND_KB_DATA_RELEASE, SEND_KB_DATA_LENGTH);

	default:
		return -1;
	}
}

/*
 * @description     	: calculate delay us
 * @param - ms_bpm		: beats per min
 * @param - ms_meter	: time
 * @return 				: delay us
 */
unsigned int cal_delay_us(int ms_bpm, char *ms_meter){
	switch (*ms_meter)
	{
		case '1':
			return (unsigned int)(60.0 / ms_bpm / 8 * 1000 * 1000);
		case '2':
			return (unsigned int)(60.0 / ms_bpm / 4 * 1000 * 1000);
		case '3':
			return (unsigned int)(60.0 / ms_bpm / 3 * 1000 * 1000);
		case '4':
			return (unsigned int)(60.0 / ms_bpm / 2 * 1000 * 1000);
		case '5':
			return (unsigned int)(60.0 / ms_bpm / 4 * 3 * 1000 * 1000);
		case '6':
			return (unsigned int)(60.0 / ms_bpm * 1000 * 1000);
		case '7':
			return (unsigned int)(60.0 / ms_bpm * 1.25 * 1000 * 1000);
		case '8':
			return (unsigned int)(60.0 / ms_bpm * 1.5 * 1000 * 1000);
		case '9':
			return (unsigned int)(60.0 / ms_bpm * 2 * 1000 * 1000);
		case '0':
			return (unsigned int)(60.0 / ms_bpm * 3 * 1000 * 1000);
		case '-':
			return (unsigned int)(60.0 / ms_bpm * 4 * 1000 * 1000);
		case '=':
			return (unsigned int)(60.0 / ms_bpm * 8 * 1000 * 1000);

		// expand
		case 'r':
			return (unsigned int)(60.0 / ms_bpm / 3 * 2 * 1000 * 1000);
		case '[':
			return (unsigned int)(60.0 / ms_bpm * 7 * 1000 * 1000);
		
		default:
			return (unsigned int)(0);
	}
}

/*
 * @description     : main function
 * @param - argc 	: as above
 * @param - argv 	: as above
 * @return 			: 0 succeed; other failed
 */
int main(int argc, char *argv[])
{
	int ms_fd, driver_fd;
	int ms_string_length;
	char *ms_filename, *driver_filename = "/dev/ttymxc2";

	struct stat ms_stat;
	struct stat driver_stat;
	struct termios new_cfg;

	char ms_string[MS_STRING_LENGTH_MAX];
	char ms_plaintext[MS_STRING_LENGTH_MAX];
	char *ms_meter;
	char *ms_tone;
	char *ms_tie;
	int ms_bpm;
	int ms_meter_length;
	int ms_tone_length;
	int ms_tie_length;

	char tone[MS_STRING_LENGTH_MAX];
	unsigned int delay_us[MS_STRING_LENGTH_MAX];

	int i, j;

	// check parameter
	if(argc != 2){
		printf("[Error] Wrong command line!\r\n");
		return -1;
	}

	ms_filename = argv[1];
	// get music score file length
	if(stat(ms_filename, &ms_stat) != 0){
		printf("[Error] Can't find file %s!\r\n", ms_filename);
		return -1;
	}
	// printf("[DEBUG] The length of %s is %d\r\n", ms_filename, ms_stat.st_size);
	// return 0;

	//check ms
	if(ms_stat.st_size > MS_STRING_LENGTH_MAX){
		printf("[Error] File %s is too big!\r\n", ms_filename);
		return -1;
	}

	// open ms
	ms_fd = open(ms_filename, O_RDONLY);
	if(ms_fd < 0){
		printf("[Error] Can't open file %s!\r\n", ms_filename);
		return -1;
	}

	// read ms
	if(read(ms_fd, ms_string, ms_stat.st_size) < 0){
		printf("[Error] Can't read file %s!\r\n", ms_filename);
		if(close(ms_fd) < 0){
			printf("[Error] Can't close file %s!\r\n", ms_filename);
		}
		return -1;
	}

	// close ms
	if(close(ms_fd) < 0){
		printf("[Error] Can't close file %s!\r\n", ms_filename);
		return -1;
	}

	// change ms_string to ms_plaintext
	j = 0;
	for(i = 0; i < ms_stat.st_size; i++){
		if(ms_string[i] != ' ' && ms_string[i] != '\t' && ms_string[i] != '\r'  && ms_string[i] != '\n'){
			ms_plaintext[j] = ms_string[i];
			j++;
		}
	}

	// parse json ms file
	cJSON* cjson = cJSON_Parse(ms_plaintext);
	if(cjson == NULL){
		printf("[Error] Can't parse ms string!\r\n");
		return -1;
	}

	ms_bpm = cJSON_GetObjectItem(cjson,BPM)->valueint;
	if(ms_bpm <= 0){
		printf("[Error] ms_bpm data wrong!\r\n");
		return -1;
	}
	ms_meter = cJSON_GetObjectItem(cjson,METER)->valuestring;
	ms_tone = cJSON_GetObjectItem(cjson,TONE)->valuestring;
	ms_tie = cJSON_GetObjectItem(cjson,TIE)->valuestring;
	
	ms_meter_length = strlen(ms_meter);
	ms_tone_length = strlen(ms_tone);
	ms_tie_length = strlen(ms_tie);

	// length check
	if(ms_tie_length != ms_tone_length || ms_meter_length != ms_tone_length){
		printf("[Error] Length unmatch!\r\n");
		return -1;
	}

	// calculate delay(us) based on tie length
	j = 0;
	for(i = 0; i < ms_tie_length; i++){
		if(*ms_tie == '0' && cal_delay_us(ms_bpm, ms_meter) > (unsigned int)(0)){
			delay_us[j] = cal_delay_us(ms_bpm, ms_meter);
			tone[j] = *ms_tone;
			// printf("delay_us[%d] = %d\r\n", j, delay_us[j]);
			// printf("tone[%d] = %c\r\n", j, tone[j]);
			ms_meter++;
			ms_tone++;
			ms_tie++;
			j++;
		}else if(*ms_tie == '1' && *(ms_tie + 1) != '1' && *(ms_tie + 1) != '\0' && 
				cal_delay_us(ms_bpm, ms_meter) > (unsigned int)(0) && 
				cal_delay_us(ms_bpm, ms_meter + 1) > (unsigned int)(0)){
			delay_us[j] = cal_delay_us(ms_bpm, ms_meter) + cal_delay_us(ms_bpm, ms_meter + 1);
			tone[j] = *ms_tone;
			// printf("delay_us[%d] = %d\r\n", j, delay_us[j]);
			// printf("tone[%d] = %c\r\n", j, tone[j]);
			ms_meter += 2;
			ms_tone += 2;
			ms_tie += 2;
			j++;
		}else if(*ms_tie == '\0'){
			break;
		}else{
			printf("[Error] Wrong in calculate delay!\r\n");
			return -1;
		}
	}
	tone[j] = '\0';
	// printf("%s\r\n", tone);
	// printf("%d\r\n", strlen(tone));
	// return 0;

	// minus DELAY_RELEASE
	// for(i = 0; i < strlen(tone); i++){
	// 	if(delay_us[i] >= (unsigned int)(2 * DELAY_RELEASE)){
	// 		delay_us[i] -= (unsigned int)(DELAY_RELEASE);
	// 	}else{
	// 		printf("[Error] No enough time for key release!\r\n");
	// 		return -1;
	// 	}
	// }

	//check driver
	if(stat(driver_filename, &driver_stat) != 0){
		printf("[Error] Can't find file %s!\r\n", driver_stat);
		return -1;
	}

	// open driver
	driver_fd = open(driver_filename, O_RDWR | O_NOCTTY);
	if(driver_fd < 0){
		printf("[Error] Can't open file %s!\r\n", driver_filename);
		return -1;
	}

	// clear cache
	if(tcflush(driver_fd, TCIOFLUSH) < 0){
		printf("[Error] Can't clear cache!\r\n");
		return -1;
	}

	// set serial port para
    if(cfsetospeed(&new_cfg, B9600) < 0){					// out boundrate
		printf("[Error] Can't set output boundrate!\r\n");
		return -1;
	}
   	if(cfsetispeed(&new_cfg, B9600) < 0){					// in boundrate
		printf("[Error] Can't set input boundrate!\r\n");
		return -1;
	}

   	new_cfg.c_cflag &= ~CSIZE;								// 8bit data
   	new_cfg.c_cflag |= CS8;

   	new_cfg.c_cflag &= ~PARENB;								// no parity
   	new_cfg.c_iflag &= ~INPCK;

   	new_cfg.c_cflag &= ~CSTOPB;								//1 stop bit
	
   	if(tcsetattr(driver_fd, TCSANOW, &new_cfg) < 0){		// refresh
		printf("[Error] Can't refresh para!\r\n");
		return -1;
	}

	// play
	for(i = 5; i > 0; i--){
		printf("[INFO] Count down: %d\r\n", i);
		sleep(1);
	}

	printf("[INFO] Playing...\r\n");

	// test place
	// char SEND_KB_DATA_SHIFT[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X02, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X0E};
	// char SEND_KB_DATA_CTRL[] = {0X57, 0XAB, 0X00, 0X02, 0X08, 0X01, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X0D};

	// test5
	// write(driver_fd, SEND_KB_DATA_Q, SEND_KB_DATA_LENGTH);
	// usleep(10000);
	// write(driver_fd, SEND_KB_DATA_Q, SEND_KB_DATA_LENGTH);
	// sleep(5);
	// write(driver_fd, SEND_KB_DATA_RELEASE, SEND_KB_DATA_LENGTH);
	// usleep(20000);

	// test4
	// write(driver_fd, SEND_KB_DATA_CTRL, SEND_KB_DATA_LENGTH);
	// usleep(10000);
	// write(driver_fd, SEND_KB_DATA_Q, SEND_KB_DATA_LENGTH);
	// sleep(5);
	// write(driver_fd, SEND_KB_DATA_RELEASE, SEND_KB_DATA_LENGTH);
	// usleep(20000);

	// test3
	// write(driver_fd, SEND_KB_DATA_SHIFT, SEND_KB_DATA_LENGTH);
	// usleep(10000);
	// write(driver_fd, SEND_KB_DATA_Z, SEND_KB_DATA_LENGTH);
	// sleep(5);
	// write(driver_fd, SEND_KB_DATA_RELEASE, SEND_KB_DATA_LENGTH);
	// usleep(20000);

	// test2
	// write(driver_fd, SEND_KB_DATA_SHIFT, SEND_KB_DATA_LENGTH);
	// sleep(5);
	// write(driver_fd, SEND_KB_DATA_A, SEND_KB_DATA_LENGTH);
	// sleep(5);
	// write(driver_fd, SEND_KB_DATA_RELEASE, SEND_KB_DATA_LENGTH);
	// close(driver_fd);
	// return 0;
	
	// test1
	// write(driver_fd, SEND_KB_DATA_SHIFT, SEND_KB_DATA_LENGTH);
	// sleep(10);
	// write(driver_fd, SEND_KB_DATA_RELEASE, SEND_KB_DATA_LENGTH);
	// close(driver_fd);
	// return 0;


	for(i = 0; i < strlen(tone); i++){
		if(send_cmd(driver_fd, tone[i]) < 0){										// can not press key
			int release_flag = 0, close_driver_flag = 0;
			if(write(driver_fd, SEND_KB_DATA_RELEASE, SEND_KB_DATA_LENGTH) < 0){	// can not release key
				release_flag = 1;
			}
			if(close(driver_fd) < 0){												// can not close driver	
				close_driver_flag = 1;
			}
			playing_error_info(release_flag, close_driver_flag);
			return -1;
		}else{
			if(delay_us[i] > (unsigned int)(DELAY_MAX)){
				sleep((unsigned int)(delay_us[i] / (unsigned int)(DELAY_MAX)));
				usleep((unsigned int)(delay_us[i] % (unsigned int)(DELAY_MAX)));
			}else{
				usleep(delay_us[i]);
			}
			if(write(driver_fd, SEND_KB_DATA_RELEASE, SEND_KB_DATA_LENGTH) < 0){
				printf("[ERROR] Can't release after press key!\r\n");
				if(close(driver_fd) < 0){
					printf("[ERROR] Can't close file %s!\r\n", driver_filename);
				}
				return -1;
			}
		}
	}

	// paly over, clear keyboard input
	if(write(driver_fd, SEND_KB_DATA_RELEASE, SEND_KB_DATA_LENGTH) < 0){
		if(close(driver_fd) < 0){
			printf("[ERROR] Play succeed! But can't clear keyboard and close file %s!\r\n", driver_filename);
			return -1;
		}
	}else{
		if(close(driver_fd) < 0){
			printf("[ERROR] Play succeed! Clear keyboard succeed! But close file %s failed!\r\n", driver_filename);
			return -1;
		}
	}
	
	printf("[INFO] Play succeed!\r\n");
	return 0;

}
