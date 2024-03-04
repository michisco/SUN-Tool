#include "fileUtilities.cpp"

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

#include <filesystem>
namespace fs = std::filesystem;

static inline int VisualizeMarkers(char folder_path[]) {

    std::vector<std::string> imgs;
    for (const auto & entry : fs::directory_iterator(folder_path))
        imgs.push_back(entry.path().string());

    cv::Mat image, imageCopy;

    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::aruco::ArucoDetector detector(dictionary, detectorParams);

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
        std::cout << "Visualizing image " << imgs.at(k).c_str() << std::endl;
        // Wait for any keystroke
        cv::waitKey(0);
        std::string path = "visual_" + std::to_string(k) + ".jpg";
        cv::imwrite(path, imageCopy);

        
    }

    cv::destroyWindow("out");
    return 1;
}

static inline int CalibrationCamera(char folder_path[], std::string fileToSave) {
    std::vector<std::vector<cv::Point3f>> _3Dpoints;
    std::vector<std::vector<cv::Point2f>> _2Dpoints;
    double square_size = 23.f;
    int width_board = 8;
    int height_board = 6;

    std::vector<std::string> imgs;
    for (const auto& entry : fs::directory_iterator(folder_path))
        imgs.push_back(entry.path().string());

    std::vector<cv::Point3f> worldPoints;
    for (int i = 0; i < height_board; i++) {
        for (int j = 0; j < width_board; j++) {
            worldPoints.push_back(cv::Point3f(j * square_size, i * square_size, 0.f));
        }
    }

    cv::Mat frame, gray;
    cv::namedWindow("out", cv::WINDOW_NORMAL);
    cv::resizeWindow("out", 1280, 720);

    std::vector<cv::Point2f> corner_pts;
    int flags_calib = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE;
    bool success;

    // Looping over all the images in the directory
    for (int i = 0; i < imgs.size(); i++)
    {
        frame = cv::imread(imgs.at(i));

        if (frame.empty()) {
            std::string errorMsg = "Image " + imgs.at(i);
            //throw invalid_argument(errorMsg + " file is empty.");
        }

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        success = cv::findChessboardCorners(frame, cv::Size(width_board, height_board), corner_pts, flags_calib);

        if (success)
        {
            cv::TermCriteria criteria(cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 30, 0.0001);

            // refining pixel coordinates for given 2d points.
            cv::cornerSubPix(gray, corner_pts, cv::Size(11, 11), cv::Size(-1, -1), criteria);

            // Displaying the detected corner points on the checker board
            cv::drawChessboardCorners(gray, cv::Size(width_board, height_board), corner_pts, success);

            _3Dpoints.push_back(worldPoints);
            _2Dpoints.push_back(corner_pts);
        }

        cv::imshow("out", gray);
        std::cout << "Calibrating image " << imgs.at(i).c_str() << std::endl;
        cv::waitKey(0);
    }

    cv::Mat cameraMatrix, distCoeffs;
    std::vector<cv::Mat> rvecs, tvecs;
    double repError = cv::calibrateCamera(_3Dpoints, _2Dpoints, cv::Size(gray.rows, gray.cols), cameraMatrix, distCoeffs, rvecs, tvecs);

    std::string path_save = "Resources/Calibration/" + fileToSave;
    bool saveOk = saveCameraParams(path_save, gray.size(), flags_calib,
        cameraMatrix, distCoeffs, repError);

    if (!saveOk)
        std::cout << "error: cannot writing camera param file." << std::endl; //throw WriteErrorException();

    return 1;
}
