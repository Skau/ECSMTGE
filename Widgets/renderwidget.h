#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>

namespace Ui{
    class Render;
}

class RenderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RenderWidget(QWidget* parent = nullptr);
    ~RenderWidget();

private:
    Ui::Render* ui;
};

#endif // RENDERWIDGET_H
