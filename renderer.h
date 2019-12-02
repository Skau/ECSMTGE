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
#include "postprocessor.h"
#include <optional>

class QOpenGLContext;
class Shader;
class Light;
class QTime;
class ParticleSystem;

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

    EntityInfo* EditorCurrentEntitySelected{nullptr};

    QOpenGLContext* getContext() { return mContext; }

    void init();

    void exposeEvent(QExposeEvent *) override;
    void toggleWireframe(bool value);

    void checkForGLerrors();

    // Should only need renders, materials and transforms
    void render(std::vector<MeshComponent>& renders, const std::vector<TransformComponent> &transforms, const CameraComponent &camera,
                        const std::vector<DirectionalLightComponent>& dirLights = std::vector<DirectionalLightComponent>(),
                        const std::vector<SpotLightComponent>& spotLights = std::vector<SpotLightComponent>(),
                        const std::vector<PointLightComponent>& pointLights = std::vector<PointLightComponent>(),
                        const std::vector<ParticleComponent>& particles = std::vector<ParticleComponent>{});

    /** Renders a picture to the screen and uses the pixel locations to figure out what objects the mouse i hovering over.
     * The process is a accurate, but slow process and therefore should never be used in runtime; only in the editor.
     */
    unsigned int getMouseHoverObject(gsl::ivec2 mouseScreenPos, const std::vector<MeshComponent> &renders, const std::vector<TransformComponent> &transforms,
                                     const CameraComponent& camera);

    int getNumberOfVerticesDrawn() { return mNumberOfVerticesDrawn; }

    void evaluateParams(Material& material);

private:
    void renderReset();
    void renderGlobalWireframe(std::vector<MeshComponent>& renders, const std::vector<TransformComponent> &transforms, const CameraComponent &camera);
    void renderDeferred(std::vector<MeshComponent>& renders, const std::vector<TransformComponent> &transforms, const CameraComponent &camera,
                        const std::vector<DirectionalLightComponent>& dirLights = std::vector<DirectionalLightComponent>(),
                        const std::vector<SpotLightComponent>& spotLights = std::vector<SpotLightComponent>(),
                        const std::vector<PointLightComponent>& pointLights = std::vector<PointLightComponent>());
    int geometryPass(std::vector<MeshComponent>& renders, const std::vector<TransformComponent>& transforms, const CameraComponent &camera,
                      ShaderType renderMode = ShaderType::Deferred, std::optional<Material> overrideMaterial = std::nullopt);
    void deferredLightningPass(const std::vector<TransformComponent>& transforms, const CameraComponent &camera,
                               const std::vector<DirectionalLightComponent>& dirLights = std::vector<DirectionalLightComponent>(),
                               const std::vector<SpotLightComponent>& spotLights = std::vector<SpotLightComponent>(),
                               const std::vector<PointLightComponent>& pointLights = std::vector<PointLightComponent>());
    void directionalLightPass(const std::vector<TransformComponent>& transforms, const CameraComponent &camera, const std::vector<DirectionalLightComponent>& dirLights);
    void pointLightPass(const std::vector<TransformComponent>& transforms,const CameraComponent &camera, const std::vector<PointLightComponent>& pointLights);
    void spotLightPass(const std::vector<TransformComponent>& transforms, const CameraComponent &camera, const std::vector<SpotLightComponent>& spotLights);
    void renderPostprocessing();
signals:
    void initDone();
    void windowUpdated();

private:
    QOpenGLContext *mContext{nullptr};

    float mTimeSinceStart{0};

    std::unique_ptr<ParticleSystem> mParticleSystem;

    std::unique_ptr<Postprocessor> mPostprocessor;
    std::unique_ptr<Postprocessor> mOutlineeffect;
    std::unique_ptr<Postprocessor> mBloomEffect;

    std::shared_ptr<MeshData> mSkyboxMesh;
    std::shared_ptr<Material> mSkyboxMaterial;
    std::shared_ptr<MeshData> mAxisMesh;
    std::shared_ptr<Material> mAxisMaterial;

    GLuint mScreenSpacedQuadVAO;
    GLuint mGBuffer, mGPosition{}, mGNormal{}, mGAlbedoSpec{}, mRboDepth{};
    std::shared_ptr<Shader> mDirectionalLightShader;
    std::shared_ptr<Shader> mPointLightShader;
    std::shared_ptr<Shader> mSpotLightShader;

    bool mGlobalWireframe{false};
    bool isInitialized{false};
    bool mDepthStencilAttachmentSupported{true};

    int mNumberOfVerticesDrawn{0};

    class QOpenGLDebugLogger *mOpenGLDebugLogger{nullptr};

    float distanceFromCamera(const CameraComponent& camera, const TransformComponent& transform);
    void resizeGBuffer(double retinaScale = 1.0);
    void renderQuad();
    void renderSkybox(const CameraComponent& camera);
    void renderAxis(const CameraComponent& camera);
    void drawEditorOutline();

    void startOpenGLDebugger();
    void initGBuffer();
};

#endif // RENDERER_H
