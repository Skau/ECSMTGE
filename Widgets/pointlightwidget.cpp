#include "pointlightwidget.h"
#include "ui_pointlight.h"
#include "mainwindow.h"
#include "entitymanager.h"

PointLightWidget::PointLightWidget(MainWindow *mainWindow, QWidget *parent) :
     ComponentWidget(mainWindow, parent), ui(new Ui::PointLightWidget)
{
    ui->setupUi(this);
}

void PointLightWidget::updateData()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto comp = mMainWindow->getEntityManager()->getComponent<PointLightComponent>(entity->entityId))
        {
            isUpdating = true;



            isUpdating = false;
        }
    }
}

void PointLightWidget::Remove()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(mMainWindow->getEntityManager()->removeComponent<PointLightComponent>(entity->entityId))
        {
            widgetRemoved(this);
        }
    }
}
