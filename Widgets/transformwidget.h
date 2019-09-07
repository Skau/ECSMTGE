#ifndef TRANSFORMWIDGET_H
#define TRANSFORMWIDGET_H

#include <QWidget>

#include "GSL/math.h"
#include "GSL/vector3d.h"

namespace Ui{
    class Transform;
}

class MainWindow;
enum class ComponentType;

class TransformWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TransformWidget(MainWindow *mainWindow, QWidget* parent = nullptr);
    ~TransformWidget();

    void setPosition(const gsl::vec3& pos);
    void setRotation(const gsl::vec3& rot);
    void setScale(const gsl::vec3& scale);

    gsl::vec3 getPosition();
    gsl::vec3 getRotation();
    gsl::vec3 getScale();

signals:
    void widgetRemoved();

public slots:
    void update(const gsl::vec3& pos, const gsl::vec3& rot, const gsl::vec3& scale);

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

    void ProvideContextMenu(const QPoint& point);

    void Remove();
private:
    Ui::Transform* ui;
    MainWindow* mMainWindow;
};
#endif // TRANSFORMWIDGET_H
