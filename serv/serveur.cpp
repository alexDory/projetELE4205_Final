#include "serveur.hpp"
using namespace std;
using namespace cv;

char resIndex = RES_INDEX;


/**
 * @brief Read the state of the key (GPIO 228).
 * @return 1 if the key is pressed, -1 on error, or 0 otherwise.
 */
int keyPressed(){
	char buffer[] = "0";
	FILE * file = fopen("/sys/class/gpio/gpio228/value", "r");
	if (file != NULL) {
		fread(buffer, 1, 1, file);
		string a(buffer);
		fclose(file);
		return stoi(a) ; 
	}else {
		cout << "Error while sending.." << endl;
		return -1;

	}
}

/**
 * @brief Read the light level from SARADC (Successive Approximation Register ADC).
 * @return Light level, -1 on error.
 */
int getLight(){

	char buffer[4] = {0};
	FILE * file = fopen("/sys/class/saradc/ch0", "r");
	if (file != NULL) {
		fread(buffer, 1, 4, file);
		string a(buffer);
		fclose(file);
		return stoi(a) ; 
	}else {
		cout << "Error while sending.." << endl;
		return -1;

	}

}

/**
 * @brief Send the header of a Mat frame over a socket.
 * @param frame The Mat frame to send.
 * @param socket The socket for communication.
 * @return 0 on success, -1 on error.
 */
int sendEntete(Mat frame,int socket){	
	int bytes ;
	Mat* enteteframe = new Mat(frame) ; 
	char* entete = reinterpret_cast<char *>(enteteframe);
	if(( bytes = send(socket,entete,sizeof(Mat),0)) < 0){
		cout << "Error while sending.." << endl;
		return -1;
	}
	delete enteteframe ;
	return 0;    
}
 
/**
 * @brief Send an image over a socket.
 * @param capture VideoCapture object for capturing frames.
 * @param frame The Mat frame to send.
 * @param socket The socket for communication.
 * @param flag Flag indicating whether to send the header.
 * @return 0 on success, -1 on error.
 */
int sendFrame(VideoCapture capture,Mat frame, int socket, int flag){
	capture >> frame;
	int imageSize = frame.total()*frame.elemSize();
	if (flag == 0){
		sendEntete(frame, socket);
	}
	if(!frame.empty()){
		if(write(socket,frame.data,imageSize) < 0){
			return -1;
		}
	}
	return 0;
}

/**
 * @brief Bind a socket to a specific port.
 * @param hSocket The socket to bind.
 * @param p Port number.
 * @return 0 on success, -1 on error.
 */
int sockBind(int hSocket, int p)
{
	int iRetval=-1;
	int ClientPort = p;
	struct sockaddr_in  remote;
	memset(&remote, 0, sizeof(remote)); 
	remote.sin_family = AF_INET;
	remote.sin_addr.s_addr = htonl(INADDR_ANY);
	remote.sin_port = htons(ClientPort); 
	iRetval = bind(hSocket,(struct sockaddr *)&remote,sizeof(remote));
	return iRetval;
}

/**
 * @brief Play music based on client messages. Only gets triggered when the button is pressed.
 */
