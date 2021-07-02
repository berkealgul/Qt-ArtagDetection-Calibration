#ifndef VIDEO_CAPTURER_H
#define VIDEO_CAPTURER_H

#include "videocapturer.h"

VideoCapturer::VideoCapturer(int id, QMutex *lock,QMutex *lock2, renderer3d *ren)
{
    camId = id;
    data_lock = lock;
    data_lock2 = lock2;
    calibrating = false;
    aruco = false;

    aruco_params = DetectorParameters::create();
    aruco_dictionary = getPredefinedDictionary(DICT_5X5_250);

    calibrator = new Calibrator();
    calibrator->boardSize = Size(5,5);
    calibrator->grid_width = 5;
    calibrator->squareSize = 5;
    calibrator->imgSize = Size(640,480);

    renderer = ren;
}

void VideoCapturer::run()
{
    cap.open(camId);

    if(!cap.isOpened())
    {
        return;
    }

    active = true;

    Mat tmp_frame;

    while(active)
    {
        cap >> tmp_frame;
        cvtColor(tmp_frame, tmp_frame, COLOR_BGR2RGB);

        if(calibrating)
        {
            calibrator->processImg(tmp_frame);

            if(calibrator->imgs == 100)
            {
                calibrating = false;
            }


            cv::putText(tmp_frame,
                        "Images taken for calbration: "+to_string(calibrator->imgs),
                        cv::Point(5,30), // Coordinates
                        cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
                        .5, // Scale. 2.0 = 2x bigger
                        cv::Scalar(255,255,255), // BGR Color
                        1, // Line Thickness (Optional)
                        cv:: LINE_AA); // Anti-alias (Optional, see version note)

        }
        else if(calibrator->imgs > 1)
        {
            calibrator->calibrate();
            calibrating = false;
            params.clear();
            params.push_back(calibrator->camMatrix.at<double>(0,0));
            params.push_back(calibrator->camMatrix.at<double>(1,1));
            params.push_back(calibrator->camMatrix.at<double>(0,2));
            params.push_back(calibrator->camMatrix.at<double>(1,2));
            params.push_back(calibrator->distCoeffs.at<double>(0,0));
            params.push_back(calibrator->distCoeffs.at<double>(1,0));
            params.push_back(calibrator->distCoeffs.at<double>(2,0));
            params.push_back(calibrator->distCoeffs.at<double>(3,0));
            params.push_back(calibrator->distCoeffs.at<double>(4,0));
            params.push_back(calibrator->distCoeffs.at<double>(5,0));
            params.push_back(calibrator->distCoeffs.at<double>(6,0));
            params.push_back(calibrator->distCoeffs.at<double>(7,0));
            emit updateUiParams();
        }


        //ARUCOOO
        if(aruco)
        {
            std::vector<std::vector<Point2f>> corners;
            std::vector<int> ids;
            std::vector<Vec3d> rvecs, tvecs;

            cout << calibrator->distCoeffs;
            cout << calibrator->camMatrix;

            detectMarkers(tmp_frame, aruco_dictionary, corners, ids ,aruco_params);
            estimatePoseSingleMarkers(corners, 0.2, calibrator->camMatrix, calibrator->distCoeffs, rvecs, tvecs);
            drawDetectedMarkers(tmp_frame, corners);

            for (int i = 0, n = ids.size(); i < n; i++)
            {
                    Vec3d r = rvecs[i];
                    Vec3d t = tvecs[i];

                    drawAxis(tmp_frame, calibrator->camMatrix, calibrator->distCoeffs, r, t, 0.1);
            }

            Rvecs = rvecs;
            Tvecs = tvecs;
        }


        data_lock->lock();
        frame = tmp_frame;
        data_lock->unlock();

        emit frameCaptured(&frame);

        if(calibrator->calibrated)
        {
            data_lock2->lock();
            undistort(frame,uframe,calibrator->camMatrix,calibrator->distCoeffs);
            data_lock2->unlock();
            emit frameCaptured2(&uframe, calibrator->imgs);
        }

        //msleep(10);
    }

    active = false;

    cap.release();
}

#endif
