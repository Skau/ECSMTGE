#include "meshwidget.h"
#include "ui_mesh.h"
#include "QFileDialog"
#include "constants.h"
#include <QMenu>
#include "mainwindow.h"
#include "entitymanager.h"
#include "resourcemanager.h"
#include "texture.h"

MeshWidget::MeshWidget(MainWindow *mainWindow, QWidget* parent)
    : ComponentWidget(mainWindow, parent), ui(new Ui::Mesh)
{
    ui->setupUi(this);

    for(auto& name : ResourceManager::instance()->getAllMeshNames())
    {
        ui->comboBox_Meshes->addItem(QString::fromStdString(name));
    }

    for(auto& name : ResourceManager::instance()->getAllShaderNames())
    {
        ui->comboBox_Shaders->addItem(QString::fromStdString(name));
    }

    for(auto& name : ResourceManager::instance()->getAllTextureNames())
    {
        ui->comboBox_Textures->addItem(QString::fromStdString(name));
    }
}

void MeshWidget::updateData()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto mesh = getRenderComponent(entity->entityId))
        {
            auto name = entity->name;
            isUpdating = true;
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

            isUpdating = false;
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
            if(auto render = getRenderComponent(mMainWindow->currentEntitySelected->entityId))
            {
                bool found = false;
                for(int i = 0; i < ui->comboBox_Meshes->count(); ++i)
                {
                    if(name == ui->comboBox_Meshes->itemText(0))
                    {
                        render->meshData = *ResourceManager::instance()->getMesh(name.toStdString());
                        found = true;
                        break;
                    }
                }

                if(!found)
                {
                    render->meshData = *ResourceManager::instance()->addMesh(name.toStdString(), last.toStdString());
                }

                if(render->meshData.mVAO)
                {
                    render->isVisible = true;
                    ui->checkBox_Visible->setCheckState(Qt::CheckState::Checked);
                    ui->label_Name->setText(name);
                    ui->comboBox_Meshes->addItem(name);
                }
            }
        }
    }
}

void MeshWidget::on_checkBox_Visible_toggled(bool checked)
{
    if(isUpdating) return;

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
            widgetRemoved(this);
        }
    }
}

MeshComponent *MeshWidget::getRenderComponent(unsigned int entity)
{
    return mMainWindow->getEntityManager()->getComponent<MeshComponent>(entity);
}

void MeshWidget::on_pushButton_ChangeMeshDropdown_clicked()
{
    if(!mMainWindow->currentEntitySelected) return;

    auto name = ui->comboBox_Meshes->currentText();
    if(!name.length()) return;

    if(auto render = getRenderComponent(mMainWindow->currentEntitySelected->entityId))
    {
        render->meshData = *ResourceManager::instance()->getMesh(name.toStdString());
        render->isVisible = true;
        ui->checkBox_Visible->setCheckState(Qt::CheckState::Checked);
        ui->label_Name->setText(name);
    }
}

void MeshWidget::on_pushButton_ChangeShaderDropdown_clicked()
{
    if(!mMainWindow->currentEntitySelected) return;

    auto name = ui->comboBox_Shaders->currentText();
    if(!name.length()) return;

    if(auto render = getRenderComponent(mMainWindow->currentEntitySelected->entityId))
    {
        render->meshData.mMaterial.mShader = ResourceManager::instance()->getShader(name.toStdString());
    }
}

void MeshWidget::on_pushButton_ChangeTextureDropdown_clicked()
{
    if(!mMainWindow->currentEntitySelected) return;

    auto name = ui->comboBox_Textures->currentText();
    if(!name.length()) return;

    if(auto render = getRenderComponent(mMainWindow->currentEntitySelected->entityId))
    {
        render->meshData.mMaterial.mTexture = ResourceManager::instance()->getTexture(name.toStdString());
    }
}
