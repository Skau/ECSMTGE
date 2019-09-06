#ifndef TRANSFORMWIDGET_H
#define TRANSFORMWIDGET_H

#include <QWidget>

namespace Ui{
    class Transform;
}


class TransformWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TransformWidget(QWidget* parent = nullptr);
    ~TransformWidget();

private:
    Ui::Transform* ui;
};

#endif // TRANSFORMWIDGET_H
