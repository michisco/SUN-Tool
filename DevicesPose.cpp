#include "ObjectMarkers.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

#include <iostream>

struct ObjectData {
    std::vector<int>  id_marker;
    std::vector<cv::Point3f> pos3D_marker;
    std::vector<cv::Point2f> pos2D_marker;

    cv::Vec3d tvec_cam;
    cv::Vec3d rvec_cam;

    cv::Mat cam_matrix;
    cv::Mat dist_coeffs;
};

class SolveObjectPose : public cv::LMSolver::Callback
{
    int npoints;
    std::vector<ObjectData>* objectData;
    std::vector<cv::Point3f> objPoints;

public:
    SolveObjectPose(std::vector<ObjectData>* objdatas) //SolveObjectPose(InputArray _opoints, InputArray _ipoints, InputArray _cameraPosition /*,InputArray _cameraMatrix, InputArray _distCoeffs)
    {
        npoints = 0;
        objectData = objdatas;
        for (int i = 0; i < objectData->size(); i++)
            npoints += objectData->at(i).id_marker.size();
    }

    bool compute(cv::InputArray _param, cv::OutputArray _err, cv::OutputArray _Jac) const
    {
        cv::Mat temp = _param.getMat();
        cv::Mat rvec_temp = cv::Mat::zeros(3, 1, CV_64F);
        cv::Mat tvec_temp = cv::Mat::zeros(3, 1, CV_64F);
        for (int i = 0; i < 3; i++) {
            rvec_temp.at<double>(i, 0) = temp.at<double>(i);
            tvec_temp.at<double>(i, 0) = temp.at<double>(i + 3);
        }

        std::vector<cv::Affine3d> paramsJac;
        double eps_tvec = 0.1;
        double eps_rvec = 0.1;

        for (int i = 0; i < 3; i++) {
            cv::Mat rvec_cp = rvec_temp;

            rvec_cp.at<double>(i) += eps_rvec;
            paramsJac.push_back(cv::Affine3d(rvec_cp, tvec_temp));

            rvec_cp.at<double>(i) -= eps_rvec * 2;
            paramsJac.push_back(cv::Affine3d(rvec_cp, tvec_temp));
        }

        for (int i = 0; i < 3; i++) {
            cv::Mat tvec_cp = tvec_temp;

            tvec_cp.at<double>(i) += eps_tvec;
            paramsJac.push_back(cv::Affine3d(rvec_temp, tvec_cp));

            tvec_cp.at<double>(i) -= eps_tvec * 2;
            paramsJac.push_back(cv::Affine3d(rvec_temp, tvec_cp));
        }

        //cv::Mat R = cv::Mat::zeros(3, 3, CV_64F);
        //cv::Rodrigues(rvec_temp, R);
        cv::Affine3d param(rvec_temp, tvec_temp);

        _err.create(npoints * 2, 1, CV_64FC1);
        cv::Mat err = _err.getMat();

        if (_Jac.needed())
        {
            _Jac.create(npoints * 2, _param.getMat().rows, CV_64FC1);
        }

        cv::Mat jac = _Jac.getMat();
        int indexError = 0;
        for (int i = 0; i < objectData->size(); i++) {
            std::vector<cv::Point3f> vec_point(objectData->at(i).pos3D_marker.size());

            for (int j = 0; j < objectData->at(i).pos3D_marker.size(); j++) {
                vec_point[j] = param * objectData->at(i).pos3D_marker.at(j);

                if (_Jac.needed()){
                    for (int p = 0; p < (rvec_temp.rows + tvec_temp.rows); p++) {
                        cv::Point3f temp3f_eps = paramsJac[p * 2] * objectData->at(i).pos3D_marker.at(j);
                        cv::Point3f temp3f_eps1 = paramsJac[p * 2 + 1] * objectData->at(i).pos3D_marker.at(j);

                        std::vector<cv::Point3f> pointJac;
                        pointJac.push_back(temp3f_eps);
                        pointJac.push_back(temp3f_eps1);

                        std::vector<cv::Point2f> temp_pts;
                        projectPoints(pointJac, objectData->at(i).rvec_cam,
                            objectData->at(i).tvec_cam, objectData->at(i).cam_matrix, objectData->at(i).dist_coeffs,
                            temp_pts, cv::noArray());

                        if (p < 3) {
                            jac.at<double>(indexError + j * 2, p) = (temp_pts[0].x - temp_pts[1].x) / 2 * eps_rvec;
                            jac.at<double>(indexError + j * 2 + 1, p) = (temp_pts[0].y - temp_pts[1].y) / 2 * eps_rvec;
                        }
                        else {
                            jac.at<double>(indexError + j * 2, p) = (temp_pts[0].x - temp_pts[1].x) / 2 * eps_tvec;
                            jac.at<double>(indexError + j * 2 + 1, p) = (temp_pts[0].y - temp_pts[1].y) / 2 * eps_tvec;
                        }
                    }
                }
            }

            std::vector<cv::Point2f> projectedPts;
            projectPoints(vec_point, objectData->at(i).rvec_cam,
                                objectData->at(i).tvec_cam, objectData->at(i).cam_matrix, objectData->at(i).dist_coeffs,
                                projectedPts, cv::noArray());

            for(int j = 0; j < objectData->at(i).pos3D_marker.size(); j++){
                err.at<double>(indexError+j*2) = projectedPts[j].x - objectData->at(i).pos2D_marker[j].x;
                err.at<double>(indexError+j*2+1) = projectedPts[j].y - objectData->at(i).pos2D_marker[j].y;
            }

            indexError += objectData->at(i).pos3D_marker.size()*2;
        }
        return true;
    }
};

