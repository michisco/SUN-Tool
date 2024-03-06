#ifndef OBJECTMARKERS_H
#define OBJECTMARKERS_H

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

class ObjectMarkers{
    private:
        std::vector<int> id_markers;
        std::vector<cv::Point3f> marker3d_position;
    public:
        ObjectMarkers(){}

        void Set(int id, cv::Point3f pos){
            id_markers.push_back(id);
            marker3d_position.push_back(pos);
        }

        std::tuple<int, cv::Point3f> Get(int i){
            return {id_markers.at(i), marker3d_position.at(i)};
        }

        std::vector<int> GetIds(){
            return id_markers;
        }

        std::vector<cv::Point3f> Get3DPositions(){
            return marker3d_position;
        }

        std::tuple<int, cv::Point3f> FindByID(int id){
            for(int i = 0; i < id_markers.size(); i++){
                if(id_markers.at(i) == id){
                    return {id_markers.at(i), marker3d_position.at(i)};
                }
            }
            return {0, cv::Point3f(0, 0, 0)};
        }
};

#endif // OBJECTMARKERS_H
