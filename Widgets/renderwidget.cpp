#include "renderwidget.h"
#include "ui_render.h"
#include "QFileDialog"
#include "constants.h"
#include <QDebug>
#include "mainwindow.h"
#include "entitymanager.h"
#include "componentdata.h"

RenderWidget::RenderWidget(MainWindow *mainWindow, QWidget* parent)
    : QWidget(parent), ui(new Ui::Render), mMainWindow(mainWindow)
{
    ui->setupUi(this);
}

RenderWidget::~RenderWidget()
{

}

void RenderWidget::on_button_ChangeMesh_clicked()
{
    if(mMainWindow->currentEntitySelected)
    {
        auto fileName = QFileDialog::getOpenFileName(this, tr("Choose"), QString::fromStdString(gsl::assetFilePath), tr("Mesh files (*.obj *.txt)"));
        auto splits = fileName.split('/');
        auto last = splits[splits.size()-1];
        splits = last.split('.');
        mMainWindow->getEntityManager()->setMesh(mMainWindow->currentEntitySelected->entityId, splits[0].toStdString());
    }

}
