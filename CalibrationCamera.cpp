#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <conio.h>

#include <filesystem>
namespace fs = std::filesystem;

static inline int VisualizeMarkers() {

    system("cls");
    std::string path = "";
    std::vector<std::string> imgs;
    std::cout << "Insert the folder with images: " << std::endl;
    std::cin >> path;

    for (const auto & entry : fs::directory_iterator(path))
        imgs.push_back(entry.path().string());

    cv::Mat image, imageCopy;

    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::aruco::ArucoDetector detector(dictionary, detectorParams);

    //std::vector<int> allIds;
    //std::vector<float> repError;
    cv::namedWindow("out", cv::WINDOW_NORMAL);
    cv::resizeWindow("out", 1280, 720);

    for (int k = 0; k < imgs.size(); k++) {
        image = cv::imread(imgs.at(k));
        image.copyTo(imageCopy);

        // Error Handling
        if (image.empty()) {
            std::string errorMsg = "Image " + imgs.at(k);
            //throw invalid_argument(errorMsg + " file is empty.");
        }

        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f> > corners;
        detector.detectMarkers(imageCopy, corners, ids);

        // if at least one marker detected
        if (ids.size() > 0) {
            cv::aruco::drawDetectedMarkers(imageCopy, corners, ids);
        }

        cv::imshow("out", imageCopy);
        // Wait for any keystroke
        cv::waitKey(0);
        std::string path = "visual_" + std::to_string(k) + ".jpg";
        cv::imwrite(path, imageCopy);

        
    }

    cv::destroyWindow("out");
    return 1;
}
