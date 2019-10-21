#include "colliderwidget.h"
#include "ui_collider.h"
#include "mainwindow.h"
#include "entitymanager.h"
#include <memory>

ColliderWidget::ColliderWidget(MainWindow *mainWindow, QWidget *parent)
    : ComponentWidget(mainWindow, parent), ui(new Ui::Collider)
{
    ui->setupUi(this);
}

void ColliderWidget::updateData()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto physics = mMainWindow->getEntityManager()->getComponent<ColliderComponent>(entity->entityId))
        {

        }
    }
}

void ColliderWidget::Remove()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(mMainWindow->getEntityManager()->removeComponent<ColliderComponent>(entity->entityId))
        {
            widgetRemoved(this);
        }
    }
}
