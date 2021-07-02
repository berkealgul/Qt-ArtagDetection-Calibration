#include "calibrator.h"

static double computeReprojectionErrors
(
        const vector<vector<Point3f> >& objectPoints,
        const vector<vector<Point2f> >& imagePoints,
        const vector<Mat>& rvecs, const vector<Mat>& tvecs,
        const Mat& cameraMatrix, const Mat& distCoeffs,
        vector<float>& perViewErrors )
{
    vector<Point2f> imagePoints2;
    int i, totalPoints = 0;
    double totalErr = 0, err;
    perViewErrors.resize(objectPoints.size());

    for( i = 0; i < (int)objectPoints.size(); i++ )
    {
        projectPoints(Mat(objectPoints[i]), rvecs[i], tvecs[i],
                      cameraMatrix, distCoeffs, imagePoints2);
        err = norm(Mat(imagePoints[i]), Mat(imagePoints2), NORM_L2);
        int n = (int)objectPoints[i].size();
        perViewErrors[i] = (float)std::sqrt(err*err/n);
        totalErr += err*err;
        totalPoints += n;
    }

    return std::sqrt(totalErr/totalPoints);
}

static void saveCameraParams( const string& filename, const Mat& cameraMatrix, const Mat& distCoeffs)
{
    FileStorage fs( filename, FileStorage::WRITE );

    time_t tt;
    time( &tt );
    struct tm *t2 = localtime( &tt );
    char buf[1024];
    strftime( buf, sizeof(buf)-1, "%c", t2 );

    fs << "calibration_time" << buf;
}

static bool readStringList( const string& filename, vector<string>& l )
{
    l.resize(0);
    FileStorage fs(filename, FileStorage::READ);
    if( !fs.isOpened() )
        return false;
    size_t dir_pos = filename.rfind('/');
    if (dir_pos == string::npos)
        dir_pos = filename.rfind('\\');
    FileNode n = fs.getFirstTopLevelNode();
    if( n.type() != FileNode::SEQ )
        return false;
    FileNodeIterator it = n.begin(), it_end = n.end();
    for( ; it != it_end; ++it )
    {
        string fname = (string)*it;
        if (dir_pos != string::npos)
        {
            string fpath = samples::findFile(filename.substr(0, dir_pos + 1) + fname, false);
            if (fpath.empty())
            {
                fpath = samples::findFile(fname);
            }
            fname = fpath;
        }
        else
        {
            fname = samples::findFile(fname);
        }
        l.push_back(fname);
    }
    return true;
}

/*
static bool runAndSave(const string& outputFilename,
                const vector<vector<Point2f> >& imagePoints,
                Size imageSize, Size boardSize, float squareSize,
                float grid_width, bool release_object,
                float aspectRatio, int flags, Mat& cameraMatrix,
                Mat& distCoeffs, bool writeExtrinsics, bool writePoints, bool writeGrid )
{
    vector<Mat> rvecs, tvecs;
    vector<float> reprojErrs;
    double totalAvgErr = 0;
    vector<Point3f> newObjPoints;

    bool ok = runCalibration(imagePoints, imageSize, boardSize, squareSize,
                   aspectRatio, grid_width, release_object, flags, cameraMatrix, distCoeffs,
                   rvecs, tvecs, reprojErrs, newObjPoints, totalAvgErr);
    printf("%s. avg reprojection error = %.7f\n",
           ok ? "Calibration succeeded" : "Calibration failed",
           totalAvgErr);

    if( ok )
        saveCameraParams( outputFilename, imageSize,
                         boardSize, squareSize, aspectRatio,
                         flags, cameraMatrix, distCoeffs,
                         writeExtrinsics ? rvecs : vector<Mat>(),
                         writeExtrinsics ? tvecs : vector<Mat>(),
                         writeExtrinsics ? reprojErrs : vector<float>(),
                         writePoints ? imagePoints : vector<vector<Point2f> >(),
                         writeGrid ? newObjPoints : vector<Point3f>(),
                         totalAvgErr );
    return ok;
}*/

