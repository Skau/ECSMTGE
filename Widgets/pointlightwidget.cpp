#include "pointlightwidget.h"
#include "ui_pointlight.h"
#include "mainwindow.h"
#include "world.h"
#include <QColorDialog>

PointLightWidget::PointLightWidget(MainWindow *mainWindow, QWidget *parent) :
     ComponentWidget(mainWindow, parent), ui(new Ui::PointLightWidget)
{
    ui->setupUi(this);
    initialColor = Qt::white;

    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto comp = World::getWorld().getEntityManager()->getComponent<PointLightComponent>(entity->entityId))
        {
            isUpdating = true;
            initialColor =  QColor(
                                static_cast<int>(comp->color.x)*255,
                                static_cast<int>(comp->color.y)*255,
                                static_cast<int>(comp->color.z)*255);

            ui->textEdit_Color->setStyleSheet("background-color: " + initialColor.name());
            ui->radiusSpinBox->setValue(static_cast<double>(comp->radius));
            ui->intensitySpinBox->setValue(static_cast<double>(comp->intensity));

            isUpdating = false;
        }
    }
}

void PointLightWidget::Remove()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(World::getWorld().getEntityManager()->removeComponent<PointLightComponent>(entity->entityId))
        {
            widgetRemoved(this);
        }
    }
}

void PointLightWidget::on_button_ChangeColor_clicked()
{
    if(auto entity = mMainWindow->currentEntitySelected)
    {
        QColor color = QColorDialog::getColor(initialColor, this, "Change light color");

        if(auto comp = World::getWorld().getEntityManager()->getComponent<PointLightComponent>(entity->entityId))
        {
            comp->color = gsl::vec3(static_cast<float>(color.redF()), static_cast<float>(color.greenF()), static_cast<float>(color.blueF()));
            initialColor = color;
            ui->textEdit_Color->setStyleSheet("background-color: " + initialColor.name());
        }
    }
}

void PointLightWidget::on_radiusSpinBox_valueChanged(double arg1)
{
    if(auto entity = mMainWindow->currentEntitySelected)
    {
        if(auto comp = World::getWorld().getEntityManager()->getComponent<PointLightComponent>(entity->entityId))
        {
            comp->radius = static_cast<float>(arg1);
        }
    }
}

void PointLightWidget::on_intensitySpinBox_valueChanged(double arg1)
{
    if(auto entity = mMainWindow->currentEntitySelected)
    {
        if(auto comp = World::getWorld().getEntityManager()->getComponent<PointLightComponent>(entity->entityId))
        {
            comp->intensity = static_cast<float>(arg1);
        }
    }
}
