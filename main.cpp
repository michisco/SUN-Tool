#include "FileUtilities.cpp"
#include "AcquisitionSetup.cpp"
#include "CalibrationCamera.cpp"
#include "DevicesPose.cpp"
#include "ObjectsPose.cpp"

#include <QApplication>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

void PrintMessage() {
    std::cout << "usage: ${TARGET_PATH} [Command] [Optional arguments]\n"
        "Available command:\n"
        "acquire - Connect to GoPro cameras to acquire images.\n"
        "visualize [IMGS_FOLDER_PATH] - Visualize images in the folder and detect their markers.\n"
        "calibrate [IMGS_FOLDER_PATH] [SAVING_FILE_YML] - Calibrate one camera and save the param file.yml.\n"
        "camera_position [IMGS_FOLDER_PATH] - Compute camera position from image and save it into a file.yml.\n"
        "object_position - Compute object position and save it into a file.yml.\n"
        "hand_position - Compute hand device position and save it into a file.yml.\n"
        "help - Open the guide." << std::endl;
}


int main(int argc, char *argv[])
{
    
    //check if exist the first argument to set number of iterations
    if (argc < 2) {
        PrintMessage();
        return 0;
    }
    else if (argc >= 2) {
        if (std::string(argv[1]) == "help" || argv[1][0] == 'H') {
            std::cout << "--- Help ---" << std::endl;
            PrintMessage();
            return 0;
        }
        else if (std::string(argv[1]) == "acquire") {
            QCoreApplication a(argc, argv);
            AcquisitionSetup();
            //return a.exec();
        }
        else if (std::string(argv[1]) == "visualize") {
            if (argc >= 3) {
                VisualizeMarkers(argv[2]);
            }
            else {
                std::cout << "error: missing folder path. usage: ${TARGET_PATH} visualize [IMGS_FOLDER_PATH]" << std::endl;
            }
        }
        else if (std::string(argv[1]) == "visualize_undistort") {
            if (argc >= 4) {
                VisualizeUndistorted(argv[2], argv[3]);
            }
            else {
                std::cout << "error: missing folder path. usage: ${TARGET_PATH} visualize_undistort [IMGS_FOLDER_PATH] [FILE_CONFIG_PATH]" << std::endl;
            }
        }
        else if (std::string(argv[1]) == "calibrate") {
            if (argc >= 3) {
                if (argc >= 4)
                    CalibrationCamera(argv[2], std::string(argv[3]));
                else
                    CalibrationCamera(argv[2], "paramsCam.yml");
            }
            else {
                std::cout << "error: missing folder path. usage: ${TARGET_PATH} calibrate [IMGS_FOLDER_PATH] [SAVING_FILE_YML]" << std::endl;
            }
        }
        else if (std::string(argv[1]) == "camera_position") {
            if (argc >= 3) {
                EstimateCameraPose(argv[2]);
            }
            else {
                std::cout << "error: missing folder path. usage: ${TARGET_PATH} camera_position [IMGS_FOLDER_PATH]" << std::endl;
            }
        }
        else if (std::string(argv[1]) == "object_position") {
            EstimateObjectPose();
        }
        else if (std::string(argv[1]) == "hand_position") {
            EstimateHandPose();
        }
        else {
            std::cout << "error: command not recognized. Use: ${TARGET_PATH} help" << std::endl;
        }
    }

    return 0;
}
