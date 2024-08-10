/**
 * @file client.hpp
 * @brief Header file for the client application.
 */
#include "client.hpp"
using namespace std;
using namespace cv;
int currentResolutionIndex = RES_INDEX;
int currentModeIndex = 0;
vector<string> personNameDatabase;
uint32_t res_array = 0;
uint32_t mode = 0;

int frame_number = 0;
int frame_number2 = 0;

string personName = "faces/";
const string databasePath = "../";


/**
 * @brief Callback function for mouse events in the menu.
 *
 * This function handles mouse events in the menu, such as button clicks.
 *
 * @param event The type of mouse event.
 * @param x The x-coordinate of the mouse position.
 * @param y The y-coordinate of the mouse position.
 * @param flags Additional flags.
 * @param userdata Additional user data (unused in this case).
 */
void ctrMenu(int event, int x, int y, int flags, void* userdata)
{
	int reslowestY = FIRST_BUTTON_Y - TEXT_H;
	int resIndex = -1;
	int modeIndex = -1;
	if  (event == EVENT_LBUTTONDOWN ) {
		for (int i = 0; i < sizeof(CAMERA_RESOLUTIONS); i++) {
			if (y > (reslowestY - PADD)
			    && y < (reslowestY + TEXT_H + PADD)
			    && x > BUTTON_X
			    && x < BUTTON_X + TEXT_W) {
				resIndex = i;
				break;
			}
			reslowestY += FIRST_BUTTON_Y;
		}
		if (y > (LEARN_Y - PADD)
			    && y < (LEARN_Y + TEXT_H + PADD)
			    && x > LEARN_X
			    && x < LEARN_X + TEXT_W) {
				modeIndex = 1;
				cout << "Apprentissage" << endl;
			
			} 
		if (y > (RECON_Y - PADD)
			    && y < (RECON_Y + TEXT_H + PADD)
			    && x > LEARN_X
			    && x < LEARN_X + TEXT_W) {
				modeIndex = 2;
				cout << "Recon" << endl;
				
			} 
	}
	if (resIndex >= 0 && currentResolutionIndex != resIndex) {
		res_array = getResArr(resIndex);
		currentResolutionIndex = resIndex;
	}
	else {
		res_array = 0;
	}
	if (modeIndex >= 0 && currentModeIndex != modeIndex) {
		mode = getModeArr(modeIndex);
		currentModeIndex = modeIndex;
		cout << currentModeIndex << endl;
	}
	else {
		mode = 0;
	}	
}
/**
 * @brief Display the menu on the given image.
 *
 * This function displays the menu on the provided image.
 *
 * @param menuImage The image to display the menu on.
 */
void fenMenu(Mat &menuImage)
{

	int yOffset = FIRST_BUTTON_Y;
	int xOffset = BUTTON_X;
	for (auto res : CAMERA_RESOLUTIONS) {
		putText(menuImage,
			to_string(res.w)+ " * " + to_string(res.h),	Point(xOffset, yOffset),FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,255), 2, false);
		yOffset += FIRST_BUTTON_Y;
		
	}
	putText(menuImage, MODE_TEXT1, Point(LEARN_X, LEARN_Y), FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,255), 2, false);
	putText(menuImage, MODE_TEXT2, Point(LEARN_X, RECON_Y), FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,255), 2, false);
	
}

/**
 * @brief Connect to a socket.
 *
 * This function creates and connects to a socket.
 *
 * @param socket The socket file descriptor.
 * @param port The port number to connect to.
 * @return 0 on success, -1 on failure.
 */
int SocketConnect(int socket, int port)
{
	// Source : https://aticleworld.com/socket-programming-in-c-using-tcpip/
	int iRetval=-1;
	struct sockaddr_in dist= {0};
	dist.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
	dist.sin_family = AF_INET;
	dist.sin_port = htons(port);
	iRetval = connect(socket,(struct sockaddr *)&dist, sizeof(struct sockaddr_in));
	return iRetval;
}

/**
 * @brief Find and save a face.
 *
 * This function finds faces on frame and saves them.
 *
 * @param frame_number Gives an ID number to the frame.
 * @param frame The last frame captured.
 * @param face_cascade The model that is used to detect faces.
 */
