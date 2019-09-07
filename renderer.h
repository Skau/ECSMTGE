#ifndef RENDERER_H
#define RENDERER_H

#include <QWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <chrono>
#include "texture.h"
#include "camera.h"
#include "Renderables/visualobject.h"
#include "input.h"
#include "componentdata.h"


class QOpenGLContext;
class Shader;
class Light;
class QTime;

/// This inherits from QWindow to get access to the Qt functionality and
/// OpenGL surface.
/// We also inherit from QOpenGLFunctions, to get access to the OpenGL functions
/// This is the same as using glad and glw from general OpenGL tutorials
class Renderer : public QWindow, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT
public:
    Renderer();
    ~Renderer() override;

    QOpenGLContext* getContext() { return mContext; }

    void init();

    void exposeEvent(QExposeEvent *) override;
    void toggleWireframe();

    void checkForGLerrors();

    void handleInput(double deltaTime);
    void render(const std::vector<VisualObject*>& objects, double deltaTime);

    // Should only need renders, materials and transforms
    void render(std::vector<Render> renders, std::vector<Transform> transforms, Camera camera);

    void setupCamera();

    CameraSystem *mCurrentCamera{nullptr};

signals:
    void initDone();
    void windowUpdated();


private:
    void setCameraSpeed(float value);

    QOpenGLContext *mContext{nullptr};

    Light *mLight;

    bool mWireframe{false};

    bool isInitialized{false};

    float mAspectratio{1.f};

   // MainWindow *mMainWindow{nullptr};    //points back to MainWindow to be able to put info in StatusBar

    class QOpenGLDebugLogger *mOpenGLDebugLogger{nullptr};

    void startOpenGLDebugger();
};

#endif // RENDERER_H
