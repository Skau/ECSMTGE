#include "transformwidget.h"
#include "ui_transform.h"
#include "mainwindow.h"
#include "world.h"
#include <QMenu>

TransformWidget::TransformWidget(MainWindow* mainWindow, QWidget *parent)
    : ComponentWidget(mainWindow, parent), ui(new Ui::Transform)
{
    ui->setupUi(this);

    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto transform = World::getWorld().getEntityManager()->getComponent<TransformComponent>(entity->entityId))
        {
                isUpdating = true;

                setPosition(transform->position);
                editorRot = transform->rotation.toEuler();
                setRotation(editorRot);
                setScale(transform->scale);

                isUpdating = false;
        }
    }
}

void TransformWidget::updateData()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto transform = World::getWorld().getEntityManager()->getComponent<TransformComponent>(entity->entityId))
        {
            if(transform->updated)
            {
                isUpdating = true;

                setPosition(transform->position);
                setScale(transform->scale);

                isUpdating = false;
            }
        }
    }
}

void TransformWidget::setPosition(const gsl::vec3 &pos)
{
    ui->spinBox_Position_X->setValue(static_cast<double>(pos.x));
    ui->spinBox_Position_Y->setValue(static_cast<double>(pos.y));
    ui->spinBox_Position_Z->setValue(static_cast<double>(pos.z));
}

void TransformWidget::setRotation(const gsl::vec3 &rot)
{
    ui->spinBox_Rotation_X->setValue(static_cast<double>(rot.x));
    ui->spinBox_Rotation_Y->setValue(static_cast<double>(rot.y));
    ui->spinBox_Rotation_Z->setValue(static_cast<double>(rot.z));
}

void TransformWidget::setScale(const gsl::vec3 &scale)
{
    ui->spinBox_Scale_X->setValue(static_cast<double>(scale.x));
    ui->spinBox_Scale_Y->setValue(static_cast<double>(scale.y));
    ui->spinBox_Scale_Z->setValue(static_cast<double>(scale.z));
}

void TransformWidget::on_spinBox_Position_X_valueChanged(double arg1)
{
    if(isUpdating) return;

    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = World::getWorld().getEntityManager();
        auto oldPos = entityManager->getComponent<TransformComponent>(entityData->entityId)->position;
        entityManager->setTransformPos(entityData->entityId,
            gsl::vec3(static_cast<float>(arg1), oldPos.y, oldPos.z));
    }
}

void TransformWidget::on_spinBox_Position_Y_valueChanged(double arg1)
{
    if(isUpdating) return;

    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = World::getWorld().getEntityManager();
        auto oldPos = entityManager->getComponent<TransformComponent>(entityData->entityId)->position;
        entityManager->setTransformPos(entityData->entityId,
            gsl::vec3(oldPos.x, static_cast<float>(arg1), oldPos.z));
    }
}

void TransformWidget::on_spinBox_Position_Z_valueChanged(double arg1)
{
    if(isUpdating) return;

    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = World::getWorld().getEntityManager();
        auto oldPos = entityManager->getComponent<TransformComponent>(entityData->entityId)->position;
        entityManager->setTransformPos(entityData->entityId,
            gsl::vec3(oldPos.x, oldPos.y, static_cast<float>(arg1)));
    }
}

void TransformWidget::on_spinBox_Rotation_X_valueChanged(double arg1)
{
    if(isUpdating) return;

    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = World::getWorld().getEntityManager();
        editorRot.x = static_cast<float>(arg1);
        if (editorRot.x < -gsl::PI * 2.f)
            editorRot.x += gsl::PI * 2.f;
        else if (editorRot.x > gsl::PI * 2.f)
            editorRot.x -= gsl::PI * 2.f;

        entityManager->setTransformRot(entityData->entityId,
            editorRot.toQuat());
    }
}

void TransformWidget::on_spinBox_Rotation_Y_valueChanged(double arg1)
{
    if(isUpdating) return;

    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = World::getWorld().getEntityManager();
        editorRot.y = static_cast<float>(arg1);
        if (editorRot.y < -gsl::PI * 2.f)
            editorRot.y += gsl::PI * 2.f;
        else if (editorRot.y > gsl::PI * 2.f)
            editorRot.y -= gsl::PI * 2.f;

        entityManager->setTransformRot(entityData->entityId,
            editorRot.toQuat());
    }
}

void TransformWidget::on_spinBox_Rotation_Z_valueChanged(double arg1)
{
    if(isUpdating) return;

    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = World::getWorld().getEntityManager();
        editorRot.z = static_cast<float>(arg1);
        if (editorRot.z < -gsl::PI * 2.f)
            editorRot.z += gsl::PI * 2.f;
        else if (editorRot.z > gsl::PI * 2.f)
            editorRot.z -= gsl::PI * 2.f;

        entityManager->setTransformRot(entityData->entityId,
            editorRot.toQuat());
    }
}

void TransformWidget::on_spinBox_Scale_X_valueChanged(double arg1)
{
    if(isUpdating) return;

    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = World::getWorld().getEntityManager();
        auto oldScale = entityManager->getComponent<TransformComponent>(entityData->entityId)->scale;
        entityManager->setTransformScale(entityData->entityId,
            gsl::vec3{static_cast<float>(arg1), oldScale.y, oldScale.z});
    }
}

void TransformWidget::on_spinBox_Scale_Y_valueChanged(double arg1)
{
    if(isUpdating) return;

    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = World::getWorld().getEntityManager();
        auto oldScale = entityManager->getComponent<TransformComponent>(entityData->entityId)->scale;
        entityManager->setTransformScale(entityData->entityId,
            gsl::vec3{oldScale.x, static_cast<float>(arg1), oldScale.z});
    }
}

void TransformWidget::on_spinBox_Scale_Z_valueChanged(double arg1)
{
    if(isUpdating) return;

    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = World::getWorld().getEntityManager();
        auto oldScale = entityManager->getComponent<TransformComponent>(entityData->entityId)->scale;
        entityManager->setTransformScale(entityData->entityId,
            gsl::vec3{oldScale.x, oldScale.y, static_cast<float>(arg1)});
    }
}

void TransformWidget::Remove()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(World::getWorld().getEntityManager()->removeComponent<TransformComponent>(entity->entityId))
        {
            widgetRemoved(this);
        }
    }
}