void playMusic(){

	string frequencyPath = "../../sys/devices/pwm-ctrl.42/freq0";
    string enablePath = "../../sys/devices/pwm-ctrl.42/enable0";
	uint32_t message;
	char* message_serveur = (char*)&message;
	uint32_t client_message = 0;
	struct sockaddr_in server, client;
	int socket_desc, socket, clientLen ;
	socket_desc = SocketCreate();
	sockBind(socket_desc, MUSIC_PORT);
	listen(socket_desc, 5);
	clientLen = sizeof(struct sockaddr_in);
	socket = accept(socket_desc,(struct sockaddr *)&client, (socklen_t*)&clientLen);
	
	
	recv(socket, &client_message, sizeof(uint32_t), 0);
	if(client_message == KNOWN){
		FILE* file = fopen("../../sys/devices/pwm-ctrl.42/enable0", "w");
		if (file != NULL) {
			fprintf(file, "%d\n", 1);
			fclose(file);
			}else{

				cout<<"Error opening file" <<endl;
			}

		file = fopen(frequencyPath.c_str(), "w");
		if (file != NULL) {
			fprintf(file, "%u\n", 440);
			fclose(file);
			}else{

				cout<<"Error opening file" <<endl;
			}
		this_thread::sleep_for(chrono::milliseconds(3000));

		cout << "Note Off! " << to_string(440) << endl;

		file = fopen(enablePath.c_str(), "w");
		if (file != NULL) {
			fprintf(file, "%d\n", 0);
			fclose(file);
			}else{

				cout<<"Error opening file" <<endl;
			}
	}else if(client_message == UNKNOWN){
		for(int i = 0; i<= 3; i++){
			FILE* file = fopen("../../sys/devices/pwm-ctrl.42/enable0", "w");
			if (file != NULL) {
				fprintf(file, "%d\n", 1);
				fclose(file);
				}else{

					cout<<"Error opening file" <<endl;
				}


			file = fopen(frequencyPath.c_str(), "w");
			if (file != NULL) {
				fprintf(file, "%u\n", 660);
				fclose(file);
				}else{

					cout<<"Error opening file" <<endl;
				}
			this_thread::sleep_for(chrono::milliseconds(500));

			cout << "Note Off! " << to_string(660) << endl;

			file = fopen(enablePath.c_str(), "w");
			if (file != NULL) {
				fprintf(file, "%d\n", 0);
				fclose(file);
				}else{

					cout<<"Error opening file" <<endl;
				}
		}
	}

	close(socket);
}

/**
 * @brief Main function for the server program.
 * @param argc Number of command-line arguments.
 * @param argv Command-line arguments.
 * @return 0 on success, -1 on error.
 */
int main(int argc, char *argv[])
{
	bool canSendImage = true;
	uint32_t message;
	char* message_serveur = (char*)&message;
	int socket_desc, socket, clientLen ; 
	struct sockaddr_in server, client;
	uint32_t client_message = 0;
	socket_desc = SocketCreate();

	if(socket_desc == -1) {
		cout << "Could not create socket" << endl;
		return 1;
	}
	sockBind(socket_desc, PORT_NUMBER); 
	VideoCapture capture(0);
	cout <<"Turn ON client" << endl;
	Mat frame;
	int bytes = 0;
	listen(socket_desc, 5);
	clientLen = sizeof(struct sockaddr_in);
	socket = accept(socket_desc,(struct sockaddr *)&client, (socklen_t*)&clientLen);
	
	this_thread::sleep_for(chrono::milliseconds(100));

	capture.set(CV_CAP_PROP_FRAME_WIDTH, CAMERA_RESOLUTIONS[RES_INDEX].w);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, CAMERA_RESOLUTIONS[RES_INDEX].h);

	for (;;) {
		if (getLight() < 1014){
			if(keyPressed() == 1) {

				message = READY;
				canSendImage = true;

			} else if (canSendImage) {

				message = PUSHB;
				canSendImage = false;
				pid_t pid = fork();
				
				if (pid == 0){	
					playMusic();
					cout<< "forkOUT" << endl;
					_exit(0);	
					canSendImage = true;
				}
				
	}else{
		message = IDOWN;
	}
		write(socket, message_serveur, sizeof(uint32_t));
		recv(socket, &client_message, sizeof(uint32_t), 0);
		uint32_t newIndex = getResIndex(client_message);

		if (newIndex<sizeof(CAMERA_RESOLUTIONS) && newIndex!=resIndex){
			resIndex = newIndex;
			capture.set(CV_CAP_PROP_FRAME_WIDTH,
				CAMERA_RESOLUTIONS[resIndex].w);
			capture.set(CV_CAP_PROP_FRAME_HEIGHT,
				CAMERA_RESOLUTIONS[resIndex].h);
		}

		if(((ELE4205_OK & client_message) == ELE4205_OK) && message != IDOWN) {
		
			int result = sendFrame(capture, frame, socket, 0);
			if(result == -1){
				close(socket);
				return -1;
			}
		}
		else if ((ELE4205_QUIT & client_message) == ELE4205_QUIT) {
			close(socket);
			return 0;
		}
		this_thread::sleep_for(chrono::milliseconds(30));
	}
	return 0;
}
}