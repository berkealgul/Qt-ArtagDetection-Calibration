#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), capturer(nullptr)
{
    ui->setupUi(this);

    data_lock = new QMutex();
    data_lock2 = new QMutex();

    connect(ui->startButton, &QPushButton::released, this, &MainWindow::startCamera);
    connect(ui->calibrateButton, &QPushButton::released, this, &MainWindow::startCalibration);
    connect(ui->pushButton, &QPushButton::released, this, &MainWindow::startAruco);

    renderer = new renderer3d;
    renderer->view->setParent(nullptr);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateFrame(Mat *mat)
{
    data_lock->lock();
    currentFrame = *mat;
    data_lock->unlock();

    QImage frame(
        currentFrame.data,
        currentFrame.cols,
        currentFrame.rows,
        currentFrame.step,
        QImage::Format_RGB888);

    img = QPixmap::fromImage(frame);

    ui->label->setPixmap(img);

    if(capturer->aruco)
    {
        renderer->renderTags(capturer->Rvecs, capturer->Tvecs);
    }
}

void MainWindow::updateFrame2(Mat *mat, int count)
{
    data_lock2->lock();
    QImage frame(
        mat->data,
        mat->cols,
        mat->rows,
        mat->step,
        QImage::Format_RGB888);

    img = QPixmap::fromImage(frame);
    data_lock2->unlock();

    ui->label_2->setPixmap(img);
}


void MainWindow::startCamera()
{
    if(capturer != nullptr)
    {
        if(capturer->active)
        {
            capturer->active = false;
            ui->startButton->setText("Start Camera");
        }
        else
        {
            try {
                capturer->camId = stoi(ui->textEdit->toPlainText().toStdString());

            }  catch (exception e) {
                QMessageBox messageBox;
                messageBox.critical(0,"Error","Invalid input");
                messageBox.setFixedSize(500,200);
                return;
            }

            connect(capturer, &VideoCapturer::frameCaptured, this, &MainWindow::updateFrame);
            connect(capturer, &VideoCapturer::frameCaptured2, this, &MainWindow::updateFrame2);
            connect(capturer, &VideoCapturer::updateUiParams, this, &MainWindow::updateUiParams);
            capturer->setParent(this);
            capturer->start();

             _sleep(500);

            if(capturer->active)
            {
                ui->startButton->setText("Stop Camera");
            }
            else
            {
                QMessageBox messageBox;
                messageBox.critical(0,"Error","Camera coulnt detected");
                messageBox.setFixedSize(500,200);
            }
        }
    }
    else
    {
        try {
            int camID = stoi(ui->textEdit->toPlainText().toStdString());
            capturer = new VideoCapturer(camID, data_lock, data_lock2, renderer);
            connect(capturer, &VideoCapturer::frameCaptured, this, &MainWindow::updateFrame);
            connect(capturer, &VideoCapturer::frameCaptured2, this, &MainWindow::updateFrame2);
            connect(capturer, &VideoCapturer::updateUiParams, this, &MainWindow::updateUiParams);
            capturer->setParent(this);
            capturer->start();

        }  catch (exception e) {
            QMessageBox messageBox;
            messageBox.critical(0,"Error","Invalid input");
            messageBox.setFixedSize(500,200);
        }

        _sleep(500);
        if(capturer->active)
        {
            ui->startButton->setText("Stop Camera");
        }
        else
        {
            QMessageBox messageBox;
            messageBox.critical(0,"Error","Camera coulnt detected");
            messageBox.setFixedSize(500,200);
        }


    }
}

void MainWindow::startCalibration()
{
    if(capturer == nullptr)
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Camera didnt detected!");
        messageBox.setFixedSize(500,200);
    }
    else
    {
        if(capturer->active == true)
        {
            if(capturer->calibrating == false)
            {
                try
                {
                    int bw = stoi(ui->textEdit_2->toPlainText().toStdString());
                    int bh = stoi(ui->textEdit_3->toPlainText().toStdString());
                    float ss = (float)stoi(ui->textEdit_4->toPlainText().toStdString())/1000;
                    capturer->calibrator->boardSize = Size(bw, bh);
                    capturer->calibrator->squareSize = ss;
                    capturer->calibrating = true;
                    capturer->aruco = false;
                    capturer->calibrator->calibrated = false;
                    ui->calibrateButton->setText("End Calibration");
                }
                catch(exception e)
                {
                    QMessageBox messageBox;
                    messageBox.critical(0,"Error","Something wrong with parameters");
                    messageBox.setFixedSize(500,200);
                }
            }
            else
            {
                capturer->calibrating = false;
                ui->calibrateButton->setText("Start Calibration");
            }

        }
        else
        {
            QMessageBox messageBox;
            messageBox.critical(0,"Error","Camera didnt detected!");
            messageBox.setFixedSize(500,200);
        }
    }
}

void MainWindow::startAruco()
{
    if(capturer == nullptr)
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Camera didnt detected!");
        messageBox.setFixedSize(500,200);
    }
    else
    {
        if(capturer->aruco == true)
        {
            capturer->aruco = false;
        }
        else
        {
            try {
                vector<double> params;
                params.push_back(stod(ui->fx->toPlainText().toStdString()));
                params.push_back(stod(ui->fy->toPlainText().toStdString()));
                params.push_back(stod(ui->cx->toPlainText().toStdString()));
                params.push_back(stod(ui->cy->toPlainText().toStdString()));
                params.push_back(stod(ui->d1->toPlainText().toStdString()));
                params.push_back(stod(ui->d2->toPlainText().toStdString()));
                params.push_back(stod(ui->d3->toPlainText().toStdString()));
                params.push_back(stod(ui->d4->toPlainText().toStdString()));
                params.push_back(stod(ui->d5->toPlainText().toStdString()));
                params.push_back(stod(ui->d6->toPlainText().toStdString()));
                params.push_back(stod(ui->d7->toPlainText().toStdString()));
                params.push_back(stod(ui->d8->toPlainText().toStdString()));
                capturer->calibrator->setParams(params);
                capturer->aruco = true;
                capturer->calibrating = false;
            }  catch (exception e) {
                QMessageBox messageBox;
                messageBox.critical(0,"Error","Invalid parameters");
                messageBox.setFixedSize(500,200);
            }

        }
    }
}

void MainWindow::updateUiParams()
{
    ui->fx->setText(QString::number(capturer->params[0]));
    ui->fy->setText(QString::number(capturer->params[1]));
    ui->cx->setText(QString::number(capturer->params[2]));
    ui->cy->setText(QString::number(capturer->params[3]));
    ui->d1->setText(QString::number(capturer->params[4]));
    ui->d2->setText(QString::number(capturer->params[5]));
    ui->d3->setText(QString::number(capturer->params[6]));
    ui->d4->setText(QString::number(capturer->params[7]));
    ui->d5->setText(QString::number(capturer->params[8]));
    ui->d6->setText(QString::number(capturer->params[9]));
    ui->d7->setText(QString::number(capturer->params[10]));
    ui->d8->setText(QString::number(capturer->params[11]));
}
