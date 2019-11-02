#include "physicswidget.h"
#include "ui_physics.h"
#include "mainwindow.h"
#include "entitymanager.h"
#include <QMenu>

PhysicsWidget::PhysicsWidget(MainWindow* mainWindow, QWidget *parent)
    : ComponentWidget(mainWindow, parent), ui(new Ui::Physics)
{
    ui->setupUi(this);
}

void PhysicsWidget::updateData()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto physics = mMainWindow->getEntityManager()->getComponent<PhysicsComponent>(entity->entityId))
        {
            isUpdating = true;

            setVelocity(physics->velocity);
            setAcceleration(physics->acceleration);
            setMass(physics->mass);

            isUpdating = false;
        }
    }
}

void PhysicsWidget::setVelocity(const gsl::vec3 &vel)
{
    ui->spinBox_Velocity_X->setValue(static_cast<double>(vel.x));
    ui->spinBox_Velocity_Y->setValue(static_cast<double>(vel.y));
    ui->spinBox_Velocity_Z->setValue(static_cast<double>(vel.z));
}

void PhysicsWidget::setAcceleration(const gsl::vec3 &acc)
{
    ui->spinBox_Acceleration_X->setValue(static_cast<double>(acc.x));
    ui->spinBox_Acceleration_Y->setValue(static_cast<double>(acc.y));
    ui->spinBox_Acceleration_Z->setValue(static_cast<double>(acc.z));
}

void PhysicsWidget::setMass(float mass)
{
    ui->spinBox_Mass->setValue(static_cast<double>(mass));
}

void PhysicsWidget::on_spinBox_Velocity_X_valueChanged(double arg1)
{
    if(isUpdating) return;

    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto physics = entityManager->getComponent<PhysicsComponent>(entityData->entityId);
        if(physics)
        {
            physics->velocity = gsl::vec3(static_cast<float>(arg1), physics->velocity.y, physics->velocity.z);
        }
    }
}

void PhysicsWidget::on_spinBox_Velocity_Y_valueChanged(double arg1)
{
    if(isUpdating) return;

    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto physics = entityManager->getComponent<PhysicsComponent>(entityData->entityId);
        if(physics)
        {
            physics->velocity = gsl::vec3(physics->velocity.x, static_cast<float>(arg1), physics->velocity.z);
        }
    }
}

void PhysicsWidget::on_spinBox_Velocity_Z_valueChanged(double arg1)
{
    if(isUpdating) return;

    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto physics = entityManager->getComponent<PhysicsComponent>(entityData->entityId);
        if(physics)
        {
            physics->velocity = gsl::vec3(physics->velocity.x, physics->velocity.y, static_cast<float>(arg1));
        }
    }
}

void PhysicsWidget::on_spinBox_Acceleration_X_valueChanged(double arg1)
{
    if(isUpdating) return;

    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto physics = entityManager->getComponent<PhysicsComponent>(entityData->entityId);
        if(physics)
        {
            physics->acceleration = gsl::vec3(static_cast<float>(arg1), physics->acceleration.y, physics->acceleration.z);
        }
    }
}

void PhysicsWidget::on_spinBox_Acceleration_Y_valueChanged(double arg1)
{
    if(isUpdating) return;

    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto physics = entityManager->getComponent<PhysicsComponent>(entityData->entityId);
        if(physics)
        {
            physics->acceleration = gsl::vec3(physics->acceleration.x, static_cast<float>(arg1), physics->acceleration.z);
        }
    }
}

void PhysicsWidget::on_spinBox_Acceleration_Z_valueChanged(double arg1)
{
    if(isUpdating) return;

    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto physics = entityManager->getComponent<PhysicsComponent>(entityData->entityId);
        if(physics)
        {
            physics->acceleration = gsl::vec3(physics->acceleration.x, physics->acceleration.y, static_cast<float>(arg1));
        }
    }
}

void PhysicsWidget::on_spinBox_Mass_valueChanged(double arg1)
{
    if(isUpdating) return;

    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto physics = entityManager->getComponent<PhysicsComponent>(entityData->entityId);
        if(physics)
        {
            physics->mass = static_cast<float>(arg1);
        }
    }
}

void PhysicsWidget::Remove()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(mMainWindow->getEntityManager()->removeComponent<PhysicsComponent>(entity->entityId))
        {
            widgetRemoved(this);
        }
    }
}
