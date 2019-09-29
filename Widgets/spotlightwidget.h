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

private:
    Ui::SpotLightWidget *ui;
};

#endif // SPOTLIGHTWIDGET_H
