#include "meshwidget.h"
#include "ui_mesh.h"
#include "QFileDialog"
#include "constants.h"
#include <QMenu>
#include "mainwindow.h"
#include "entitymanager.h"
#include "resourcemanager.h"
#include "texture.h"
#include <QLabel>
#include <QSpinBox>

MeshWidget::MeshWidget(MainWindow *mainWindow, QWidget* parent)
    : ComponentWidget(mainWindow, parent), ui(new Ui::Mesh)
{
    ui->setupUi(this);

    ui->comboBox_Meshes->addItem("None");
    for(auto& name : ResourceManager::instance()->getAllMeshNames())
    {
        ui->comboBox_Meshes->addItem(QString::fromStdString(name));
    }

    ui->comboBox_Shaders->addItem("None");
    for(auto& name : ResourceManager::instance()->getAllShaderNames())
    {
        ui->comboBox_Shaders->addItem(QString::fromStdString(name));
    }

    ui->comboBox_Textures->addItem("None");
    for(auto& name : ResourceManager::instance()->getAllTextureNames())
    {
        ui->comboBox_Textures->addItem(QString::fromStdString(name));
    }

    isUpdating = true;
    if(auto comp = getRenderComponent())
    {
        auto name = comp->meshData.mName;
        if(name.size())
        {
            ui->label_Name->setText(QString::fromStdString(name));
            ui->comboBox_Meshes->setCurrentText(QString::fromStdString(comp->meshData.mName));
        }
        else
        {
            ui->label_Name->setText("None");
        }
        if(auto shader = comp->mMaterial.mShader)
        {
            ui->comboBox_Shaders->setCurrentText(QString::fromStdString(shader->mName));

            updateShaderParameters(comp->mMaterial);
        }

        ui->checkBox_Visible->setCheckState(comp->isVisible ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    }
    isUpdating = false;
}

void MeshWidget::updateData()
{
//    auto entity = mMainWindow->currentEntitySelected;
//    if(entity)
//    {
//        if(auto mesh = getRenderComponent(entity->entityId))
//        {
//            isUpdating = true;
//            isUpdating = false;
//        }
//    }
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
            if(auto render = getRenderComponent())
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

                if(render->meshData.mVAOs[0])
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

    if(auto render = getRenderComponent())
    {
        render->isVisible = checked;
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

MeshComponent *MeshWidget::getRenderComponent()
{
    if(auto entity = mMainWindow->currentEntitySelected)
    {
        if(auto comp = mMainWindow->getEntityManager()->getComponent<MeshComponent>(entity->entityId))
        {
            return comp;
        }
    }

    return nullptr;
}

void MeshWidget::updateShaderParameters(Material& material)
{
    if(material.mParameters.size())
    {
        if(ui->widget_Parameters->children().size())
        {
            for(auto& child : ui->widget_Parameters->children())
            {
                delete child;
            }
        }

        if(ui->widget_Parameters->isHidden())
            ui->widget_Parameters->show();

        QVBoxLayout* layout = new QVBoxLayout(ui->widget_Parameters);
        ui->widget_Parameters->setLayout(layout);
        layout->setMargin(0);
        QWidget* widget = new QWidget();
        layout->addWidget(widget);

        float minimumHeight = 0.f;
        for(auto& param : material.mParameters)
        {
            QHBoxLayout* hLayout = new QHBoxLayout();
            QLabel* label = new QLabel(QString::fromStdString(param.first), widget);
            hLayout->addWidget(label);
            if (std::holds_alternative<int>(param.second))
            {
                QSpinBox* spinBox = new QSpinBox(widget);
                spinBox->setValue(std::get<int>(param.second));
                connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                [=](int i)
                {
                    if(auto render = getRenderComponent())
                    {
                        if(render->mMaterial.mParameters.size())
                        {
                            if(render->mMaterial.mParameters.find(param.first) != render->mMaterial.mParameters.end())
                            {
                                render->mMaterial.mParameters[param.first] = i;
                            }
                        }
                    }
                });
                hLayout->addWidget(spinBox);
                minimumHeight += 25.33f;
            }
            else if (std::holds_alternative<float>(param.second))
            {
                QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox(widget);
                doubleSpinBox->setSingleStep(0.1);
                doubleSpinBox->setValue(static_cast<double>(std::get<float>(param.second)));
                connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                [=](double d)
                {
                    if(auto render = getRenderComponent())
                    {
                        if(render->mMaterial.mParameters.size())
                        {
                            if(render->mMaterial.mParameters.find(param.first) != render->mMaterial.mParameters.end())
                            {
                                render->mMaterial.mParameters[param.first] = static_cast<float>(d);
                            }
                        }
                    }
                });
                hLayout->addWidget(doubleSpinBox);
                minimumHeight += 25.33f;
            }
            else if (std::holds_alternative<gsl::vec2>(param.second))
            {
                for(int i = 0; i <= 1; ++i)
                {
                    QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox(widget);
                    doubleSpinBox->setSingleStep(0.1);
                    float value = 0;
                    switch (i)
                    {
                    case 0:
                        value = std::get<gsl::vec2>(param.second).x;
                        connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                        [=](double d)
                        {
                            if(auto render = getRenderComponent())
                            {
                                if(render->mMaterial.mParameters.size())
                                {
                                    if(render->mMaterial.mParameters.find(param.first) != render->mMaterial.mParameters.end())
                                    {
                                        std::get<gsl::vec2>(render->mMaterial.mParameters[param.first]).setX(static_cast<float>(d));
                                    }
                                }
                            }
                        });
                        break;
                    case 1:
                        value = std::get<gsl::vec2>(param.second).y;
                        connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                        [=](double d)
                        {
                            if(auto render = getRenderComponent())
                            {
                                if(render->mMaterial.mParameters.size())
                                {
                                    if(render->mMaterial.mParameters.find(param.first) != render->mMaterial.mParameters.end())
                                    {
                                        std::get<gsl::vec2>(render->mMaterial.mParameters[param.first]).setY(static_cast<float>(d));
                                    }
                                }
                            }
                        });
                        break;
                    }
                    doubleSpinBox->setValue(static_cast<double>(value));
                    hLayout->addWidget(doubleSpinBox);
                }
                minimumHeight += 25.33f;
            }
            else if (std::holds_alternative<gsl::vec3>(param.second))
            {
                for(int i = 0; i <= 2; ++i)
                {
                    QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox(widget);
                    doubleSpinBox->setSingleStep(0.1);
                    float value = 0;
                    switch (i)
                    {
                    case 0:
                        value = std::get<gsl::vec3>(param.second).x;
                        connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                        [=](double d)
                        {
                            if(auto render = getRenderComponent())
                            {
                                if(render->mMaterial.mParameters.size())
                                {
                                    if(render->mMaterial.mParameters.find(param.first) != render->mMaterial.mParameters.end())
                                    {
                                        std::get<gsl::vec3>(render->mMaterial.mParameters[param.first]).setX(static_cast<float>(d));
                                    }
                                }
                            }
                        });
                        break;
                    case 1:
                        value = std::get<gsl::vec3>(param.second).y;
                        connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                        [=](double d)
                        {
                            if(auto render = getRenderComponent())
                            {
                                if(render->mMaterial.mParameters.size())
                                {
                                    if(render->mMaterial.mParameters.find(param.first) != render->mMaterial.mParameters.end())
                                    {
                                        std::get<gsl::vec3>(render->mMaterial.mParameters[param.first]).setY(static_cast<float>(d));
                                    }
                                }
                            }
                        });
                        break;
                    case 2:
                        value = std::get<gsl::vec3>(param.second).z;
                        connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                        [=](double d)
                        {
                            if(auto render = getRenderComponent())
                            {
                                if(render->mMaterial.mParameters.size())
                                {
                                    if(render->mMaterial.mParameters.find(param.first) != render->mMaterial.mParameters.end())
                                    {
                                        std::get<gsl::vec3>(render->mMaterial.mParameters[param.first]).setZ(static_cast<float>(d));
                                    }
                                }
                            }
                        });
                        break;
                    }
                    doubleSpinBox->setValue(static_cast<double>(value));
                    hLayout->addWidget(doubleSpinBox);
                }
                minimumHeight += 25.33f;
            }
            else if (std::holds_alternative<gsl::vec4>(param.second))
            {
                for(int i = 0; i <= 3; ++i)
                {
                    QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox(widget);
                    doubleSpinBox->setSingleStep(0.1);
                    float value = 0;
                    switch (i)
                    {
                    case 0:
                        value = std::get<gsl::vec4>(param.second).x;
                        connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                        [=](double d)
                        {
                            if(auto render = getRenderComponent())
                            {
                                if(render->mMaterial.mParameters.size())
                                {
                                    if(render->mMaterial.mParameters.find(param.first) != render->mMaterial.mParameters.end())
                                    {
                                        std::get<gsl::vec4>(render->mMaterial.mParameters[param.first]).setX(static_cast<float>(d));
                                    }
                                }
                            }
                        });
                        break;
                    case 1:
                        value = std::get<gsl::vec4>(param.second).y;
                        connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                        [=](double d)
                        {
                            if(auto render = getRenderComponent())
                            {
                                if(render->mMaterial.mParameters.size())
                                {
                                    if(render->mMaterial.mParameters.find(param.first) != render->mMaterial.mParameters.end())
                                    {
                                        std::get<gsl::vec4>(render->mMaterial.mParameters[param.first]).setY(static_cast<float>(d));
                                    }
                                }
                            }
                        });
                        break;
                    case 2:
                        value = std::get<gsl::vec4>(param.second).z;
                        connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                        [=](double d)
                        {
                            if(auto render = getRenderComponent())
                            {
                                if(render->mMaterial.mParameters.size())
                                {
                                    if(render->mMaterial.mParameters.find(param.first) != render->mMaterial.mParameters.end())
                                    {
                                        std::get<gsl::vec4>(render->mMaterial.mParameters[param.first]).setZ(static_cast<float>(d));
                                    }
                                }
                            }
                        });
                        break;
                    case 3:
                        value = std::get<gsl::vec4>(param.second).w;
                        connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                        [=](double d)
                        {
                            if(auto render = getRenderComponent())
                            {
                                if(render->mMaterial.mParameters.size())
                                {
                                    if(render->mMaterial.mParameters.find(param.first) != render->mMaterial.mParameters.end())
                                    {
                                        std::get<gsl::vec4>(render->mMaterial.mParameters[param.first]).setW(static_cast<float>(d));
                                    }
                                }
                            }
                        });
                        break;
                    }
                    doubleSpinBox->setValue(static_cast<double>(value));
                    hLayout->addWidget(doubleSpinBox);
                }
                minimumHeight += 25.33f;
            }
            layout->addLayout(hLayout);
            ui->widget_Parameters->setMinimumHeight(static_cast<int>(minimumHeight));
        }
    }
    else
    {
        if(!ui->widget_Parameters->isHidden())
            ui->widget_Parameters->hide();
    }
}

void MeshWidget::on_pushButton_ChangeMeshDropdown_clicked()
{
    if(!mMainWindow->currentEntitySelected) return;

    auto name = ui->comboBox_Meshes->currentText();
    if(!name.length() || name == "None") return;

    if(auto render = getRenderComponent())
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
    if(!name.length() || name == "None") return;

    if(auto render = getRenderComponent())
    {
        render->mMaterial.mShader = ResourceManager::instance()->getShader(name.toStdString());
        updateShaderParameters(render->mMaterial);
    }
}

void MeshWidget::on_pushButton_ChangeTextureDropdown_clicked()
{
    if(!mMainWindow->currentEntitySelected) return;

    auto name = ui->comboBox_Textures->currentText();
    if(!name.length() || name == "None") return;

    if(auto render = getRenderComponent())
    {
        auto texture = ResourceManager::instance()->getTexture(name.toStdString());
        render->mMaterial.mTextures[0] = {texture->id(), texture->mType};
    }
}