static inline bool CheckObjectIDs(std::vector<int> idsObject, int idToCheck){
    for(int i = 0; i < idsObject.size(); i++){
        if(idsObject.at(i) == idToCheck)
            return true;
    }
    return false;
}

static inline void ReadObjectCoordinates(std::string marker_3ds, ObjectMarkers* info) {
    std::vector<std::string> row;
    std::string line, word;

    std::fstream file(marker_3ds, std::ios::in);
    if (file.is_open())
    {
        int i = 0;
        while (getline(file, line))
        {
            row.clear();
            std::stringstream str(line);

            while (getline(str, word, ','))
                row.push_back(word);

            if (i > 0) {
                info->Set(stoi(row[0]), cv::Point3f(stof(row[1]), stof(row[2]), stof(row[3])));
            }
            i++;
        }
        file.close();
    }
    else
        std::cout << "Could not open the file\n";
}

static inline void ComputeObjectPose(std::vector<ObjectData> *objdatas, cv::InputOutputArray _rvec, cv::InputOutputArray _tvec){
    cv::Mat rvec0 = _rvec.getMat(), tvec0 = _tvec.getMat();
    cv::Mat params(6, 1, CV_64FC1);
    for (int i = 0; i < 3; i++)
    {
        params.at<double>(i,0) = rvec0.at<double>(i,0);
        params.at<double>(i+3,0) = tvec0.at<double>(i,0);
    }

    auto createLM = cv::LMSolver::create(cv::makePtr<SolveObjectPose>(objdatas), 1000, 0.01);
    int itr = createLM->run(params);
    int itr2 = createLM->run(params);

    params.rowRange(0, 3).convertTo(rvec0, rvec0.depth());
    params.rowRange(3, 6).convertTo(tvec0, tvec0.depth());
}

