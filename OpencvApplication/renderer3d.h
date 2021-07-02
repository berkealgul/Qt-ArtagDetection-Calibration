#ifndef RENDERER3D_H
#define RENDERER3D_H


#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QAspectEngine>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QForwardRenderer>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


class renderer3d
{

public:
    renderer3d(QObject *parent = nullptr);

    Qt3DExtras::Qt3DWindow *view;
    Qt3DCore::QEntity *root;
    Qt3DRender::QCamera *camera;

    void renderScene();
    void deleteChildrenRecursively(Qt3DCore::QNodeVector& vector);
    void delete_entity(Qt3DCore::QNode *node);
    void renderTags(vector<Vec3d> rvecs, vector<Vec3d>tvecs);
    void drawLine(const QVector3D& start, const QVector3D& end, const QColor& color, Qt3DCore::QEntity *_rootEntity);
};

#endif // RENDERER3D_H
