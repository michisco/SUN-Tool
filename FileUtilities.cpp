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

inline static bool saveCameraPose(const std::string& filename, cv::Vec3d r, cv::Vec3d t, std::vector<float> errors) {

    cv::FileStorage fs(filename, cv::FileStorage::WRITE);
    if (!fs.isOpened())
        return false;

    char buf[1024];
    fs << "rvec" << r;
    fs << "tvec" << t;
    fs << "repError" << errors;

    return true;
}

inline static bool saveObjectPose(const std::string& filename, std::vector<cv::Matx44d> m, int nframes) {

    cv::FileStorage fs(filename, cv::FileStorage::WRITE);
    if (!fs.isOpened())
        return false;

    char buf[1024];

    for (int i = 0; i < nframes; i++) {
        fs << "frames" << i;
        fs << "matrix" << m[i];
    }

    // Create and open a text file
    std::ofstream MyFile("Resources/ObjectPose_MeshLab.txt");

    for (int i = 0; i < nframes; i++) {
        for (int j = 0; j < m[i].rows; j++) {
            for (int z = 0; z < m[i].cols; z++)
                MyFile << m[i](j, z) << " ";
        }
        MyFile << "\n";
    }

    // Close the file
    MyFile.close();

    return true;
}

inline static bool readCameraParams(const std::string filename, cv::Mat& camMatrix, cv::Mat& distCoeffs) {
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if (!fs.isOpened())
        return false;

    fs["camera_matrix"] >> camMatrix;
    fs["distortion_coefficients"] >> distCoeffs;
    return true;
}

inline static bool readCameraPose(const std::string filename, cv::Vec3d& r, cv::Vec3d& t) {
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if (!fs.isOpened())
        return false;

    fs["rvec"] >> r;
    fs["tvec"] >> t;
    return true;
}
