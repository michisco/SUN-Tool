
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
#include <string>
#include <fstream>
#include <sstream>

#include <filesystem>
namespace fs = std::filesystem;

static inline std::vector<cv::Vec3f> createObjectMarkers(std::vector<std::string> pos_3ds, std::vector<int>* id_markers) {
    std::vector<cv::Vec3f> res;
    std::vector<std::string> row;
    std::string line, word;

    for (int k = 0; k < pos_3ds.size(); k++) {
        std::fstream file(pos_3ds.at(k), std::ios::in);
        if(file.is_open())
        {
            int i = 0;
            while(getline(file, line))
            {
                row.clear();
                std::stringstream str(line);

                while(getline(str, word, ','))
                    row.push_back(word);

                if(i>0){
                    id_markers->push_back(stoi(row[0]));
                    cv::Vec3f marker_position = cv::Vec3f(stof(row[1]), stof(row[2]), stof(row[3]));
                    res.push_back(marker_position);
                }
                i++;
            }
            file.close();
        }
        else
            std::cout<<"Could not open the file\n";
    }
    return res;
}

static inline std::string CheckCameraSide(std::vector<int> idsDetected) {

    int front_ids[16] = { 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };
    int left_ids[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    int right_ids[16] = { 32,33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47 };
    int top_ids[16] = { 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63 };

    bool found = false;
    int i = 0;
    while (i < 16 && !found) {
        int j = 0;
        while (j < idsDetected.size() && !found) {
            if (front_ids[i] == idsDetected[j])
                found = true;
            else
                j++;
        }
        
        if (!found)
            i++;
    }

    if (!found)
        return "front";

    found = false;
    i = 0;
    while (i < 16 && !found) {
        int j = 0;
        while (j < idsDetected.size() && !found) {
            if (left_ids[i] == idsDetected[j])
                found = true;
            else
                j++;
        }

        if (!found)
            i++;
    }

    if (!found)
        return "left";

    found = false;
    i = 0;
    while (i < 16 && !found) {
        int j = 0;
        while (j < idsDetected.size() && !found) {
            if (right_ids[i] == idsDetected[j])
                found = true;
            else
                j++;
        }

        if (!found)
            i++;
    }

    if (!found)
        return "right";

    return "top";
}

static inline void ComputeReprErrors(const std::vector<cv::Point3f>& objectPoints,
    const std::vector<cv::Point2f>& imagePoints,
    const std::vector<cv::Vec3d>& rvecs, const std::vector<cv::Vec3d>& tvecs,
    const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs, std::vector<int> ids) {

    std::vector<cv::Point2f> imagePoints2;
    double err;
    std::vector<double> errors;
    projectPoints(objectPoints, rvecs, tvecs, cameraMatrix, distCoeffs, imagePoints2);

    for (size_t i = 0; i < objectPoints.size(); ++i)
    {
        std::vector<cv::Point2f> tempImg1;
        std::vector<cv::Point2f> tempImg2;

        tempImg1.push_back(imagePoints[i]);
        tempImg2.push_back(imagePoints2[i]);
        err = cv::norm(tempImg1, tempImg2, cv::NORM_L2);
        errors.push_back(err);
        std::cout << "Marker: " << ids[i] << ", repError: " << err << std::endl;
        tempImg1.clear();
        tempImg2.clear();
    }
}

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

static inline int VisualizeUndistorted(char folder_path[], char file_config_path[]) {

    std::vector<std::string> imgs;
    for (const auto& entry : fs::directory_iterator(folder_path))
        imgs.push_back(entry.path().string());

    cv::Mat image, undistorted;
    cv::Mat camMatrix, distCoeffs;

    std::cout << folder_path << std::endl;
    std::cout << file_config_path << std::endl;

    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::aruco::ArucoDetector detector(dictionary, detectorParams);

    bool readOk = readCameraParams(file_config_path, camMatrix, distCoeffs);
    if (!readOk) {
        std::cout << "Invalid camera file" << std::endl;
        return 0;
    }

    cv::namedWindow("Distorted Image", cv::WINDOW_NORMAL);
    cv::resizeWindow("Distorted Image", 1280, 720);

    cv::namedWindow("Undistorted Image", cv::WINDOW_NORMAL);
    cv::resizeWindow("Undistorted Image", 1280, 720);

    for (int k = 0; k < imgs.size(); k++) {
        std::cout << k << std::endl;
        image = cv::imread(imgs.at(k));
        //image.copyTo(imageCopy);

        // Error Handling
        if (image.empty()) {
            std::string errorMsg = "Image " + imgs.at(k);
            //throw invalid_argument(errorMsg + " file is empty.");
        }
       
        // undistort
        undistort(image, undistorted, camMatrix, distCoeffs);

        // Display the original and undistorted images    
        imshow("Distorted Image", image);
        imshow("Undistorted Image", undistorted);
        std::cout << "Visualizing image " << imgs.at(k).c_str() << std::endl;
        // Wait for any keystroke
        cv::waitKey(0);
        std::string path = "visual_" + std::to_string(k) + ".jpg";
        cv::imwrite(path, undistorted);
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

    std::string path_save = "Resources/" + fileToSave;
    bool saveOk = saveCameraParams(path_save, gray.size(), flags_calib,
        cameraMatrix, distCoeffs, repError);

    if (!saveOk)
        std::cout << "error: cannot writing camera param file." << std::endl; //throw WriteErrorException();

    return 1;
}

static inline int EstimateCameraPose(char folder_path[]) {
    cv::Mat image, imageCopy;
    std::vector<cv::Mat> camMatrices, distCoeffs;

    //get all images in the folder
    std::vector<std::string> imgs;
    for (const auto& entry : fs::directory_iterator(folder_path))
        imgs.push_back(entry.path().string());

    //get all 3d marker position of the box
    std::vector<std::string> _3dpos;
    _3dpos.push_back("Resources/backsideMarkers.csv");
    _3dpos.push_back("Resources/bottomsideMarkers.csv");
    _3dpos.push_back("Resources/frontsideMarkers.csv");
    _3dpos.push_back("Resources/leftsideMarkers.csv");
    _3dpos.push_back("Resources/rightsideMarkers.csv");
    _3dpos.push_back("Resources/topsideMarkers.csv");

    //get all camera params files
    std::vector<std::string> _paramsFiles;
    _paramsFiles.push_back("Resources/camFront.yml");
    _paramsFiles.push_back("Resources/camLeft.yml");
    _paramsFiles.push_back("Resources/camRight.yml");
    _paramsFiles.push_back("Resources/camTop.yml");

    for (const auto& file : _paramsFiles) {
        cv::Mat camMatrix, distCoeff;

        bool readOk = readCameraParams(file, camMatrix, distCoeff);
        if (!readOk) {
            std::cout << "Invalid camera param file" << std::endl;
            return 0;
        }

        camMatrices.push_back(camMatrix);
        distCoeffs.push_back(distCoeff);
    }

    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::aruco::ArucoDetector detector(dictionary, detectorParams);

    std::vector<int> allIds;
    std::vector<cv::Vec3f> objPointsDefault = createObjectMarkers(_3dpos, &allIds);

    std::vector<float> repError;

    for (int k = 0; k < imgs.size(); k++) {
        image = cv::imread(imgs.at(k));
        image.copyTo(imageCopy);

        // Error Handling
        if (image.empty()) {
            std::string errorMsg = "Image " + imgs.at(k);
            //throw invalid_argument(errorMsg + " file is empty.");
        }

        cv::namedWindow("out", cv::WINDOW_NORMAL);
        cv::resizeWindow("out", 1280, 720);

        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f> > corners;
        detector.detectMarkers(imageCopy, corners, ids);

        std::string side_cam = CheckCameraSide(ids);
        int index_side = 0;
        if (side_cam == "front")
            index_side = 0;
        else if (side_cam == "left")
            index_side = 1;
        else if (side_cam == "right")
            index_side = 2;
        else
            index_side = 3;

        std::vector<cv::Vec3d> rvec, tvec;

        // estimate 3d-2d correspondences
        int n_markersDetected = 0;
        if (!ids.empty()) {
            cv::Mat objPoints, imgPoints;
            std::vector<cv::Point3f> objPnts;
            std::vector<cv::Point2f> imgPnts;

            // Get object and image points for the solvePnP function
            for (int i = 0; i < ids.size(); i++) {
                int currentId = ids.at(i);
                for (int j = 0; j < allIds.size(); j++) {
                    if (currentId == allIds[j]) {
                        n_markersDetected++;
                        float x_sum = corners[i][0].x + corners[i][1].x + corners[i][2].x + corners[i][3].x;
                        float y_sum = corners[i][0].y + corners[i][1].y + corners[i][2].y + corners[i][3].y;

                        cv::Point2f center_marker = cv::Point2f(x_sum*.25f, y_sum*.25f);

                        objPnts.push_back(objPointsDefault[j]);
                        imgPnts.push_back(center_marker);
                    }
                }
            }
            
            // create output
            cv::Mat(objPnts).copyTo(objPoints);
            cv::Mat(imgPnts).copyTo(imgPoints);

            // Find pose
            cv::solvePnPGeneric(objPoints, imgPoints, camMatrices.at(index_side), distCoeffs.at(index_side), rvec, tvec, false, cv::SOLVEPNP_ITERATIVE, cv::noArray(), cv::noArray(), repError);

            //ComputeReprErrors(objPoints, imgPnts, rvec, tvec, camMatrices.at(index_side), distCoeffs.at(index_side), ids);
        }

        cv::Mat R;
        cv::Rodrigues(rvec, R);

        cv::Mat R_T = R.t();
        float axisLength = 0.5f * ((float)5 * (20.f + 0.025f) +
                   0.025f);

        // if at least one marker detected
        if (ids.size() > 0) {
            cv::aruco::drawDetectedMarkers(imageCopy, corners, ids);

            if (n_markersDetected > 0)
                cv::drawFrameAxes(imageCopy, camMatrices.at(index_side), distCoeffs.at(index_side), rvec, tvec[0], axisLength);
        }

        cv::imshow("out", imageCopy);
        // Wait for any keystroke
        cv::waitKey(0);

        cv::Mat cam_position = -R_T * (cv::Mat) tvec[0];    
        for (int i = 0; i < 3; i++)
            cam_position.at<double>(i) = -cam_position.at<double>(i);

        std::cout << imgs.at(k) << ", Camera Position: " << cam_position << std::endl;

        for (int i = 3; i < 9; i++) {
            //if (i % 3 != 0)
            R.at<double>(i) *= -1;
        }
        std::cout << imgs.at(k) << ", Camera Rotation Matrix: " << R << std::endl;

        std::string fileToSave = "Resources/camPos_" + side_cam + ".yml";
        bool saveOk = saveCameraPose(
            fileToSave, rvec[0], tvec[0], cam_position, R, repError);
    }

    return 1;
}
