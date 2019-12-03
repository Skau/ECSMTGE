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
 * @brief The creator of all things. This is the main class holding both the UI and the World, connecting them wherever necessary.
 */
class App : public QObject
{
    Q_OBJECT

private:
    // Settings
    float FOV = 45.f;

public:
    App();
    ~App();

signals:
    void initScene();

public slots:
    /**
     * @brief Called after the renderer has created its context.
     */
    void initTheRest();
    /**
     * @brief Global mute.
     */
    void toggleMute(bool mode);
    void mousePicking();
    void saveSession();

private slots:
    /**
     * @brief The main update loop.
     */
    void update();
    /**
     * @brief Quits the application.
     */
    void quit();
    /**
     * @brief Calculates new perspective matrix for the cameras.
     */
    void updatePerspective();

    /**
     * @brief Called when play is pressed.
     */
    void onPlay();
    /**
     * @brief Called when stop is pressed.
     */
    void onStop();
    /**
     * @brief Creates a new base scene with only a game camera and some lights.
     */
    void newScene();
    /**
     * @brief Loads the scene at the given path.
     */
    void loadScene(const std::string& path);
    /**
     * @brief Saves the scene to the given path.
     */
    void saveScene(const std::string& path);

private:
    /**
     * @brief Calculates the current FPS and time between frames using delta time and frame counting.
     */
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
