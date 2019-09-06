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
