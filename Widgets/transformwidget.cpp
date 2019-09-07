#include "transformwidget.h"
#include "ui_transform.h"
#include "mainwindow.h"
#include "entitymanager.h"
#include <QMenu>

TransformWidget::TransformWidget(MainWindow* mainWindow, QWidget *parent)
    : QWidget(parent), ui(new Ui::Transform), mMainWindow(mainWindow)
{
    ui->setupUi(this);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &TransformWidget::ProvideContextMenu);
}

TransformWidget::~TransformWidget()
{
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

gsl::vec3 TransformWidget::getPosition()
{
    gsl::vec3 returnVec;
    returnVec.x = static_cast<float>(ui->spinBox_Position_X->value());
    returnVec.y = static_cast<float>(ui->spinBox_Position_Y->value());
    returnVec.z = static_cast<float>(ui->spinBox_Position_Z->value());
    return returnVec;
}

gsl::vec3 TransformWidget::getRotation()
{
    gsl::vec3 returnVec;
    returnVec.x = static_cast<float>(ui->spinBox_Rotation_X->value());
    returnVec.y = static_cast<float>(ui->spinBox_Rotation_Y->value());
    returnVec.z = static_cast<float>(ui->spinBox_Rotation_Z->value());
    return returnVec;
}

gsl::vec3 TransformWidget::getScale()
{
    gsl::vec3 returnVec;
    returnVec.x = static_cast<float>(ui->spinBox_Scale_X->value());
    returnVec.y = static_cast<float>(ui->spinBox_Scale_Y->value());
    returnVec.z = static_cast<float>(ui->spinBox_Scale_Z->value());
    return returnVec;
}

void TransformWidget::update(const gsl::vec3 &pos, const gsl::vec3 &rot, const gsl::vec3 &scale)
{
    setPosition(pos);
    setRotation(rot);
    setScale(scale);
}

void TransformWidget::on_spinBox_Position_X_valueChanged(double arg1)
{
    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto transform = entityManager->getComponent<Transform>(entityData->entityId);
        if(transform)
        {
            transform->position = gsl::vec3(static_cast<float>(arg1), transform->position.y, transform->position.z);
            transform->updated = true;
        }
    }
}

void TransformWidget::on_spinBox_Position_Y_valueChanged(double arg1)
{
    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto transform = entityManager->getComponent<Transform>(entityData->entityId);
        if(transform)
        {
            transform->position = gsl::vec3(transform->position.x, static_cast<float>(arg1), transform->position.z);
            transform->updated = true;
        }
    }
}

void TransformWidget::on_spinBox_Position_Z_valueChanged(double arg1)
{
    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto transform = entityManager->getComponent<Transform>(entityData->entityId);
        if(transform)
        {
            transform->position = gsl::vec3(transform->position.x, transform->position.y, static_cast<float>(arg1));
            transform->updated = true;
        }
    }
}

void TransformWidget::on_spinBox_Rotation_X_valueChanged(double arg1)
{
    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto transform = entityManager->getComponent<Transform>(entityData->entityId);
        if(transform)
        {
            transform->rotation = gsl::vec3(static_cast<float>(arg1), transform->rotation.y, transform->rotation.z);
            transform->updated = true;
        }
    }
}

void TransformWidget::on_spinBox_Rotation_Y_valueChanged(double arg1)
{
    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto transform = entityManager->getComponent<Transform>(entityData->entityId);
        if(transform)
        {
            transform->rotation = gsl::vec3(transform->rotation.x, static_cast<float>(arg1), transform->rotation.z);
            transform->updated = true;
        }
    }
}

void TransformWidget::on_spinBox_Rotation_Z_valueChanged(double arg1)
{
    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto transform = entityManager->getComponent<Transform>(entityData->entityId);
        if(transform)
        {
            transform->rotation = gsl::vec3(transform->rotation.x, transform->rotation.y, static_cast<float>(arg1));
            transform->updated = true;
        }
    }
}

void TransformWidget::on_spinBox_Scale_X_valueChanged(double arg1)
{
    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto transform = entityManager->getComponent<Transform>(entityData->entityId);
        if(transform)
        {
            transform->scale = gsl::vec3(static_cast<float>(arg1), transform->scale.y, transform->scale.z);
            transform->updated = true;
        }
    }
}

void TransformWidget::on_spinBox_Scale_Y_valueChanged(double arg1)
{
    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto transform = entityManager->getComponent<Transform>(entityData->entityId);
        if(transform)
        {
            transform->scale = gsl::vec3(transform->scale.x, static_cast<float>(arg1), transform->scale.z);
            transform->updated = true;
        }
    }
}

void TransformWidget::on_spinBox_Scale_Z_valueChanged(double arg1)
{
    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto transform = entityManager->getComponent<Transform>(entityData->entityId);
        if(transform)
        {
            transform->scale = gsl::vec3(transform->scale.x, transform->scale.y, static_cast<float>(arg1));
            transform->updated = true;
        }
    }
}

void TransformWidget::ProvideContextMenu(const QPoint& point)
{
    QMenu subMenu;
    subMenu.addAction("Remove", this, &TransformWidget::Remove);

    QPoint globalPos = mapToGlobal(point);

    subMenu.exec(globalPos);
}

void TransformWidget::Remove()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(mMainWindow->getEntityManager()->removeComponent<Transform>(entity->entityId))
        {
            widgetRemoved();
        }
    }
}