static inline int EstimateObjectPose() {

    std::vector<cv::Mat> camMatrix, distCoeffs;
    std::vector<ObjectData> markers_data;
    ObjectMarkers marker_info = ObjectMarkers();

    //get all images in the folder
    std::vector<std::string> imgs_FCam;
    for (const auto& entry : fs::directory_iterator("Data/Front"))
        imgs_FCam.push_back(entry.path().string());

    std::sort(imgs_FCam.begin(), imgs_FCam.end(),
                  [](const auto& lhs, const auto& rhs) {
                      return lhs < rhs;
                  });

    std::vector<std::string> imgs_TCam;
    for (const auto& entry : fs::directory_iterator("Data/Top"))
        imgs_TCam.push_back(entry.path().string());

    std::sort(imgs_TCam.begin(), imgs_TCam.end(),
                  [](const auto& lhs, const auto& rhs) {
                      return lhs < rhs;
                  });

    std::vector<std::string> imgs_LCam;
    for (const auto& entry : fs::directory_iterator("Data/Left"))
        imgs_LCam.push_back(entry.path().string());

    std::sort(imgs_LCam.begin(), imgs_LCam.end(),
                  [](const auto& lhs, const auto& rhs) {
                      return lhs < rhs;
                  });

    std::vector<std::string> imgs_RCam;
    for (const auto& entry : fs::directory_iterator("Data/Right"))
        imgs_RCam.push_back(entry.path().string());

    std::sort(imgs_RCam.begin(), imgs_RCam.end(),
                  [](const auto& lhs, const auto& rhs) {
                      return lhs < rhs;
                  });

    std::string _3dMarkerFile = "Resources/ObjectMarkers.csv";

    //get all camera params files
    std::vector<std::string> _paramsFiles;
    _paramsFiles.push_back("Resources/camFront.yml");
    _paramsFiles.push_back("Resources/camLeft.yml");
    _paramsFiles.push_back("Resources/camRight.yml");
    _paramsFiles.push_back("Resources/camTop.yml");

    for (const auto& file : _paramsFiles) {
        cv::Mat temp_matrix, temp_dist;

        bool readOk = readCameraParams(file, temp_matrix, temp_dist);
        if (!readOk) {
            std::cout << "Invalid camera param file" << std::endl;
            return 0;
        }

        camMatrix.push_back(temp_matrix);
        distCoeffs.push_back(temp_dist);
    }

    std::vector<cv::Vec3d> cam_tvecs;
    std::vector<cv::Vec3d> cam_rvecs;

    //get position cameras
    std::vector<std::string> _3dposCameras;
    _3dposCameras.push_back("Resources/camPos_front.yml");
    _3dposCameras.push_back("Resources/camPos_left.yml");
    _3dposCameras.push_back("Resources/camPos_right.yml");
    _3dposCameras.push_back("Resources/camPos_top.yml");

    for (const auto& file : _3dposCameras) {
        cv::Vec3d temp_t, temp_r;

        bool readOk = readCameraPose(file, temp_r, temp_t);
        if (!readOk) {
            std::cout << "Invalid camera pose file." << std::endl;
            return 0;
        }
        cam_rvecs.push_back(temp_r);
        cam_tvecs.push_back(temp_t);
    }

    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::aruco::ArucoDetector detector(dictionary, detectorParams);

    //get 3d position markers on the object
    ReadObjectCoordinates(_3dMarkerFile, &marker_info);

    std::vector<cv::Matx44d> objPose_frames;
    std::vector<cv::Mat> frame_imgs;

    //compute position object on each time frame
    for (int k = 0; k < imgs_FCam.size(); k++) {
        cv::Mat img_FCam, img_TCam, img_LCam, img_RCam;
        img_FCam = cv::imread(imgs_FCam.at(k));
        img_TCam = cv::imread(imgs_TCam.at(k));
        img_LCam = cv::imread(imgs_LCam.at(k));
        img_RCam = cv::imread(imgs_RCam.at(k));

        std::cout << "Analizing images: " << imgs_FCam.at(k) << 
            ", " << imgs_TCam.at(k) <<
            ", " << imgs_LCam.at(k) << 
            ", " << imgs_RCam.at(k) << std::endl;

        // Error Handling
        if (img_FCam.empty()
                || img_TCam.empty()
                || img_LCam.empty()
                || img_RCam.empty()) {
            std::cout << "Image files are empty." << std::endl;
            return 0;
        }

       /* cv::namedWindow("out", cv::WINDOW_NORMAL);
        cv::resizeWindow("out", 1280, 720);*/

        frame_imgs.push_back(img_FCam);
        frame_imgs.push_back(img_LCam);
        frame_imgs.push_back(img_RCam);
        frame_imgs.push_back(img_TCam);

        cv::Mat imageCopy;
        //see different view of the object
        for (int i = 0; i < frame_imgs.size(); i++){
            std::vector<int> ids;
            std::vector<std::vector<cv::Point2f> > corners, cornersNotRejected;
            frame_imgs[i].copyTo(imageCopy);

            detector.detectMarkers(frame_imgs.at(i), corners, ids);

            ObjectData temp_markerData;

            if(!ids.empty()) {          
                // Calculate 2d pose for each marker
                for (size_t  j = 0; j < ids.size(); j++) {
                    if(CheckObjectIDs(marker_info.GetIds(), ids.at(j))){
                        cornersNotRejected.push_back(corners[j]);
                        // Find 2d pose
                        float x_sum = corners[j][0].x + corners[j][1].x + corners[j][2].x + corners[j][3].x;
                        float y_sum = corners[j][0].y + corners[j][1].y + corners[j][2].y + corners[j][3].y;

                        cv::Point2f center_marker = cv::Point2f(x_sum*.25f, y_sum*.25f);

                        //save each single marker found on the struct data array to give to the function later
                        auto [idMarker, posMarker] = marker_info.FindByID(ids.at(j));
                        if(idMarker != 0){
                            temp_markerData.id_marker.push_back(idMarker);
                            temp_markerData.pos3D_marker.push_back(posMarker);
                        }
                        else
                            std::cout << "error" << std::endl;
                        temp_markerData.pos2D_marker.push_back(center_marker);
                    }
                }
            }

            //if (!temp_markerData.id_marker.empty()) {
            //    cv::aruco::drawDetectedMarkers(imageCopy, cornersNotRejected, temp_markerData.id_marker);
            //}

            //cv::imshow("out", imageCopy);
            //// Wait for any keystroke
            //cv::waitKey(0);

            if (!temp_markerData.id_marker.empty()) {
                //save camera params
                temp_markerData.rvec_cam = cam_rvecs.at(i);
                temp_markerData.tvec_cam = cam_tvecs.at(i);
                temp_markerData.cam_matrix = camMatrix.at(i);
                temp_markerData.dist_coeffs = distCoeffs.at(i);

                markers_data.push_back(temp_markerData);
            }    
        }

        //cv::Mat rvec_temp = cv::Mat::zeros(3,3, CV_64F);
        cv::Mat rvec = cv::Mat::zeros(3,1, CV_64F);
        //cv::Rodrigues(rvec_temp, rvec);

        rvec.at<double>(0,0) = 1;
        rvec.at<double>(1,0) = 1;
        rvec.at<double>(2,0) = 1;

        cv::Mat tvec = cv::Mat::zeros(3,1, CV_64F);
        ComputeObjectPose(&markers_data, rvec, tvec);

        cv::Affine3d res(rvec, tvec);
        objPose_frames.push_back(res.matrix);

        markers_data.clear();
        frame_imgs.clear();
    }

    bool saveOk = saveObjectPose(
        "Resources/objectPose.yml", objPose_frames, imgs_FCam.size());

    if (!saveOk)
       std::cout << "Error saving file" << std::endl;

    return 1;
}
