#ifndef RENDERER_H
#define RENDERER_H

#include <QWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <chrono>
#include "texture.h"
#include "camerasystem.h"
#include "Renderables/visualobject.h"
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

    void updateShaders();

    void render(const std::vector<VisualObject*>& objects, double deltaTime);

    // Should only need renders, materials and transforms
    void render(const std::vector<MeshComponent> &renders, const std::vector<TransformComponent> &transforms, const CameraComponent &camera);
    void renderDeferred(const std::vector<MeshComponent> &renders, const std::vector<TransformComponent> &transforms, const CameraComponent &camera);

private:
    void deferredGeometryPass(const std::vector<MeshComponent> &renders, const std::vector<TransformComponent> &transforms, const CameraComponent &camera);
    void deferredLightningPass(const CameraComponent &camera);
signals:
    void initDone();
    void windowUpdated();


private:
    QOpenGLContext *mContext{nullptr};

    Light *mLight;

    unsigned int mScreenSpacedQuadVAO;
    unsigned int mGBuffer, mGPosition{}, mGNormal{}, mGAlbedoSpec{};
    unsigned int mRboDepth{};
    std::shared_ptr<Shader> mDirectionalLightShader;

    bool mWireframe{false};

    bool isInitialized{false};

   // MainWindow *mMainWindow{nullptr};    //points back to MainWindow to be able to put info in StatusBar

    class QOpenGLDebugLogger *mOpenGLDebugLogger{nullptr};

    void renderQuad();

    void startOpenGLDebugger();
};

#endif // RENDERER_H
