#include "meshwidget.h"
#include "ui_mesh.h"
#include "QFileDialog"
#include "constants.h"
#include <QMenu>
#include "mainwindow.h"
#include "entitymanager.h"

MeshWidget::MeshWidget(MainWindow *mainWindow, QWidget* parent)
    : ComponentWidget(mainWindow, parent), ui(new Ui::Mesh)
{
    ui->setupUi(this);
}

void MeshWidget::update(const std::string &name)
{
    if(name.size())
    {
        ui->label_Name->setText(QString::fromStdString(name));
    }
    else
    {
        ui->label_Name->setText("None");
    }


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
            if(auto render = getRenderComponent(mMainWindow->currentEntitySelected->entityId))
            {
                render->isVisible = true;
                ui->checkBox_Visible->setCheckState(Qt::CheckState::Checked);
            }
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
