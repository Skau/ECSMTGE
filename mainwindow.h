#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

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

    void showFPS(double deltaTime, double frameCounter);

    Renderer* getRenderer() { return mRenderer; }
    Ui::MainWindow *ui;

private:
    Renderer* mRenderer;
    QWidget *mRenderWindowContainer;
};

#endif // MAINWINDOW_H
