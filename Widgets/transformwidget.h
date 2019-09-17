#ifndef TRANSFORMWIDGET_H
#define TRANSFORMWIDGET_H

#include "componentwidget.h"

#include "GSL/math.h"
#include "GSL/vector3d.h"
#include "GSL/quaternion.h"

namespace Ui{
    class Transform;
}

class TransformWidget : public ComponentWidget
{
    Q_OBJECT

public:
    explicit TransformWidget(MainWindow *mainWindow, QWidget* parent = nullptr);

    void updateData() override;

    void setPosition(const gsl::vec3& pos);
    void setRotation(const gsl::vec3 &rot);
    void setScale(const gsl::vec3& scale);

    gsl::vec3 getPosition();
    gsl::vec3 getRotation();
    gsl::vec3 getScale();

private slots:
    void on_spinBox_Position_X_valueChanged(double arg1);

    void on_spinBox_Position_Y_valueChanged(double arg1);

    void on_spinBox_Position_Z_valueChanged(double arg1);

    void on_spinBox_Rotation_X_valueChanged(double arg1);

    void on_spinBox_Rotation_Y_valueChanged(double arg1);

    void on_spinBox_Rotation_Z_valueChanged(double arg1);

    void on_spinBox_Scale_X_valueChanged(double arg1);

    void on_spinBox_Scale_Y_valueChanged(double arg1);

    void on_spinBox_Scale_Z_valueChanged(double arg1);

    void Remove() override;

private:
    Ui::Transform* ui;
};
#endif // TRANSFORMWIDGET_H
