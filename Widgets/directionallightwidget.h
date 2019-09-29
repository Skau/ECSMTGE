#ifndef DIRECTIONALLIGHTWIDGET_H
#define DIRECTIONALLIGHTWIDGET_H

#include "componentwidget.h"

namespace Ui {
class DirectionalLightWidget;
}

class DirectionalLightWidget : public ComponentWidget
{
    Q_OBJECT

public:
    explicit DirectionalLightWidget(MainWindow* mainWindow, QWidget *parent = nullptr);

    void updateData() override;

protected slots:
    void Remove() override;

private:
    Ui::DirectionalLightWidget *ui;
};

#endif // DIRECTIONALLIGHTWIDGET_H
