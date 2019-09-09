#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include <QWidget>

namespace Ui{
    class Input;
}

class MainWindow;

class InputWidget : public QWidget
{
    Q_OBJECT

public:
    InputWidget(MainWindow *mainWindow, QWidget* parent = nullptr);

signals:
    void widgetRemoved();

public slots:
    void update(bool isControlled);

private slots:
    void ProvideContextMenu(const QPoint& point);
    void Remove();

    void on_checkBox_IsBeingControlled_toggled(bool checked);

private:
    Ui::Input* ui;
    MainWindow* mMainWindow;
};

#endif // INPUTWIDGET_H
