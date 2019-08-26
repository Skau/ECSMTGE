#ifndef APP_H
#define APP_H

#include <QObject>
#include <QElapsedTimer>
#include <QTimer>

class MainWindow;
class RenderWindow;

class App : public QObject
{
    Q_OBJECT

public:
    App();

    void init();

private slots:
    void update();

private:
    void processInput();
    void render();

    MainWindow* mMainWindow;
    RenderWindow* mRenderWindow;

    QElapsedTimer mElapsedTime;
    QTimer mTimer;

    double previous = 0;
    double lag = 0;
    double MS_PER_UPDATE = 1.0 / 120.0;

    double deltaTime = 1.0 / 60.0;
};

#endif // APP_H
