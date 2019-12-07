#ifndef POINTLIGHTWIDGET_H
#define POINTLIGHTWIDGET_H

#include "componentwidget.h"

namespace Ui {
class PointLightWidget;
}

/** Widget for pointlightcomponents in the editor.
 * @brief Widget for pointlightcomponents in the editor.
 */
class PointLightWidget : public ComponentWidget
{
    Q_OBJECT

public:
    explicit PointLightWidget(MainWindow* mainWindow, QWidget *parent = nullptr);

protected slots:
    void Remove() override;

private slots:
    void on_button_ChangeColor_clicked();

    void on_radiusSpinBox_valueChanged(double arg1);

    void on_intensitySpinBox_valueChanged(double arg1);

private:
    Ui::PointLightWidget *ui;

    QColor initialColor;
};

#endif // POINTLIGHTWIDGET_H