static bool runCalibration(vector<vector<Point2f>> imagePoints,
                    Size imageSize, Size boardSize,
                    float squareSize,
                    float grid_width,
                    Mat& cameraMatrix, Mat& distCoeffs,
                    vector<Mat>& rvecs, vector<Mat>& tvecs,
                    vector<float>& reprojErrs,
                    vector<Point3f>& newObjPoints,
                    double& totalAvgErr)
{
    cameraMatrix = Mat::eye(3, 3, CV_64F);

    //if( flags & CALIB_FIX_ASPECT_RATIO )
        //cameraMatrix.at<double>(0,0) = aspectRatio;

    distCoeffs = Mat::zeros(8, 1, CV_64F);

    vector<vector<Point3f> > objectPoints(1);

    objectPoints[0].clear();

    for( int i = 0; i < boardSize.height; ++i )
        for( int j = 0; j < boardSize.width; ++j )
        {
            objectPoints[0].push_back(Point3f(float(j*squareSize), float(i*squareSize), 0));
        }

    objectPoints[0][boardSize.width - 1].x = objectPoints[0][0].x + grid_width;
    newObjPoints = objectPoints[0];

    objectPoints.resize(imagePoints.size(),objectPoints[0]);

    double rms;
    int iFixedPoint = -1;

    rms = calibrateCameraRO(objectPoints, imagePoints, imageSize, iFixedPoint,
                            cameraMatrix, distCoeffs, rvecs, tvecs, newObjPoints,
                            CALIB_FIX_K3 | CALIB_USE_LU);
    //printf("RMS error reported by calibrateCamera: %g\n", rms);



    bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);

    objectPoints.clear();
    objectPoints.resize(imagePoints.size(), newObjPoints);
    totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
                rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);

    return ok;
}


Calibrator::Calibrator()
{
    imgs = 0;
    calibrated = false;
    criteria = TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 30, 0.0001 );
}

void Calibrator::calibrate()
{

 grid_width = squareSize * (boardSize.width - 1);
 runCalibration(imagePoints, imgSize, boardSize, squareSize, grid_width, camMatrix, distCoeffs, rvecs, tvecs, reprojErrs, newObjPoints, totalAvgErr);
 imgs = 0;
 imagePoints.clear();
 reprojErrs.clear();
 newObjPoints.clear();
 calibrated = true;
}

void Calibrator::processImg(Mat img)
{
    vector<Point2f> corners;
    imgSize = img.size();

    bool found = findChessboardCorners(img, boardSize, corners, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);
    drawChessboardCorners(img, boardSize, corners, found);

    if(counter == 10)
    {
        counter = 0;

        if(found)
        {
            Mat imgg;
            cvtColor(img, imgg, COLOR_BGR2GRAY);
            cornerSubPix(imgg, corners, Size(6,6),Size(-1,-1),criteria);

            if(corners.size()==boardSize.area())
            {
                imagePoints.push_back(corners);
                imgs++;
            }
        }
    }
    else
        counter++;
}


void Calibrator::setParams(vector<double> params)
{
    camMatrix = Mat::eye(3, 3, CV_64F);
    distCoeffs = Mat::zeros(8, 1, CV_64F);

    camMatrix.at<double>(0,0) = params[0];
    camMatrix.at<double>(1,1) = params[1];
    camMatrix.at<double>(0,2) = params[2];
    camMatrix.at<double>(1,2) = params[3];
    distCoeffs.at<double>(0,0) = params[4];
    distCoeffs.at<double>(1,0) = params[5];
    distCoeffs.at<double>(2,0) = params[6];
    distCoeffs.at<double>(3,0) = params[7];
    distCoeffs.at<double>(4,0) = params[8];
    distCoeffs.at<double>(5,0) = params[9];
    distCoeffs.at<double>(6,0) = params[10];
    distCoeffs.at<double>(7,0) = params[11];
}
