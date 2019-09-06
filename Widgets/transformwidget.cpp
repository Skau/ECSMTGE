#include "transformwidget.h"
#include "ui_transform.h"

TransformWidget::TransformWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Transform)
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
