#ifndef VIDEOCAPTURER_H
#define VIDEOCAPTURER_H

#include <QMessageBox>
#include <QThread>
#include <QImage>
#include <QMutex>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include "calibrator.h"
#include "aruco.hpp"
#include "renderer3d.h"

using namespace cv;
using namespace aruco;

class VideoCapturer : public QThread
{
    Q_OBJECT
public:
    VideoCapturer(QObject *parent = nullptr);
    VideoCapturer(int id, QMutex *data_lock,QMutex *data_lock2, renderer3d *red);

    QPixmap pixmap()
    {
        return mPixmap;
    }

signals:
    void frameCaptured(cv::Mat *frame);
    void frameCaptured2(cv::Mat *frame, float);
    void updateUiParams();

public:
    void run();

    bool active;
    bool calibrating;
    bool aruco;

    int camId;
    vector<double> params;
    QPixmap mPixmap;
    QMutex *data_lock,*data_lock2;
    Mat frame, uframe;
    VideoCapture cap;
    Calibrator *calibrator;
    renderer3d *renderer;

    std::vector<Vec3d> Rvecs, Tvecs;

    Ptr<DetectorParameters> aruco_params;
    Ptr<Dictionary> aruco_dictionary;

};

#endif // VIDEOCAPTURER_H
