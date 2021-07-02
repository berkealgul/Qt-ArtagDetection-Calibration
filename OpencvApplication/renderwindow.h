#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H



#include <Qt3DRender/QRenderAspect>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QTorusMesh>


namespace Ui {
class renderWindow;
}

class renderWindow : public QDialog
{
    Q_OBJECT

public:
    explicit renderWindow(QWidget *parent = nullptr);
    ~renderWindow();

private:
    Ui::renderWindow *ui;
};

#endif // RENDERWINDOW_H
