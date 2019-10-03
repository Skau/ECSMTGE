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

    friend class App;
    friend class Postprocessor;
public:
    Renderer();
    ~Renderer() override;

    QOpenGLContext* getContext() { return mContext; }

    void init();

    void exposeEvent(QExposeEvent *) override;
    void toggleWireframe();

    void checkForGLerrors();

    void render(const std::vector<VisualObject*>& objects, double deltaTime);

    // Should only need renders, materials and transforms
    void render(const std::vector<MeshComponent> &renders, const std::vector<TransformComponent> &transforms, const CameraComponent &camera);
    void renderDeferred(const std::vector<MeshComponent> &renders, const std::vector<TransformComponent> &transforms, const CameraComponent &camera,
                        const std::vector<DirectionalLightComponent>& dirLights = std::vector<DirectionalLightComponent>(),
                        const std::vector<SpotLightComponent>& spotLights = std::vector<SpotLightComponent>(),
                        const std::vector<PointLightComponent>& pointLights = std::vector<PointLightComponent>());

    /** Renders a picture to the screen and uses the pixel locations to figure out what objects the mouse i hovering over.
     * The process is a accurate, but slow process and therefore should never be used in runtime; only in the editor.
     */
    gsl::vec2 getMouseHoverObject(const std::vector<MeshComponent> &renders, const std::vector<TransformComponent> &transforms);

private:
    void deferredGeometryPass(const std::vector<MeshComponent> &renders, const std::vector<TransformComponent> &transforms, const CameraComponent &camera);
    void deferredLightningPass(const std::vector<TransformComponent>& transforms, const CameraComponent &camera,
                               const std::vector<DirectionalLightComponent>& dirLights = std::vector<DirectionalLightComponent>(),
                               const std::vector<SpotLightComponent>& spotLights = std::vector<SpotLightComponent>(),
                               const std::vector<PointLightComponent>& pointLights = std::vector<PointLightComponent>());
    void directionalLightPass(const std::vector<TransformComponent>& transforms, const CameraComponent &camera, const std::vector<DirectionalLightComponent>& dirLights);
    void pointLightPass(const std::vector<TransformComponent>& transforms,const CameraComponent &camera, const std::vector<PointLightComponent>& pointLights);
    void spotLightPass(const std::vector<TransformComponent>& transforms, const CameraComponent &camera, const std::vector<SpotLightComponent>& spotLights);
signals:
    void initDone();
    void windowUpdated();

public slots:
    void resizeGBuffer();


private:
    QOpenGLContext *mContext{nullptr};

    float mTimeSinceStart{0};

    std::shared_ptr<MeshData> mSkybox;

    unsigned int mScreenSpacedQuadVAO;
    unsigned int mGBuffer, mGPosition{}, mGNormal{}, mGAlbedoSpec{};
    unsigned int mRboDepth{};
    std::shared_ptr<Shader> mDirectionalLightShader;
    std::shared_ptr<Shader> mPointLightShader;
    std::shared_ptr<Shader> mSpotLightShader;

    bool mWireframe{false};

    bool isInitialized{false};

   // MainWindow *mMainWindow{nullptr};    //points back to MainWindow to be able to put info in StatusBar

    class QOpenGLDebugLogger *mOpenGLDebugLogger{nullptr};

    void renderQuad();
    void renderSkybox(const CameraComponent& camera);

    void startOpenGLDebugger();
};

#endif // RENDERER_H
