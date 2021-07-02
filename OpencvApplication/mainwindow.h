#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMutex>
#include <QKeyEvent>
#include <QDebug>
#include "opencv2/opencv.hpp"
#include "videocapturer.h"
#include "calibrator.h"
#include "renderer3d.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
using namespace cv;
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QMutex *data_lock, *data_lock2;
    QPixmap img, img2;
    Mat currentFrame;

    QThread* thread;

    VideoCapturer *capturer;
    renderer3d *renderer;

    void updateFrame(Mat *mat);
    void updateFrame2(Mat *mat, int count);
    void startCamera();
    void startCalibration();
    void startAruco();
    void updateUiParams();

};
#endif // MAINWINDOW_H
