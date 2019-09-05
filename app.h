#ifndef APP_H
#define APP_H

#include <memory>

#include <QObject>
#include <QElapsedTimer>
#include <QTimer>

#include "mainwindow.h"
#include "renderer.h"
#include "world.h"

/**
 * @brief The creator of all things.
 */
class App : public QObject
{
    Q_OBJECT

public:
    App();


public slots:
    void initTheRest();

private slots:
    void update();
    void quit();

private:

    void calculateFrames();

    std::unique_ptr<MainWindow> mMainWindow;
    Renderer* mRenderer;
    std::unique_ptr<World> mWorld;

    QTimer mUpdateTimer; // Calls update

    double mDeltaTime;
    double mTotalDeltaTime;

    QElapsedTimer mDeltaTimer;

    QElapsedTimer mFPSTimer;
    int mFrameCounter = 0;

    bool currentlyUpdating = false;

    char padding[3]; // to get rid of annoying alignment boundary issue (and because I'm too lazy to remove the warning)
};

#endif // APP_H
