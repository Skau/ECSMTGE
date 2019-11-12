#include "objecttreewidget.h"
#include <QDropEvent>
#include <QDebug>
#include <QMimeData>
#include "mainwindow.h"
#include "entitymanager.h"

ObjectTreeWidget::ObjectTreeWidget(QWidget* parent) : QTreeWidget(parent)
{

}

void ObjectTreeWidget::dropEvent(QDropEvent* event)
{
    auto currentEntity = mMainWindow->getEntityAt(currentItem());
    auto destinationEntity = mMainWindow->getEntityAt(itemAt(event->pos()));
    auto parentEntity = mMainWindow->getEntityAt(currentItem()->parent());

    if(currentEntity)
    {
        if(parentEntity)
        {
            if(auto transform = mMainWindow->getEntityManager()->getComponent<TransformComponent>(parentEntity->entityId))
            {
                transform->children.erase(std::remove(transform->children.begin(), transform->children.end(), currentEntity->entityId), transform->children.end());
            }
        }

        if(destinationEntity)
        {
            if(auto transform = mMainWindow->getEntityManager()->getComponent<TransformComponent>(destinationEntity->entityId))
            {
                if (std::find(transform->children.begin(), transform->children.end(), currentEntity->entityId) == transform->children.end())
                    transform->children.push_back(currentEntity->entityId);
            }
        }
    }

    QTreeWidget::dropEvent(event);
}
