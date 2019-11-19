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

protected slots:
    void Remove() override;

private slots:
    void on_button_ChangeColor_clicked();

private:
    Ui::DirectionalLightWidget *ui;

    QColor initialColor;
};

#endif // DIRECTIONALLIGHTWIDGET_H
