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

}

void ColliderWidget::onSelected()
{
    if (auto entity = mMainWindow->currentEntitySelected)
    {
        auto comp = mMainWindow->getEntityManager()->getComponent<ColliderComponent>(entity->entityId);
        if (comp) {
            auto index = static_cast<int>(comp->collisionType);
            lastHighlighted = index;
            ui->comboBox_Colliders->setCurrentIndex(index);
            ready = true;
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

void ColliderWidget::on_comboBox_Colliders_currentIndexChanged(int index)
{
    if (!ready)
        return;

    if (auto entity = mMainWindow->currentEntitySelected)
    {
        auto comp = mMainWindow->getEntityManager()->getComponent<ColliderComponent>(entity->entityId);
        auto currentIndex = static_cast<int>(comp->collisionType);
        if (lastHighlighted == index && currentIndex != index)
        {
            comp->collisionType = static_cast<ColliderComponent::Type>(currentIndex);
        }
    }
}

void ColliderWidget::on_comboBox_Colliders_highlighted(int index)
{
    lastHighlighted = index;
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
