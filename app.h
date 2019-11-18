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
class CameraComponent;

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
    /**
     * @brief Called after the renderer has created its context.
     */
    void initTheRest();
    void toggleMute(bool mode);
    void mousePicking();
    void saveSession();

private slots:
    void update();
    void quit();
    void updatePerspective();

    void onPlay();
    void onStop();

    void newScene();
    void loadScene(const std::string& path);
    void saveScene(const std::string& path);

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
    std::unique_ptr<ResourceManager> mResourceManager;

    void loadSession(const std::string& path);

    QTimer mUpdateTimer; // Calls update

    float mDeltaTime;
    float mTotalDeltaTime;

    QElapsedTimer mDeltaTimer;
    QElapsedTimer mFPSTimer;

    int mFrameCounter = 0;

    bool currentlyUpdating = false;

    bool mCurrentlyPlaying = false;
};

#endif // APP_H
