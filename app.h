#ifndef APP_H
#define APP_H

#include <memory>

#include <QObject>
#include <QElapsedTimer>
#include <QTimer>

#include "mainwindow.h"
#include "renderer.h"
#include "world.h"

#include "soundmanager.h"
#include "soundlistener.h"
#include "scriptsystem.h"

class InputHandler;
class InputSystem;

/**
 * @brief The creator of all things.
 */
class App : public QObject
{
    Q_OBJECT

private:
    // Settings
    float FOV = 45.f;

public:
    App();


signals:
    void initScene();

public slots:
    void initTheRest();
    void toggleMute(bool mode);
    void mousePicking();

private slots:
    void update();
    void quit();
    void updatePerspective();

    void onPlay();
    void onStop();

private:

    void calculateFrames();

    std::unique_ptr<MainWindow> mMainWindow;

    Renderer* mRenderer;
    InputSystem* mInputSystem;

    std::unique_ptr<SoundManager> mSoundManager;
    std::unique_ptr<SoundListener> mSoundListener;

    // Handles input events in Renderer
    std::shared_ptr<InputHandler> mEventHandler;
    std::unique_ptr<World> mWorld;

    QTimer mUpdateTimer; // Calls update

    float mDeltaTime;
    float mTotalDeltaTime;

    QElapsedTimer mDeltaTimer;

    QElapsedTimer mFPSTimer;
    int mFrameCounter = 0;

    bool currentlyUpdating = false;

    char padding[3]; // to get rid of annoying alignment boundary issue (and because I'm too lazy to remove the warning)

    bool mCurrentlyPlaying = false;
};

#endif // APP_H