void saveImageLean(int frame_number, Mat frame, CascadeClassifier* face_cascade){
    Mat gray;
    cvtColor(frame, gray, COLOR_BGR2GRAY);

    vector<Rect> faces;
    face_cascade->detectMultiScale(gray, faces, 1.3, 5);

    for (const auto& face : faces) {
        rectangle(frame, face, Scalar(255, 0, 0), 2);
    }	
   
    int faceIndex = 0;
	int predLabel = -1;
	double confidence = 0.0;

    for (const auto& face : faces) {
        Mat detectedFace = (frame)(face); 
		resize(detectedFace, detectedFace,Size(252,252), INTER_LINEAR);
		string faceFileName = ("../" + personName + "/face_" + to_string(frame_number)+".png");
		imwrite(faceFileName, detectedFace);
		this_thread::sleep_for(chrono::milliseconds(100));
        cout << "Detected face saved: " << faceFileName << endl;
    }
}

/**
 * @brief Find and recognizes a face.
 *
 * This function finds faces on frame and check for its correspondance in known database.
 *
 * @param frame_number Gives an ID number to the frame.
 * @param frame The last frame captured.
 * @param face_cascade The model that is used to detect faces.
 * @param modelreco The first model to identify a face.
 * @param modelreco The second model to identify a face.
 */
void saveImage(int frame_number, Mat frame, CascadeClassifier* face_cascade, Ptr<FaceRecognizer> modelreco, Ptr<FaceRecognizer> modelreco2) {
    string frameName = ("../frames/frame_" + to_string(frame_number) + ".png");
    string faceName = ("../faces/face_" + to_string(frame_number) + ".png");

    Mat gray;
    cvtColor(frame, gray, COLOR_BGR2GRAY);

    vector<Rect> faces;
    face_cascade->detectMultiScale(gray, faces, 1.3, 5);

    for (const auto& face : faces) {
        rectangle(frame, face, Scalar(255, 0, 0), 2);
    }

	if(!imwrite(frameName, frame)){
		
		cout << "Image not saved" << endl;
	}		
	uint32_t message;
	char* message_data = (char*)&message;
   
	int socket;
	socket = SocketCreate();
	SocketConnect(socket, MUSIC_PORT);

	// Source : https://docs.opencv.org/2.4.5/modules/contrib/doc/facerec/facerec_api.html#id4
    int faceIndex = 0;
	int predLabel = -1;
	int predLabel2 = -1;
	double confidence = 0.0;

    for (const auto& face : faces) {
        Mat detectedFace = (frame)(face); 
		resize(detectedFace, detectedFace,Size(252,252), INTER_LINEAR);
		resize(gray, gray,Size(252,252), INTER_LINEAR);
		string faceFileName = ("../faces/face_" + to_string(frame_number) + "_detected_" + to_string(faceIndex++) + ".png");
		imwrite(faceFileName, detectedFace);
		string label;
		int predicted_label1 = -1;
		double predicted_confidence1 = 0.0;	
		int predicted_label2 = -1;
		double predicted_confidence2 = 0.0;	
		modelreco->predict(imread(faceFileName, CV_LOAD_IMAGE_GRAYSCALE),predicted_label1,predicted_confidence1);
		modelreco2->predict(imread(faceFileName, CV_LOAD_IMAGE_GRAYSCALE),predicted_label2,predicted_confidence2);

		if (predicted_confidence1 < predicted_confidence2 && predicted_confidence1 < CONF_THRESHOLD){
			label ="Alex";
			putText(frame, label, Point(face.x, face.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,255,0), 2);
			message = KNOWN;
			send(socket,message_data, sizeof(uint32_t), 0);

		}
		else if (predicted_confidence1 > predicted_confidence2 && predicted_confidence2 < CONF_THRESHOLD) {
			label = "Alexandre";
			putText(frame, label, Point(face.x, face.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,255,0), 2);
			message = KNOWN;
			send(socket,message_data, sizeof(uint32_t), 0);
		}
		else {

			label = "Inconnu";
			putText(frame, label, Point(face.x, face.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,255,0), 2);
			message = UNKNOWN;
			send(socket,message_data, sizeof(uint32_t), 0);
		}
	
		imwrite(frameName, frame);
    
        cout << "Detected face saved: " << faceFileName << endl;
		this_thread::sleep_for(chrono::milliseconds(100));
    }	
	close(socket);
}

