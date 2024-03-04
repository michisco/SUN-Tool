#include "AcquisitionSetup.cpp"
#include "CalibrationCamera.cpp"

#include <stdio.h>
#include <iostream>
#include <stdlib.h>


int main(int argc, char *argv[])
{
    //check if exist the first argument to set number of iterations
    if (argc < 2) {
        std::cout << "usage: ${TARGET_PATH} [Command] [Optional arguments]\n"
            "Available command:\n"
            "acquisition - Connect to GoPro cameras to acquire images.\n"
            "visualize [FOLDER_PATH] - Visualize images in the folder and detect their markers.\n"
            "help - Open the guide." << std::endl;
        return 0;
    }
    else if (argc >= 2) {
        if (std::string(argv[1]) == "help" || argv[1][0] == 'H' || argv[1][0] == 'h') {
            std::cout << "--- Help ---" << std::endl;
            std::cout << "usage: ${TARGET_PATH} [Command] [Optional arguments]\n"
                "Available command:\n"
                "acquire - Connect to GoPro cameras to acquire images.\n"
                "visualize [FOLDER_PATH] - Visualize images in the folder and detect their markers.\n"
                "help - Open this guide." << std::endl;
            return 0;
        }
        else if (std::string(argv[1]) == "acquire") {
            AcquisitionSetup();
        }
        else if (std::string(argv[1]) == "visualize") {
            if (argc >= 3) {
                VisualizeMarkers(argv[2]);
            }
            else {
                std::cout << "error: missing folder path. usage: ${TARGET_PATH} visualize [FOLDER_PATH]" << std::endl;
            }
        }
        else {
            std::cout << "error: command not recognized. Use: ${TARGET_PATH} help" << std::endl;
        }
    }

    return 0;
}
