#include "spotlightwidget.h"
#include "ui_spotlight.h"
#include "mainwindow.h"
#include "entitymanager.h"
#include <QColorDialog>

SpotLightWidget::SpotLightWidget(MainWindow *mainWindow, QWidget *parent) :
    ComponentWidget(mainWindow, parent), ui(new Ui::SpotLightWidget)
{
    ui->setupUi(this);
    initialColor = Qt::white;

    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto comp = mMainWindow->getEntityManager()->getComponent<SpotLightComponent>(entity->entityId))
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

void SpotLightWidget::updateData()
{
//    auto entity = mMainWindow->currentEntitySelected;
//    if(entity)
//    {
//        if(auto comp = mMainWindow->getEntityManager()->getComponent<SpotLightComponent>(entity->entityId))
//        {
//            isUpdating = true;

//            isUpdating = false;
//        }
//    }
}

void SpotLightWidget::Remove()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(mMainWindow->getEntityManager()->removeComponent<SpotLightComponent>(entity->entityId))
        {
            widgetRemoved(this);
        }
    }
}


void SpotLightWidget::on_button_ChangeColor_clicked()
{
    if(auto entity = mMainWindow->currentEntitySelected)
    {
        QColor color = QColorDialog::getColor(initialColor, this, "Change light color");

        if(auto comp = mMainWindow->getEntityManager()->getComponent<SpotLightComponent>(entity->entityId))
        {
            comp->color = gsl::vec3(static_cast<float>(color.redF()), static_cast<float>(color.greenF()), static_cast<float>(color.blueF()));
            initialColor = color;
            ui->textEdit_Color->setStyleSheet("background-color: " + initialColor.name());
        }
    }
}
