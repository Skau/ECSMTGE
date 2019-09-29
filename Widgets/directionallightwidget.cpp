#include "directionallightwidget.h"
#include "ui_directionallight.h"
#include "mainwindow.h"
#include "entitymanager.h"

DirectionalLightWidget::DirectionalLightWidget(MainWindow *mainWindow, QWidget *parent) :
    ComponentWidget(mainWindow, parent), ui(new Ui::DirectionalLightWidget)
{
    ui->setupUi(this);
}

void DirectionalLightWidget::updateData()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto comp = mMainWindow->getEntityManager()->getComponent<DirectionalLightComponent>(entity->entityId))
        {
            isUpdating = true;



            isUpdating = false;
        }
    }
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
