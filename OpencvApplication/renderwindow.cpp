#include "renderwindow.h"
#include "ui_renderwindow.h"

renderWindow::renderWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::renderWindow)
{
    ui->setupUi(this);


}

renderWindow::~renderWindow()
{
    delete ui;
}
