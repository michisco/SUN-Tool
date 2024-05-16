#include <iostream>
#include <fstream>
#include <string>
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

inline static bool saveCameraPose(const std::string& filename, cv::Vec3d r, cv::Vec3d t, cv::Mat m, std::vector<float> errors) {

    cv::FileStorage fs(filename, cv::FileStorage::WRITE);
    if (!fs.isOpened())
        return false;

    char buf[1024];
    fs << "rvec" << r;
    fs << "tvec" << t;
    fs << "cameraPosition" << m;
    fs << "repError" << errors;

    return true;
}

inline static bool saveObjectPose(const std::string& filename, std::vector<cv::Matx44d> m, std::vector<cv::Vec3d> r, std::vector <cv::Vec3d> t, int nframes) {

    std::string pathFile = filename + ".yml";
    cv::FileStorage fs(pathFile, cv::FileStorage::WRITE);
    if (!fs.isOpened())
        return false;

    char buf[1024];

    fs << "frames" << "[";
    for (int i = 0; i < nframes; i++) {
        fs << "{";
        fs << "frame" << i;
        fs << "rvec" << r[i];
        fs << "tvec" << t[i];
        fs << "matrix" << m[i];
        fs << "}";
    }
    fs << "]";

    // Create and open a text file
    std::string meshLabFile = filename + "_MeshLab.txt";
    std::ofstream MyFile(meshLabFile);

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

inline static bool saveHandMLPFile(const std::string& filename, std::vector<cv::Matx44d> matrices) {

    std::string pathFile = "MLPs/boxHand.mlp";
    std::ifstream myfile;
    std::ofstream ofile("MLPs/box_handNEW.mlp", std::ios_base::binary | std::ios_base::out);
    std::string line;
    myfile.open(pathFile, std::ios_base::binary);

    int count = 1;
    int i = 0;
    if (myfile.is_open())
    {
        while (std::getline(myfile, line))
        {         
            if (count % 6 == 0 && count > 6 && i < matrices.size()) {
                for (int j = 0; j < matrices.at(i).rows; j++) {
                    for (int z = 0; z < matrices.at(i).cols; z++)
                        ofile << matrices.at(i)(j, z) << " " << std::flush;          
                } 
                ofile << "\n" << std::flush;
                i++;
            }   
            else {
                ofile << line << std::endl;
            }
            count++;
        }
        myfile.close();
        ofile.close();
    }
    
    return true;
}

inline static bool saveBoxMLPFile(const std::string& filename, std::vector<cv::Matx44d> matrices) {

    std::string pathFile = "MLPs/box.mlp";
    std::ifstream myfile;
    std::ofstream ofile("MLPs/box_NEW.mlp", std::ios_base::binary | std::ios_base::out);
    std::string line;
    myfile.open(pathFile, std::ios_base::binary);

    int count = 1;
    int i = 0;
    if (myfile.is_open())
    {
        while (std::getline(myfile, line))
        {
            if (count % 6 == 0 && count > 6 && i < matrices.size()) {
                for (int j = 0; j < matrices.at(i).rows; j++) {
                    for (int z = 0; z < matrices.at(i).cols; z++)
                        ofile << matrices.at(i)(j, z) << " " << std::flush;
                }
                ofile << "\n" << std::flush;
                i++;
            }
            else {
                ofile << line << std::endl;
            }
            count++;
        }
        myfile.close();
        ofile.close();
    }

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

inline static bool readObjectPose(const std::string filename, std::vector<cv::Vec3d> &r, std::vector <cv::Vec3d> &t, std::vector<cv::Matx44d> &m) {
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if (!fs.isOpened())
        return false;

    cv::FileNode fn = fs["frames"];
    for (cv::FileNodeIterator it = fn.begin(); it != fn.end(); it++)
    {
        cv::FileNode item = *it;

        cv::Vec3d r_temp, t_temp, id_frame;
        cv::Matx44d m_temp;

        item["frame"] >> id_frame;
        item["rvec"] >> r_temp;
        item["tvec"] >> t_temp;
        item["matrix"] >> m_temp;

        r.push_back(r_temp);
        t.push_back(t_temp);
        m.push_back(m_temp);
    }

    return true;
}
