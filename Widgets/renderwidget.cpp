#include "renderwidget.h"
#include "ui_render.h"

RenderWidget::RenderWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::Render)
{
    ui->setupUi(this);
}

RenderWidget::~RenderWidget()
{

}
