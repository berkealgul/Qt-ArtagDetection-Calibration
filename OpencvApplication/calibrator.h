#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <QMessageBox>
#include <QThread>
#include <QImage>
#include <QMutex>
#include <QPixmap>
#include <QDebug>

using namespace std;
using namespace cv;

class Calibrator
{

public:
    Mat camMatrix, distCoeffs;

    vector<vector<Point2f> > imagePoints;
    vector<Mat> tvecs, rvecs;
    vector<float> reprojErrs;
    vector<Point3f> newObjPoints;

    Size imgSize, boardSize;

    int imgs;
    int counter = 0;
    float squareSize, grid_width;
    double totalAvgErr;

    TermCriteria criteria;

    bool calibrated;

    Calibrator();

    void processImg(Mat);
    void calibrate();
    void setParams(vector<double> params);
};

#endif // CALIBRATOR_H
