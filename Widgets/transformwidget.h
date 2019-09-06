#ifndef TRANSFORMWIDGET_H
#define TRANSFORMWIDGET_H

#include <QWidget>

#include "GSL/math.h"
#include "GSL/vector3d.h"

namespace Ui{
    class Transform;
}

class MainWindow;

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

private slots:
    void on_line_Position_X_textEdited(const QString &arg1);

    void on_line_Position_Y_textEdited(const QString &arg1);

    void on_line_Position_Z_textEdited(const QString &arg1);

    void on_line_Rotation_X_textEdited(const QString &arg1);

    void on_line_Rotation_Y_textEdited(const QString &arg1);

    void on_line_Rotation_Z_textEdited(const QString &arg1);

    void on_line_Scale_X_textEdited(const QString &arg1);

    void on_line_Scale_Y_textEdited(const QString &arg1);

    void on_line_Scale_Z_textEdited(const QString &arg1);

private:
    Ui::Transform* ui;
    MainWindow* mMainWindow;
};
#endif // TRANSFORMWIDGET_H
