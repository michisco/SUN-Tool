#ifndef DEVICEMARKERS_H
#define DEVICEMARKERS_H

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

class DeviceMarkers{
    private:
        std::vector<int> id_markers;
        std::vector<cv::Point3f> marker3d_center;
        std::vector<std::vector<cv::Point3f>> marker3d_corner;

    public:
        DeviceMarkers(){}

        void Set(int id, cv::Point3f center, std::vector<cv::Point3f> corners){
            id_markers.push_back(id);
            marker3d_center.push_back(center);
            marker3d_corner.push_back(corners);
        }

        std::tuple<int, cv::Point3f, std::vector<cv::Point3f>> Get(int i){
            return {id_markers.at(i), marker3d_center.at(i), marker3d_corner.at(i) };
        }

        std::vector<int> GetIds(){
            return id_markers;
        }

        std::vector<cv::Point3f> Get3DPositionsCenter() {
            return marker3d_center;
        }

        std::vector<std::vector<cv::Point3f>> Get3DPositions(){
            return marker3d_corner;
        }

        std::tuple<int, cv::Point3f, std::vector<cv::Point3f>> FindByID(int id){
            for(int i = 0; i < id_markers.size(); i++){
                if(id_markers.at(i) == id){
                    return {id_markers.at(i), marker3d_center.at(i), marker3d_corner.at(i) };
                }
            }
            std::vector<cv::Point3f> res;
            res.push_back(cv::Point3f(0, 0, 0));
            return {0, cv::Point3f(0, 0, 0), res};
        }
};

#endif // DEVICEMARKERS_H
