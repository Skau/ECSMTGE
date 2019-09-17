#ifndef PHYSICSWIDGET_H
#define PHYSICSWIDGET_H

#include "componentwidget.h"

#include "GSL/math.h"
#include "GSL/vector3d.h"

namespace Ui{
    class Physics;
}

class PhysicsWidget : public ComponentWidget
{
    Q_OBJECT

public:
    explicit PhysicsWidget(MainWindow *mainWindow, QWidget* parent = nullptr);

    void updateData() override;

    void setVelocity(const gsl::vec3& vel);
    void setAcceleration(const gsl::vec3& acc);
    void setMass(float ass); // ;)

    gsl::vec3 getVelocity();
    gsl::vec3 getAcceleration();
    float getMass();

private slots:
    void on_spinBox_Velocity_X_valueChanged(double arg1);

    void on_spinBox_Velocity_Y_valueChanged(double arg1);

    void on_spinBox_Velocity_Z_valueChanged(double arg1);

    void on_spinBox_Acceleration_X_valueChanged(double arg1);

    void on_spinBox_Acceleration_Y_valueChanged(double arg1);

    void on_spinBox_Acceleration_Z_valueChanged(double arg1);

    void on_spinBox_Mass_valueChanged(double arg1);

    void Remove() override;

private:
    Ui::Physics* ui;
};
#endif // PhysicsWIDGET_H
