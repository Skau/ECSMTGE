#include "meshwidget.h"
#include "ui_mesh.h"
#include "QFileDialog"
#include "constants.h"
#include <QDebug>
#include <QMenu>
#include "mainwindow.h"
#include "entitymanager.h"
#include "componentdata.h"

MeshWidget::MeshWidget(MainWindow *mainWindow, QWidget* parent)
    : QWidget(parent), ui(new Ui::Mesh), mMainWindow(mainWindow)
{
    ui->setupUi(this);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &MeshWidget::ProvideContextMenu);
}

MeshWidget::~MeshWidget()
{

}

void MeshWidget::update(const std::string &name)
{
    ui->label_Name->setText(QString::fromStdString(name));

    if(auto render = getRenderComponent(mMainWindow->currentEntitySelected->entityId))
    {
        if(render->isVisible)
        {
             ui->checkBox_Visible->setCheckState(Qt::CheckState::Checked);
        }
        else
        {
            ui->checkBox_Visible->setCheckState(Qt::CheckState::Unchecked);
        }
    }
}

void MeshWidget::on_button_ChangeMesh_clicked()
{
    if(mMainWindow->currentEntitySelected)
    {
        auto fileName = QFileDialog::getOpenFileName(this, tr("Choose"), QString::fromStdString(gsl::assetFilePath), tr("Mesh files (*.obj *.txt)"));
        auto splits = fileName.split('/');
        auto last = splits[splits.size()-1];
        splits = last.split('.');
        auto name = splits[0];
        if(name.length())
        {
            mMainWindow->getEntityManager()->setMesh(mMainWindow->currentEntitySelected->entityId, name.toStdString());
            update(name.toStdString());
        }
    }
}

void MeshWidget::on_checkBox_Visible_toggled(bool checked)
{
    if(mMainWindow->currentEntitySelected)
    {
        if(auto render = getRenderComponent(mMainWindow->currentEntitySelected->entityId))
        {
            render->isVisible = checked;
        }
    }
}

void MeshWidget::ProvideContextMenu(const QPoint &point)
{
    QMenu subMenu;
    subMenu.addAction("Remove", this, &MeshWidget::Remove);

    QPoint globalPos = mapToGlobal(point);

    subMenu.exec(globalPos);
}

void MeshWidget::Remove()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(mMainWindow->getEntityManager()->removeComponent<MeshComponent>(entity->entityId))
        {
            widgetRemoved();
        }
    }
}

MeshComponent *MeshWidget::getRenderComponent(unsigned int entity)
{
    return mMainWindow->getEntityManager()->getComponent<MeshComponent>(entity);
}
