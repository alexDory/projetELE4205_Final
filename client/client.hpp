#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <map>
#include <sys/types.h>
#include <regex>
#include <sys/wait.h>
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>
#include <dirent.h>

using namespace cv;

#define PORT_NUMBER	4099
#define MUSIC_PORT	4100
#define SERVER_ADDRESS	"192.168.7.2"
#define ELE4205_OK	0b00000001
#define ELE4205_END	0b00000010
#define RES01	0b00000100
#define RES02	0b00001000
#define RES03	0b00010000
#define RES04	0b00100000
#define MODE01	0b01000000
#define MODE02	0b10000000
#define RES_INDEX 2
#define MODE_INDEX 1
#define READY	0b00000001
#define IDOWN 	0b00000010
#define PUSHB 	0b00000100
#define LEARN 	0b00001000
#define KNOWN 	0b00010000
#define UNKNOWN 0b00100000
#define ESC 27
#define ESC_WAIT_TIME	30
#define CAM_NAME "Caméra"
#define MENU_NAME "Sélectionner la résolution / Mode"
#define MODE_TEXT1 "APPRENTISSAGE"
#define MODE_TEXT2 "RECONNAISSANCE"
#define MENU_W 	900
#define MENU_H 	450
#define TEXT_H	32
#define TEXT_W	200
#define FIRST_BUTTON_Y	100
#define BUTTON_X	100
#define LEARN_X	600
#define LEARN_Y	100
#define RECON_Y	200
#define PADD 10

#define BLANK_FRAME_H		200
#define BLANK_FRAME_W		400
#define BLANK_FRAME_NAME	"Pas d'image."

#define CONF_THRESHOLD	10000

#define MODEL_PATH	"/model/trained_model.xml"



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

const int MODE_SELECT[] = {
	1,2
};

uint32_t getResArr(uint32_t index){
	uint32_t res_array = RES01;
	res_array = res_array << index;
	return res_array;
}

uint32_t getModeArr(uint32_t index){
	uint32_t mode_array = MODE01;
	mode_array = mode_array << index;
	return mode_array;
}

short SocketCreate(void)
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

int SocketConnect(int socket, int port=PORT_NUMBER);

void saveImage(int frame_number, Mat* frame, CascadeClassifier* face_cascade);

void LearnFace (Mat* frame, CascadeClassifier* face_cascade);

extern int frame_number;

void fenMenu(Mat &menuImage);

void ctrMenu(int event, int x, int y, int flags, void* userdata);

void setctrMenu(Mat &menuImage);

