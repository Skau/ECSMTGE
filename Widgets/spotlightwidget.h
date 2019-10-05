#ifndef SPOTLIGHTWIDGET_H
#define SPOTLIGHTWIDGET_H

#include "componentwidget.h"

namespace Ui {
class SpotLightWidget;
}

class SpotLightWidget : public ComponentWidget
{
    Q_OBJECT

public:
    explicit SpotLightWidget(MainWindow* mainWindow, QWidget *parent = nullptr);

    void updateData() override;

protected slots:
    void Remove() override;

private slots:
    void on_button_ChangeColor_clicked();

private:
    Ui::SpotLightWidget *ui;

    QColor initialColor;
};

#endif // SPOTLIGHTWIDGET_H