/**
 * @brief Get the list of files in a directory with a specific extension.
 *
 * This function retrieves the list of files in the specified directory (`dirPath`)
 * with the specified extension (".png" in this case).
 *
 * @param dirPath The path to the directory.
 * @return A vector of strings representing the file paths.
 */
vector<string> getFilesInDirectory(const string& dirPath) {
    vector<string> files;
    DIR* dir = opendir(dirPath.c_str());

    if (dir) {
        dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            string fileName = entry->d_name;
            
            size_t pos = fileName.find_last_of(".");
            if (pos != string::npos && fileName.substr(pos) == ".png") {
                files.push_back(dirPath + fileName);
            }
        }
        closedir(dir);
    }
    return files;
}

/**
 * @brief Create a face recognition database and save the trained model.
 *
 * This function creates a face recognition database using images from the specified directory.
 * It trains a face recognition model using the Eigenfaces algorithm and saves the trained model.
 */
void createDatabase() {
	// Source : https://docs.opencv.org/2.4.5/modules/contrib/doc/facerec/facerec_api.html#id4

    Ptr<FaceRecognizer> model = createEigenFaceRecognizer();
    vector<Mat> images;
    vector<int> labels;
    string faceFolderPath = databasePath + personName + "/";
	string modelFolderPath =  databasePath + personName + MODEL_PATH;
    vector<string> imagePaths = getFilesInDirectory(faceFolderPath);

    for (const auto& imagePath : imagePaths) {
        Mat face = imread(imagePath, IMREAD_GRAYSCALE);
        if (!face.empty()) {
            images.push_back(face);
            labels.push_back(0);  
        }
    }

    if (images.empty() || labels.empty()) {
        cerr << "No training data loaded" << endl;
    }

    model->train(images, labels);
	
    model->save(modelFolderPath);
	
    cout << "Entraînement terminé. Modèle sauvegardé sous '" << modelFolderPath << endl;

}

/**
 * @brief Update a face recognition database with new images and save the trained model.
 *
 * This function updates an existing face recognition database by adding new images from the specified directory.
 * It loads the existing face recognition model, updates it with new images, and saves the updated model.
 */
void updateDatabase() {
	// Source : https://docs.opencv.org/2.4.5/modules/contrib/doc/facerec/facerec_api.html#id4

 	Ptr<FaceRecognizer> model = createEigenFaceRecognizer();
    vector<Mat> images;
    vector<int> labels;
    string faceFolderPath = databasePath + personName + "/";
	string modelFolderPath =  databasePath + personName + MODEL_PATH;
	model->load(modelFolderPath + MODEL_PATH);
    vector<string> imagePaths = getFilesInDirectory(faceFolderPath);

    for (const auto& imagePath : imagePaths) {
        Mat face = imread(imagePath, IMREAD_GRAYSCALE);
        if (!face.empty()) {
            images.push_back(face);
            labels.push_back(0);  
        }
    }

    if (images.empty() || labels.empty()) {
        cerr << "No training data loaded" << endl;
    }

    model->update(images, labels);

    model->save(modelFolderPath);

    cout << "Entraînement terminé. Modèle sauvegardé sous '" << modelFolderPath << endl;
}

/**
 * @brief Check if a directory exists.
 *
 * This function checks if a directory specified by `personName` exists in the parent directory ("../").
 *
 * @return True if the directory exists, false otherwise.
 */
bool checkDir(){
	string path = "../" + personName;
	DIR* dir = opendir(path.c_str());
	if (dir) {
		closedir(dir);
		return true;
	}else{
		return false;
	}
}
/**
 * @brief Create a new directory.
 *
 * This function creates a new directory specified by `personName` in the parent directory ("../").
 * It uses the "mkdir" command with the "-p" option to create parent directories as needed.
 */
void mkdir(){
	string newDirectoryPath = "../" + personName;

    string command = "mkdir -p " + string(newDirectoryPath);
    int result = system(command.c_str());

    if (result == 0) {
        cout << "Directory created :  " << newDirectoryPath << endl;
    } else {
        cerr << "Failed to create directory" << newDirectoryPath << endl;
    }
}

