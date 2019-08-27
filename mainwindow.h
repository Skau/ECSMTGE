#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QWidget;
class Renderer;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void addViewport(Renderer* renderWindow);

    void showFPS(double deltaTime, double frameCounter);

signals:
    void toggledWireframe();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    QWidget *mRenderWindowContainer;
};

#endif // MAINWINDOW_H
