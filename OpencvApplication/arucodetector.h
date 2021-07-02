#ifndef ARUCODETECTOR_H
#define ARUCODETECTOR_H

#include <opencv2/opencv.hpp>
#include "aruco.hpp"
#include <vector>

using namespace cv;
using namespace aruco;

class ArucoDetector
{
public:
    ArucoDetector();

    Ptr<DetectorParameters> aruco_params;
    Ptr<Dictionary> aruco_dictionary;
    float marker_size;
    Mat dist;
    Mat mtx;

    void runFrame(Mat frame);
    void setParams(Ptr<DetectorParameters>, Ptr<Dictionary>, float, Mat, Mat);
};

#endif // ARUCODETECTOR_H
