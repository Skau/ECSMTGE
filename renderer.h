#ifndef RENDERER_H
#define RENDERER_H

#include <QWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <chrono>
#include "texture.h"
#include "camerasystem.h"
#include "componentdata.h"
#include "postprocessor.h"
#include <optional>

class QOpenGLContext;
class Shader;
class QTime;
class ParticleSystem;

/**
 * @brief The render system. Holds onto the context and renders everything when needed.
 */
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

    /**
     * @brief Initializes context, gbuffer and other required items.
     */
    void init();

    void exposeEvent(QExposeEvent *) override;
    void toggleWireframe(bool value);

    void checkForGLerrors();

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
    /**
     * @brief Renders the scene with wireframe enabled.
     */
    void renderGlobalWireframe(std::vector<MeshComponent>& renders, const std::vector<TransformComponent> &transforms, const CameraComponent &camera);
    /**
     * @brief Render the scene as normal.
     */
    void renderDeferred(std::vector<MeshComponent>& renders, const std::vector<TransformComponent> &transforms, const CameraComponent &camera,
                        const std::vector<DirectionalLightComponent>& dirLights = std::vector<DirectionalLightComponent>(),
                        const std::vector<SpotLightComponent>& spotLights = std::vector<SpotLightComponent>(),
                        const std::vector<PointLightComponent>& pointLights = std::vector<PointLightComponent>());
    /**
     * @brief First pass of the deferred pipeline.
     */
    int geometryPass(std::vector<MeshComponent>& renders, const std::vector<TransformComponent>& transforms, const CameraComponent &camera,
                      ShaderType renderMode = ShaderType::Deferred, std::optional<Material> overrideMaterial = std::nullopt);
    /**
     * @brief Part of the lighting pass of the deferred pipeline.
     */
    void deferredLightningPass(const std::vector<TransformComponent>& transforms, const CameraComponent &camera,
                               const std::vector<DirectionalLightComponent>& dirLights = std::vector<DirectionalLightComponent>(),
                               const std::vector<SpotLightComponent>& spotLights = std::vector<SpotLightComponent>(),
                               const std::vector<PointLightComponent>& pointLights = std::vector<PointLightComponent>());
    /**
     * @brief Part of the lighting pass of the deferred pipeline.
     */
    void directionalLightPass(const std::vector<TransformComponent>& transforms, const CameraComponent &camera, const std::vector<DirectionalLightComponent>& dirLights);
    /**
     * @brief Part of the lighting pass of the deferred pipeline.
     */
    void pointLightPass(const std::vector<TransformComponent>& transforms,const CameraComponent &camera, const std::vector<PointLightComponent>& pointLights);
    /**
     * @brief Part of the lighting pass of the deferred pipeline.
     */
    void spotLightPass(const std::vector<TransformComponent>& transforms, const CameraComponent &camera, const std::vector<SpotLightComponent>& spotLights);
    /**
     * @brief Last part of the deferred pipeline. Renders a quad on top based on the post processors.
     */
    void renderPostprocessing();

    std::vector<gsl::vec4> calcViewFrustum(const CameraComponent &camera, const TransformComponent &trans);
    bool insideViewingArea(const MeshComponent& render, const TransformComponent& trans, const CameraComponent& camera);
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
