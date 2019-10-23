#include "colliderwidget.h"
#include "ui_collider.h"
#include "mainwindow.h"
#include "entitymanager.h"
#include <memory>

ColliderWidget::ColliderWidget(MainWindow *mainWindow, QWidget *parent)
    : ComponentWidget(mainWindow, parent), ui(new Ui::Collider)
{
    ui->setupUi(this);

    for (auto type : ColliderComponent::typeNames)
    {
       ui->comboBox_Colliders->addItem(type);
    }
}

void ColliderWidget::updateData()
{
    isUpdating = true;

    if (auto entity = mMainWindow->currentEntitySelected)
    {
        auto comp = mMainWindow->getEntityManager()->getComponent<ColliderComponent>(entity->entityId);
        if (comp) {
            currentIndex = static_cast<int>(comp->collisionType);
            ui->comboBox_Colliders->setCurrentIndex(currentIndex);
        }

    }
    isUpdating = false;
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

void ColliderWidget::on_comboBox_Colliders_currentIndexChanged(int index)
{
    if (isUpdating)
        return;

    if (auto entity = mMainWindow->currentEntitySelected)
    {
        auto comp = mMainWindow->getEntityManager()->getComponent<ColliderComponent>(entity->entityId);
        if (lastHighlighted == index && static_cast<int>(comp->collisionType) != index)
        {
            currentIndex = index;
            qDebug() << entity->entityId << "'s index changed to: " << index;
        }
    }
}

void ColliderWidget::on_comboBox_Colliders_highlighted(int index)
{
    if (isUpdating)
        return;

    lastHighlighted = index;
    qDebug() << "last highlighted: " << lastHighlighted;
}

void ColliderWidget::updateParameters()
{
    if (ui->widget_Parameters->children().size())
    {
        for (auto& child : ui->widget_Parameters->children())
        {
            delete child;
        }
    }


    if (ui->widget_Parameters->isHidden())
        ui->widget_Parameters->show();


}
