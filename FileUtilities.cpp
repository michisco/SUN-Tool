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

inline static bool saveCameraPose(const std::string& filename, cv::Vec3d r, cv::Vec3d t, cv::Mat m, cv::Mat m_rot, std::vector<float> errors) {

    cv::FileStorage fs(filename, cv::FileStorage::WRITE);
    if (!fs.isOpened())
        return false;

    char buf[1024];
    fs << "rvec" << r;
    fs << "tvec" << t;
    fs << "cameraPosition" << m;
    fs << "rotationMatrix" << m_rot;
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
    std::ofstream ofile("MLPs/handEstimation.mlp", std::ios_base::binary | std::ios_base::out);
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

    std::ofstream ofile("MLPs/boxEstimations.mlp", std::ios_base::binary | std::ios_base::out);
    std::string line;

    ofile << "<!DOCTYPE MeshLabDocument>\n" << std::flush;
    ofile << "<MeshLabProject>\n" << std::flush;
    ofile << " <MeshGroup>\n" << std::flush;
    ofile << "  <MLMesh label=\"box.ply\" visible=\"1\" idInFile=\"-1\" filename=\"box.ply\">\n" << std::flush;
    ofile << "   <MLMatrix44>\n" << std::flush;
    ofile << "1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1 \n" << std::flush;
    ofile << "</MLMatrix44>\n" << std::flush;

    ofile << "   <RenderingOption pointSize=\"3\" boxColor=\"234 234 234 255\" solidColor=\"192 192 192 255\" wireWidth=\"1\" pointColor=\"252 233 79 255\" wireColor=\"64 64 64 255\">100001010000000000000100000001010110000010100000000100111010100000001001</RenderingOption>\n" << std::flush;
    ofile << "  </MLMesh>\n" << std::flush;
    ofile << "  <MLMesh label=\"Rotate" << 0 << "\" visible=\"0\" idInFile=\"-1\" filename=\"object.ply\">\n" << std::flush;
    ofile << "   <MLMatrix44>\n" << std::flush;
    for (int j = 0; j < matrices.at(0).rows; j++) {
        for (int z = 0; z < matrices.at(0).cols; z++)
            ofile << matrices.at(0)(j, z) << " " << std::flush;
    }
    ofile << "\n" << std::flush;
    ofile << "</MLMatrix44>\n" << std::flush;

    for (int i = 1; i < matrices.size(); i++) {
        ofile << "   <RenderingOption pointSize=\"3\" boxColor=\"234 234 234 255\" solidColor=\"192 192 192 255\" wireWidth=\"1\" pointColor=\"252 233 79 255\" wireColor=\"64 64 64 255\">100001000000000000000100000001010100000010100000000100111011110000001001</RenderingOption>\n" << std::flush;
        ofile << "  </MLMesh>\n" << std::flush;
        ofile << "  <MLMesh label=\"Rotate" << i << "\" visible=\"0\" idInFile=\"-1\" filename=\"object.ply\">\n" << std::flush;
        ofile << "   <MLMatrix44>\n" << std::flush;
        for (int j = 0; j < matrices.at(i).rows; j++) {
            for (int z = 0; z < matrices.at(i).cols; z++)
                ofile << matrices.at(i)(j, z) << " " << std::flush;
        }
        ofile << "\n" << std::flush;
        ofile << "</MLMatrix44>\n" << std::flush;
    }
    ofile << "   <RenderingOption pointSize=\"3\" boxColor=\"234 234 234 255\" solidColor=\"192 192 192 255\" wireWidth=\"1\" pointColor=\"252 233 79 255\" wireColor=\"64 64 64 255\">100001000000000000000100000001010100000010100000000100111011110000001001</RenderingOption>\n" << std::flush;
    ofile << "  </MLMesh>\n" << std::flush;
    ofile << " </MeshGroup>\n" << std::flush;
    ofile << " <RasterGroup/>\n" << std::flush;
    ofile << "</MeshLabProject>\n" << std::flush;
    ofile.close();

    return true;
}

inline static bool saveCSVData(const std::string filename, std::vector<std::vector<int>> fingers_data) {
    std::fstream fout;
    std::string path_filename = "Resources/" + filename + ".csv";
    fout.open(path_filename, std::ios::out | std::ios::app);
    fout << "thumb, index, medium, pinkie\n";

    for (int i = 0; i < fingers_data.size(); i++) {
        fout << fingers_data[i][0] << ", "
            << fingers_data[i][1] << ", "
            << fingers_data[i][2] << ", "
            << fingers_data[i][3] << "\n";
    }

    fout.close();
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
