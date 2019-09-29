#ifndef POINTLIGHTWIDGET_H
#define POINTLIGHTWIDGET_H

#include "componentwidget.h"

namespace Ui {
class PointLightWidget;
}

class PointLightWidget : public ComponentWidget
{
    Q_OBJECT

public:
    explicit PointLightWidget(MainWindow* mainWindow, QWidget *parent = nullptr);

    void updateData() override;

protected slots:
    void Remove() override;

private:
    Ui::PointLightWidget *ui;
};

#endif // POINTLIGHTWIDGET_H
