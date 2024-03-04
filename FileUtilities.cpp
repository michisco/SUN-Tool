#include <iostream>
#include <fstream>
#include <opencv2/highgui.hpp>

inline static bool saveCameraParams(const std::string& filename, cv::Size imageSize, int flags,
        const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs, double totalAvgErr) {

    cv::FileStorage fs(filename, cv::FileStorage::WRITE);
    if (!fs.isOpened())
        return false;

    char buf[1024];
    fs << "image_width" << imageSize.width;
    fs << "image_height" << imageSize.height;

    fs << "flags" << flags;
    fs << "camera_matrix" << cameraMatrix;
    fs << "distortion_coefficients" << distCoeffs;
    fs << "avg_reprojection_error" << totalAvgErr;
    return true;
}
