#include<stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <map>
#include <sys/types.h>
#include <sys/wait.h>
#include <regex>

#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;

#define PORT_NUMBER		4099
#define MUSIC_PORT	4100
#define ELE4205_OK	0b00000001
#define ELE4205_QUIT	0b00000010
#define RES01	0b00000100
#define RES02	0b00001000
#define RES03	0b00010000
#define RES04	0b00100000
#define RES_INDEX 2
#define LEARN 0b00001000
#define KNOWN 0b00010000
#define UNKNOWN 0b00100000

#define READY 0b00000001
#define IDOWN 0b00000010
#define PUSHB 0b00000100

struct Resolution {
	int w;
	int h;
};

const Resolution CAMERA_RESOLUTIONS[] = {
	{160, 120},
	{320, 240},
	{800, 600},
	{1280, 720}
};


uint32_t getResIndex(uint32_t res_array){
	for (uint32_t i = 0; i < 4; i++){
		if ((res_array & (RES01 << i)) != 0){
			return i;
		}
	}
	return -1;
}

short SocketCreate(void)
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

int sockBind(int hSocket, int p);

int sendImage(VideoCapture capture, Mat frame, int sock, int flag);

int getLight();

int keyPressed(); 


