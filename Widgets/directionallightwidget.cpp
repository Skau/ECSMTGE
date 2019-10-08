#include "directionallightwidget.h"
#include "ui_directionallight.h"
#include "mainwindow.h"
#include "entitymanager.h"
#include <QColorDialog>

DirectionalLightWidget::DirectionalLightWidget(MainWindow *mainWindow, QWidget *parent) :
    ComponentWidget(mainWindow, parent), ui(new Ui::DirectionalLightWidget)
{
    ui->setupUi(this);
    initialColor = Qt::white;

    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto comp = mMainWindow->getEntityManager()->getComponent<DirectionalLightComponent>(entity->entityId))
        {
            isUpdating = true;
            initialColor =  QColor(
                                static_cast<int>(comp->color.x)*255,
                                static_cast<int>(comp->color.y)*255,
                                static_cast<int>(comp->color.z)*255);

            ui->textEdit_Color->setStyleSheet("background-color: " + initialColor.name());
            isUpdating = false;
        }
    }
}

void DirectionalLightWidget::updateData()
{
//    auto entity = mMainWindow->currentEntitySelected;
//    if(entity)
//    {
//        if(auto comp = mMainWindow->getEntityManager()->getComponent<DirectionalLightComponent>(entity->entityId))
//        {
//            isUpdating = true;
//            isUpdating = false;
//        }
//    }
}

void DirectionalLightWidget::Remove()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(mMainWindow->getEntityManager()->removeComponent<DirectionalLightComponent>(entity->entityId))
        {
            widgetRemoved(this);
        }
    }
}

void DirectionalLightWidget::on_button_ChangeColor_clicked()
{
    if(auto entity = mMainWindow->currentEntitySelected)
    {
        QColor color = QColorDialog::getColor(initialColor, this, "Change light color");

        if(auto comp = mMainWindow->getEntityManager()->getComponent<DirectionalLightComponent>(entity->entityId))
        {
            comp->color = gsl::vec3(static_cast<float>(color.redF()), static_cast<float>(color.greenF()), static_cast<float>(color.blueF()));
            initialColor = color;
            ui->textEdit_Color->setStyleSheet("background-color: " + initialColor.name());
        }
    }
}
