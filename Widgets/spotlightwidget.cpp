#include "spotlightwidget.h"
#include "ui_spotlight.h"
#include "mainwindow.h"
#include "entitymanager.h"

SpotLightWidget::SpotLightWidget(MainWindow *mainWindow, QWidget *parent) :
    ComponentWidget(mainWindow, parent), ui(new Ui::SpotLightWidget)
{
    ui->setupUi(this);
}

void SpotLightWidget::updateData()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto comp = mMainWindow->getEntityManager()->getComponent<SpotLightComponent>(entity->entityId))
        {
            isUpdating = true;



            isUpdating = false;
        }
    }
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

