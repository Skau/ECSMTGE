#include "transformwidget.h"
#include "ui_transform.h"
#include "mainwindow.h"
#include "entitymanager.h"

TransformWidget::TransformWidget(MainWindow* mainWindow, QWidget *parent)
    : QWidget(parent), ui(new Ui::Transform), mMainWindow(mainWindow)
{
    ui->setupUi(this);
}

TransformWidget::~TransformWidget()
{
}

void TransformWidget::setPosition(const gsl::vec3 &pos)
{
    ui->line_Position_X->setText(QString::number(static_cast<double>(pos.x)));
    ui->line_Position_Y->setText(QString::number(static_cast<double>(pos.y)));
    ui->line_Position_Z->setText(QString::number(static_cast<double>(pos.z)));
}

void TransformWidget::setRotation(const gsl::vec3 &rot)
{
    ui->line_Rotation_X->setText(QString::number(static_cast<double>(rot.x)));
    ui->line_Rotation_Y->setText(QString::number(static_cast<double>(rot.y)));
    ui->line_Rotation_Z->setText(QString::number(static_cast<double>(rot.z)));
}

void TransformWidget::setScale(const gsl::vec3 &scale)
{
    ui->line_Scale_X->setText(QString::number(static_cast<double>(scale.x)));
    ui->line_Scale_Y->setText(QString::number(static_cast<double>(scale.y)));
    ui->line_Scale_Z->setText(QString::number(static_cast<double>(scale.z)));
}

gsl::vec3 TransformWidget::getPosition()
{
    gsl::vec3 returnVec;
    returnVec.x = ui->line_Position_X->text().toFloat();
    returnVec.y = ui->line_Position_Y->text().toFloat();
    returnVec.z = ui->line_Position_Z->text().toFloat();
    return returnVec;
}

gsl::vec3 TransformWidget::getRotation()
{
    gsl::vec3 returnVec;
    returnVec.x = ui->line_Rotation_X->text().toFloat();
    returnVec.y = ui->line_Rotation_Y->text().toFloat();
    returnVec.z = ui->line_Rotation_Z->text().toFloat();
    return returnVec;
}

gsl::vec3 TransformWidget::getScale()
{
    gsl::vec3 returnVec;
    returnVec.x = ui->line_Scale_X->text().toFloat();
    returnVec.y = ui->line_Scale_Y->text().toFloat();
    returnVec.z = ui->line_Scale_Z->text().toFloat();
    return returnVec;
}

void TransformWidget::on_line_Position_X_textEdited(const QString &arg1)
{
    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto transform = entityManager->getComponent<Transform>(entityData->entityId);
        if(transform)
        {
            transform->position = gsl::vec3(arg1.toFloat(), transform->position.y, transform->position.z);
            transform->updated = true;
        }
    }
}

void TransformWidget::on_line_Position_Y_textEdited(const QString &arg1)
{
    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto transform = entityManager->getComponent<Transform>(entityData->entityId);
        if(transform)
        {
            transform->position = gsl::vec3(transform->position.x, arg1.toFloat(), transform->position.z);
            transform->updated = true;
        }
    }
}

void TransformWidget::on_line_Position_Z_textEdited(const QString &arg1)
{
    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto transform = entityManager->getComponent<Transform>(entityData->entityId);
        if(transform)
        {
            transform->position = gsl::vec3(transform->position.x, transform->position.y, arg1.toFloat());
            transform->updated = true;
        }
    }
}

void TransformWidget::on_line_Rotation_X_textEdited(const QString &arg1)
{
    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto transform = entityManager->getComponent<Transform>(entityData->entityId);
        if(transform)
        {
            transform->rotation = gsl::vec3(arg1.toFloat(), transform->rotation.y, transform->rotation.z);
            transform->updated = true;
        }
    }
}

void TransformWidget::on_line_Rotation_Y_textEdited(const QString &arg1)
{
    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto transform = entityManager->getComponent<Transform>(entityData->entityId);
        if(transform)
        {
            transform->rotation = gsl::vec3(transform->rotation.x, arg1.toFloat(), transform->rotation.z);
            transform->updated = true;
        }
    }
}

void TransformWidget::on_line_Rotation_Z_textEdited(const QString &arg1)
{
    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto transform = entityManager->getComponent<Transform>(entityData->entityId);
        if(transform)
        {
            transform->rotation = gsl::vec3(transform->rotation.x, transform->rotation.y, arg1.toFloat());
            transform->updated = true;
        }
    }
}

void TransformWidget::on_line_Scale_X_textEdited(const QString &arg1)
{
    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto transform = entityManager->getComponent<Transform>(entityData->entityId);
        if(transform)
        {
            transform->scale = gsl::vec3(arg1.toFloat(), transform->scale.y, transform->scale.z);
            transform->updated = true;
        }
    }
}

void TransformWidget::on_line_Scale_Y_textEdited(const QString &arg1)
{
    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto transform = entityManager->getComponent<Transform>(entityData->entityId);
        if(transform)
        {
            transform->scale = gsl::vec3(transform->scale.x, arg1.toFloat(), transform->scale.z);
            transform->updated = true;
        }
    }
}

void TransformWidget::on_line_Scale_Z_textEdited(const QString &arg1)
{
    auto entityData = mMainWindow->currentEntitySelected;
    if(entityData)
    {
        auto entityManager = mMainWindow->getEntityManager();
        auto transform = entityManager->getComponent<Transform>(entityData->entityId);
        if(transform)
        {
            transform->scale = gsl::vec3(transform->scale.x, transform->scale.y, arg1.toFloat());
            transform->updated = true;
        }
    }
}