/**
 * @brief Main function of the client application.
 *
 * The main function of the client application handles socket communication, menu display,
 * and image processing.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return 0 on success, 1 on failure.
 */
int main(int argc, char *argv[])
{
	bool data_ok = true;
	int socket, read_size;
	struct sockaddr_in server;
	vector<Mat> detectedFaces;
	Mat gray;
	Ptr<FaceRecognizer> model = createEigenFaceRecognizer();
	uint32_t message;
	char* message_data = (char*)&message;
	uint32_t server_message = 0;
	socket = SocketCreate();
	Ptr<FaceRecognizer> modelreco = createEigenFaceRecognizer();
	Ptr<FaceRecognizer> modelreco2 = createEigenFaceRecognizer();
	modelreco->load("../trained_model.xml");
	modelreco2->load("../trained_model2.xml");
	Mat menu(MENU_H, MENU_W, CV_64FC4);
	fenMenu(menu);
	imshow(MENU_NAME, menu);
	setMouseCallback(MENU_NAME, ctrMenu, NULL);
	Mat blankFrame(BLANK_FRAME_H, BLANK_FRAME_W, CV_64FC4);
	CascadeClassifier face_cascade;
	face_cascade.load("./haarcascade_frontalface_default.xml");
	
	
	if(socket == -1) {
		cout << "Could not create socket" << endl;
		return 1;
	}

	cout << "Socket is created" << endl;

	if (SocketConnect(socket) < 0) {
		cout << "connection to socket failed" << endl;
		return 1;
	}

	cout << "Connection approuved" << endl;
	


	for (;;) {
		
		if (recv(socket, &server_message, sizeof(uint32_t), 0) == -1){
			cout << "Reception error" << endl;
		}
		
		bool mustQuit = false;
		int end_key = waitKey(ESC_WAIT_TIME)&0xFF;
		message = ELE4205_OK;
		if(end_key == ESC){
			message = ELE4205_END;
			mustQuit = true;
			
		}

		message |= res_array;
		send(socket, message_data, sizeof(uint32_t), 0);

		if (mustQuit){
			close(socket);
			return 0;
		}

		if(server_message == READY || server_message == PUSHB){
	
			char *entete;
			read_size = recv(socket, entete, sizeof(Mat),0);
			Mat* head = reinterpret_cast<Mat*>(entete); 
			int imgSize = head->rows * head->cols * CV_ELEM_SIZE(head->flags);
			uchar* sockData = new uchar[imgSize];
			int bytes = 0;

			for (int i = 0; i < imgSize; i += bytes) {
				bytes = recv(socket, sockData+i, imgSize-i, 0);
				if (bytes == -1){
					cout << "reception error" << endl;	
				}
			}

			Mat frame(head->rows,head->cols, head->type(), sockData);

            if (currentModeIndex == 1)  {
				cin >> personName;
				bool dirExist = checkDir();

				if (dirExist && personName != "quit"){
					if ( find(personNameDatabase.begin(), personNameDatabase.end(), personName) != personNameDatabase.end() )
						{personNameDatabase.push_back(personName);}
					personName += "/faces";
					saveImageLean(frame_number2, frame, &face_cascade);
				}else if (!dirExist && personName != "quit"){
					personNameDatabase.push_back(personName);
					personName += "/faces";
					mkdir();
					saveImageLean(frame_number2, frame, &face_cascade);

				}else {
					for (auto name :  personNameDatabase){
						personName = name + "/model";
						mkdir();
						personName = name;
						createDatabase();
					}
				}
			}

			if (server_message == PUSHB && data_ok){
				data_ok = false;
				frame_number++;
				pid_t pid = fork();
				if(pid==0){
					saveImage(frame_number, frame, &face_cascade, modelreco, modelreco2);
					_exit(0);
					
				}else{
					waitpid(pid, NULL, 0);
				}
				imshow("Visage Trouve", imread("../frames/frame_" + to_string(frame_number) + ".png"));
			}
			imshow(CAM_NAME, frame);
			delete sockData;
		} else if (server_message == IDOWN) {
			imshow(CAM_NAME, blankFrame);
		}
		if(server_message != PUSHB){
			data_ok = true;
		}
	}
	close(socket);
	return 0;
}