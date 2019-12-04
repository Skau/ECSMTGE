#include "innpch.h"
#include "renderer.h"
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLDebugLogger>

#include "inputhandler.h"
#include "resourcemanager.h"
#include "texture.h"
#include "particlesystem.h"

#include "Instrumentor.h"

Renderer::Renderer()
{
    PROFILE_FUNCTION();
    QSurfaceFormat format;
    format.setVersion(4, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setOption(QSurfaceFormat::DebugContext);
    format.setDepthBufferSize(24);
    format.setSamples(8);
    format.setSwapInterval(0);

    qDebug() << "Requesting surface format: " << format;

    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);
    mContext = new QOpenGLContext(this);
    mContext->setFormat(requestedFormat());
    if (!mContext->create()) {
        delete mContext;
        mContext = nullptr;
        qDebug() << "Context could not be made - quitting this application";
    }

    mPostprocessor = std::make_unique<Postprocessor>(this, "main");
    mOutlineeffect = std::make_unique<Postprocessor>(this, "outlineeffect");
    mBloomEffect = std::make_unique<Postprocessor>(this, "bloom");
}

Renderer::~Renderer()
{
}

// Called when first exposed
void Renderer::init()
{
    PROFILE_FUNCTION();
    //The OpenGL context has to be set.
    //The context belongs to the instanse of this class!
    if (!mContext->makeCurrent(this)) {
        qDebug() << "makeCurrent() failed";
        return;
    }

    //must call this to use OpenGL functions
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);    //enables depth sorting - must use GL_DEPTH_BUFFER_BIT in glClear
    glEnable(GL_CULL_FACE);     //draws only front side of models - usually what you want

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //Print render version info:
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;

    startOpenGLDebugger();

    // Setup deferred shading textures
    initGBuffer();

    // Create renderquad
    float quadVertices[] = {
        //    positions   texture Coords
        -1.0f,  -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.f, 1.0f,
    };
    // plane VAO setup
    GLuint mQuadVBO;
    glGenVertexArrays(1, &mScreenSpacedQuadVAO);
    glGenBuffers(1, &mQuadVBO);
    glBindVertexArray(mScreenSpacedQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // Init postprocessor
    mPostprocessor->init();
    if (mDepthStencilAttachmentSupported)
        mOutlineeffect->init();
    mBloomEffect->init();

    // Init particlesystem
    mParticleSystem = std::make_unique<ParticleSystem>();

    // Called to tell App that it can continue initializing
    initDone();
}

void Renderer::initGBuffer()
{
    PROFILE_FUNCTION();
    bool framebufferComplete = false;
    bool buffersInitialized = false;

    do {
        if (buffersInitialized)
        {
            glDeleteRenderbuffers(1, &mRboDepth);
            glDeleteTextures(1, &mGAlbedoSpec);
            glDeleteTextures(1, &mGNormal);
            glDeleteTextures(1, &mGPosition);
            glDeleteFramebuffers(1, &mGBuffer);
        }

        glGenFramebuffers(1, &mGBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer);

        // - position color buffer
        glGenTextures(1, &mGPosition);
        glBindTexture(GL_TEXTURE_2D, mGPosition);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mGPosition, 0);

        // - normal color buffer
        glGenTextures(1, &mGNormal);
        glBindTexture(GL_TEXTURE_2D, mGNormal);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mGNormal, 0);

        // - color + specular color buffer
        glGenTextures(1, &mGAlbedoSpec);
        glBindTexture(GL_TEXTURE_2D, mGAlbedoSpec);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mGAlbedoSpec, 0);

        // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
        GLenum attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);

        // create and attach depth and stencil buffer (renderbuffer)
        glGenRenderbuffers(1, &mRboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, mRboDepth);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, mDepthStencilAttachmentSupported ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRboDepth);

        // Set sizes and storage types of gBuffer
        resizeGBuffer();

        buffersInitialized = true;

        // finally check if framebuffer is complete
        switch (glCheckFramebufferStatus(GL_FRAMEBUFFER))
        {
            case GL_FRAMEBUFFER_UNDEFINED:
            qDebug() << "Framebuffer error: GL_FRAMEBUFFER_UNDEFINED";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            qDebug() << "Framebuffer error: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n"
                     << "GL_DEPTH_STENCIL_ATTACHMENT is probably not supported. Attempting to recreate framebuffer.";
            mDepthStencilAttachmentSupported = false;
            framebufferComplete = false;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            qDebug() << "Framebuffer error: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            qDebug() << "Framebuffer error: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            qDebug() << "Framebuffer error: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            qDebug() << "Framebuffer error: GL_FRAMEBUFFER_UNSUPPORTED";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            qDebug() << "Framebuffer error: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            qDebug() << "Framebuffer error: GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
            break;
        case GL_FRAMEBUFFER_COMPLETE:
            // Framebuffer is complete
            framebufferComplete = true;
            break;

        }

        if (!framebufferComplete)
            qDebug() << "Framebuffer didn't get created. Attempting to recreate.";
    } while (!framebufferComplete);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::render(std::vector<MeshComponent>& renders, const std::vector<TransformComponent>& transforms, const CameraComponent& camera,
                      const std::vector<DirectionalLightComponent>& dirLights, const std::vector<SpotLightComponent>& spotLights,
                      const std::vector<PointLightComponent>& pointLights, const std::vector<ParticleComponent>& particles)
{
    if(isExposed() && mContext->makeCurrent(this))
    {
        renderReset();

        if (mGlobalWireframe)
            renderGlobalWireframe(renders, transforms, camera);
        else
            renderDeferred(renders, transforms, camera, dirLights, spotLights, pointLights);

        mParticleSystem->updateParticles(camera, transforms, particles, mTimeSinceStart);

        renderPostprocessing();

        checkForGLerrors();
        mContext->swapBuffers(this);
    }
}

void Renderer::renderGlobalWireframe(std::vector<MeshComponent>& renders, const std::vector<TransformComponent>& transforms, const CameraComponent& camera)
{
    PROFILE_FUNCTION();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisable(GL_CULL_FACE);

    mNumberOfVerticesDrawn = geometryPass(renders, transforms, camera, ShaderType::Forward,
                 std::make_optional<Material>(ResourceManager::instance().getShader("singleColor"),
                                              std::map<std::string, ShaderParamType>{    {"p_color", gsl::vec3{1.f, 1.f, 1.f}}     }));

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
}

void Renderer::renderReset()
{
    PROFILE_FUNCTION();
    // Setup
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glStencilMask(0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glViewport(0, 0, static_cast<int>(width() * devicePixelRatio()), static_cast<int>(height() * devicePixelRatio()));

    // Clear postprocessor from last frame and bind to it
    mPostprocessor->clear();

    glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer);
    glEnable(GL_STENCIL_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, mPostprocessor->input());
}

void Renderer::renderDeferred(std::vector<MeshComponent>& renders, const std::vector<TransformComponent>& transforms,
                              const CameraComponent& camera, const std::vector<DirectionalLightComponent>& dirLights,
                              const std::vector<SpotLightComponent>& spotLights, const std::vector<PointLightComponent>& pointLights)
{
    PROFILE_FUNCTION();
    gsl::ivec2 scrSize{static_cast<int>(width() * devicePixelRatio()), static_cast<int>(height() * devicePixelRatio())};

    glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer);
    mNumberOfVerticesDrawn = geometryPass(renders, transforms, camera, ShaderType::Deferred);
    glBindFramebuffer(GL_FRAMEBUFFER, mPostprocessor->input());


    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mGPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mGNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mGAlbedoSpec);

    deferredLightningPass(transforms, camera, dirLights, spotLights, pointLights);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    // copy content of geometry's depth buffer to default framebuffer's depth buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBuffer);

    // blit to postprocessor framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mPostprocessor->input());
    glBlitFramebuffer(0, 0, scrSize.x, scrSize.y, 0, 0, scrSize.x, scrSize.y, GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, mPostprocessor->input());



    // ** Forward shading ** //
    /// Draw foward here

    mNumberOfVerticesDrawn += geometryPass(renders, transforms, camera, ShaderType::Forward);
    // Skybox
    renderSkybox(camera);

    // Axis
    // Need to disable depth testing, or else the axis will sometimes appear behind other meshes
    glDisable(GL_DEPTH_TEST);
    renderAxis(camera);
}

int Renderer::geometryPass(std::vector<MeshComponent>& renders, const std::vector<TransformComponent> &transforms, const CameraComponent &camera,
                            ShaderType renderMode, std::optional<Material> overrideMaterial)
{
    PROFILE_FUNCTION();
    auto transIt = transforms.begin();
    auto renderIt = renders.begin();
    gsl::vec3 camPos{};
    auto camTransPos = EntityManager::find(transforms.begin(), transforms.end(), camera.entityId);
    if (camTransPos != transforms.end())
        camPos = camTransPos->position;
    else
        return 0;

    bool transformShortest = transforms.size() < renders.size();

    bool _{true};

    GLuint currentlySelectedEID = (EditorCurrentEntitySelected != nullptr) ? EditorCurrentEntitySelected->entityId : 0;

    // ** Geometry pass ** //

    checkForGLerrors();

    auto verticesDrawn = 0;

    // cause normal while (true) loops are so outdated
    for ( ;_; )
    {
        if (transformShortest)
        {
            if (transIt == transforms.end())
                break;
        }
        else
        {
            if (renderIt == renders.end())
                break;
        }

        // Increment lowest index
        if (!transIt->valid || transIt->entityId < renderIt->entityId)
        {
            ++transIt;
        }
        else if (!renderIt->valid || renderIt->entityId < transIt->entityId)
        {
            ++renderIt;
        }
        else
        {
            // They are the same
            if(!renderIt->isVisible)
            {
                // Increment all
                ++transIt;
                ++renderIt;
                continue;
            }

            Material material{renderIt->mMaterial};
            if (!overrideMaterial)
            {
                if(!material.mShader)
                {
                    material.mShader = renderMode == ShaderType::Deferred ?
                                ResourceManager::instance().getShader("phong") :
                                ResourceManager::instance().getShader("singleColor");
                    renderIt->mMaterial.loadShaderWithParameters(material.mShader);
                    material = renderIt->mMaterial;
                }

                if (material.mShader->mRenderingType != renderMode)
                {
                    // Increment all
                    ++transIt;
                    ++renderIt;
                    continue;
                }
            }
            else
            {
                material = overrideMaterial.value();
            }

            float distance = (camPos - transIt->position).length();

            unsigned index = 0;
            if(distance > 70.f)
            {
                index = 2;
            }
            else if(distance > 20.f)
            {
                index = 1;
            }

            // Mesh data available
            auto& meshData = renderIt->meshData;
            if(!meshData.mVerticesCounts[index])
            {
                // Increment all
                ++transIt;
                ++renderIt;
                continue;
            }

            // Entity can be drawn. Draw.

            glBindVertexArray(meshData.mVAOs[index]);

            glUseProgram(material.mShader->getProgram());

            evaluateParams(material);

            if(material.mTextures.size())
            {
                for(unsigned i = 0; i < material.mTextures.size(); ++i)
                {
                    glActiveTexture(GL_TEXTURE0 + i);
                    glBindTexture(material.mTextures[i].second, material.mTextures[i].first);
                    glUniform1i(glGetUniformLocation(material.mShader->getProgram(), "textureSampler"), static_cast<int>(material.mTextures[i].first));
                }
            }

            auto mMatrix = gsl::mat4::modelMatrix(transIt->position, transIt->rotation, transIt->scale);
            glUniformMatrix4fv(glGetUniformLocation(material.mShader->getProgram(), "mMatrix"), 1, true, mMatrix.constData());
            glUniformMatrix4fv(glGetUniformLocation(material.mShader->getProgram(), "vMatrix"), 1, true, camera.viewMatrix.constData());
            glUniformMatrix4fv(glGetUniformLocation(material.mShader->getProgram(), "pMatrix"), 1, true, camera.projectionMatrix.constData());

            // If selected in editor, change it's stencil value
            if (currentlySelectedEID == renderIt->entityId)
                glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

            verticesDrawn += meshData.mVerticesCounts[index];

            if(meshData.mIndicesCounts[index] > 0)
            {
                glDrawElements(meshData.mRenderType, static_cast<GLsizei>(meshData.mIndicesCounts[index]), GL_UNSIGNED_INT, nullptr);
            }
            else
            {
                glDrawArrays(meshData.mRenderType, 0, static_cast<GLsizei>(meshData.mVerticesCounts[index]));
            }

            // Remember to change back so others won't get changed.
            if (currentlySelectedEID == renderIt->entityId)
                glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

            // Increment all
            ++transIt;
            ++renderIt;
        }
    }
    checkForGLerrors();
    return verticesDrawn;
}

void Renderer::deferredLightningPass(const std::vector<TransformComponent> &transforms, const CameraComponent &camera,
                                     const std::vector<DirectionalLightComponent>& dirLights,
                                     const std::vector<SpotLightComponent>& spotLights,
                                     const std::vector<PointLightComponent>& pointLights)
{
    PROFILE_FUNCTION();
    if(dirLights.size())
    {
        if(mDirectionalLightShader == nullptr)
        {
            mDirectionalLightShader = ResourceManager::instance().getShader("directionalLight");
        }
        directionalLightPass(transforms, camera, dirLights);
    }


    if(spotLights.size())
    {
        if(mSpotLightShader == nullptr)
        {
            mSpotLightShader = ResourceManager::instance().getShader("spotLight");
        }
        spotLightPass(transforms, camera, spotLights);
    }

    if(pointLights.size())
    {
        if(mPointLightShader == nullptr)
        {
            mPointLightShader = ResourceManager::instance().getShader("pointLight");
        }
        pointLightPass(transforms, camera, pointLights);
    }
}

void Renderer::directionalLightPass(const std::vector<TransformComponent> &transforms, const CameraComponent& camera, const std::vector<DirectionalLightComponent> &dirLights)
{
    PROFILE_FUNCTION();
    auto v = camera.viewMatrix;
    v.inverse();
    auto pos = gsl::vec3(v.at(0, 3), v.at(1, 3), v.at(2, 3));
    auto location = mDirectionalLightShader->getProgram();
    glUseProgram(location);
    glUniform1i(glGetUniformLocation(location, "gPosition"),   0);
    glUniform1i(glGetUniformLocation(location, "gNormal"),     1);
    glUniform1i(glGetUniformLocation(location, "gAlbedoSpec"), 2);
    glUniform3fv(glGetUniformLocation(location, "viewPos"), 1, pos.xP());

    auto transIt = transforms.begin();
    auto lightIt = dirLights.begin();

    bool transformShortest = transforms.size() < dirLights.size();

    bool _{true};

    // cause normal while (true) loops are so outdated
    for ( ;_; )
    {
        if (transformShortest)
        {
            if (transIt == transforms.end())
                break;
        }
        else
        {
            if (lightIt == dirLights.end())
                break;
        }

        // Increment lowest index
        if (!transIt->valid || transIt->entityId < lightIt->entityId)
        {
            ++transIt;
        }
        else if (!lightIt->valid || lightIt->entityId < transIt->entityId)
        {
            ++lightIt;
        }
        else
        {
            glUniform3fv(glGetUniformLocation(location, "light.Color"), 1, lightIt->color.xP());
            glUniform3fv(glGetUniformLocation(location, "light.Direction"), 1, transIt->rotation.forwardVector().xP());
            renderQuad();

            // Increment all
            ++transIt;
            ++lightIt;
        }
    }
}

void Renderer::pointLightPass(const std::vector<TransformComponent> &transforms,const CameraComponent& camera, const std::vector<PointLightComponent> &pointLights)
{
    PROFILE_FUNCTION();
    auto v = camera.viewMatrix;
    v.inverse();
    auto pos = gsl::vec3(v.at(0, 3), v.at(1, 3), v.at(2, 3));
    auto location = mPointLightShader->getProgram();
    glUseProgram(location);
    glUniform1i(glGetUniformLocation(location, "gPosition"),   0);
    glUniform1i(glGetUniformLocation(location, "gNormal"),     1);
    glUniform1i(glGetUniformLocation(location, "gAlbedoSpec"), 2);
    glUniform3fv(glGetUniformLocation(location, "viewPos"), 1, pos.xP());

    auto transIt = transforms.begin();
    auto lightIt = pointLights.begin();

    bool transformShortest = transforms.size() < pointLights.size();

    bool _{true};

    // cause normal while (true) loops are so outdated
    for ( ;_; )
    {
        if (transformShortest)
        {
            if (transIt == transforms.end())
                break;
        }
        else
        {
            if (lightIt == pointLights.end())
                break;
        }

        // Increment lowest index
        if (!transIt->valid || transIt->entityId < lightIt->entityId)
        {
            ++transIt;
        }
        else if (!lightIt->valid || lightIt->entityId < transIt->entityId)
        {
            ++lightIt;
        }
        else
        {
            glUniform3fv(glGetUniformLocation(location, "light.Color"), 1, lightIt->color.xP());
            glUniform3fv(glGetUniformLocation(location, "light.Position"), 1, transIt->position.xP());
            glUniform1f(glGetUniformLocation(location, "light.Radius"), lightIt->radius);
            glUniform1f(glGetUniformLocation(location, "light.Intensity"), lightIt->intensity);
            renderQuad();

            // Increment all
            ++transIt;
            ++lightIt;
        }
    }
}

void Renderer::spotLightPass(const std::vector<TransformComponent> &transforms, const CameraComponent& camera, const std::vector<SpotLightComponent> &spotLights)
{
    PROFILE_FUNCTION();
    auto v = camera.viewMatrix;
    v.inverse();
    auto pos = gsl::vec3(v.at(0, 3), v.at(1, 3), v.at(2, 3));
    auto location = mSpotLightShader->getProgram();
    glUseProgram(location);
    glUniform1i(glGetUniformLocation(location, "gPosition"),   0);
    glUniform1i(glGetUniformLocation(location, "gNormal"),     1);
    glUniform1i(glGetUniformLocation(location, "gAlbedoSpec"), 2);
    glUniform3fv(glGetUniformLocation(location, "viewPos"), 1, pos.xP());

    auto transIt = transforms.begin();
    auto lightIt = spotLights.begin();

    bool transformShortest = transforms.size() < spotLights.size();

    bool _{true};

    // cause normal while (true) loops are so outdated
    for ( ;_; )
    {
        if (transformShortest)
        {
            if (transIt == transforms.end())
                break;
        }
        else
        {
            if (lightIt == spotLights.end())
                break;
        }

        // Increment lowest index
        if (!transIt->valid || transIt->entityId < lightIt->entityId)
        {
            ++transIt;
        }
        else if (!lightIt->valid || lightIt->entityId < transIt->entityId)
        {
            ++lightIt;
        }
        else
        {
            glUniform3fv(glGetUniformLocation(location, "light.Color"), 1, lightIt->color.xP());
            glUniform3fv(glGetUniformLocation(location, "light.Position"), 1, transIt->position.xP());
            glUniform3fv(glGetUniformLocation(location, "light.Direction"), 1, transIt->rotation.forwardVector().xP());
            glUniform1f(glGetUniformLocation(location, "light.CutOff"), lightIt->cutOff);
            glUniform1f(glGetUniformLocation(location, "light.OuterCutOff"), lightIt->outerCutOff);
            glUniform1f(glGetUniformLocation(location, "light.constant"), lightIt->constant);
            glUniform1f(glGetUniformLocation(location, "light.linear"), lightIt->linear);
            glUniform1f(glGetUniformLocation(location, "light.quadratic"), lightIt->quadratic);
            renderQuad();

            // Increment all
            ++transIt;
            ++lightIt;
        }
    }
}

void Renderer::renderPostprocessing()
{
    PROFILE_FUNCTION();
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mPostprocessor->input());

    auto retinaScale = devicePixelRatio();

    if (mDepthStencilAttachmentSupported)
    {
        // Outline effect needs stencil buffer
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mOutlineeffect->input());
        glBlitFramebuffer(0, 0, static_cast<int>(width() * retinaScale), static_cast<int>(height() * retinaScale),
                          0, 0, static_cast<int>(width() * retinaScale), static_cast<int>(height() * retinaScale), GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mBloomEffect->input());
    glViewport(0, 0, static_cast<int>(mBloomEffect->width() * retinaScale), static_cast<int>(mBloomEffect->height() * retinaScale));
    glBlitFramebuffer(0, 0, static_cast<int>(width() * retinaScale), static_cast<int>(height() * retinaScale),
                      0, 0, static_cast<int>(mBloomEffect->width() * retinaScale), static_cast<int>(mBloomEffect->height() * retinaScale),
                      GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, mPostprocessor->input());

    // Postprocessing
    glDisable(GL_DEPTH_TEST);

    mBloomEffect->Render();
    glViewport(0, 0, static_cast<int>(width() * retinaScale), static_cast<int>(height() * retinaScale));
    *mPostprocessor += *mBloomEffect;
    if (mDepthStencilAttachmentSupported)
        drawEditorOutline();

    // Note: Remember to do main postprocessor last because this also deals with tone mapping
    mPostprocessor->Render();

    glEnable(GL_DEPTH_TEST);
}

std::vector<gsl::vec4> Renderer::calcViewFrustum(const CameraComponent &camera, const TransformComponent &trans)
{
    auto invViewMat = gsl::mat4::viewMatrixInv(gsl::quat::lookAt(gsl::deg2radf(camera.pitch), gsl::deg2radf(camera.yaw)), trans.position) *
            camera.invProjectionMatrix;
    return {
        invViewMat * gsl::vec4{-1.f, -1.f, 1.f, 1.f},
        invViewMat * gsl::vec4{1.f, -1.f, 1.f, 1.f},
        invViewMat * gsl::vec4{-1.f, -1.f, -1.f, 1.f},
        invViewMat * gsl::vec4{1.f, -1.f, -1.f, 1.f}
    };
}

bool Renderer::insideViewingArea(const MeshComponent &render, const TransformComponent &trans, const CameraComponent &camera)
{

}

float Renderer::distanceFromCamera(const CameraComponent& camera, const TransformComponent& transform)
{
    PROFILE_FUNCTION();
    return std::abs((camera.viewMatrix.getPosition() - transform.position).length());
}

unsigned int Renderer::getMouseHoverObject(gsl::ivec2 mouseScreenPos, const std::vector<MeshComponent> &renders, const std::vector<TransformComponent> &transforms,
                                           const CameraComponent &camera)
{
    PROFILE_FUNCTION();
    if(isExposed() && mouseScreenPos.x < width() && mouseScreenPos.y < height())
    {
        mContext->makeCurrent(this);

        // Using the postprocessor input framebuffer as an offscreen render.
        mPostprocessor->clear();

        auto shader = ResourceManager::instance().getShader("mousepicking");
        if(!shader)
        {
            std::cout << "mouse hover object function failed because it could'nt find mousepicking shader!" << std::endl;
            return 0;
        }

        glUseProgram(shader->getProgram());


        auto transIt = transforms.begin();
        auto renderIt = renders.begin();

        bool transformShortest = transforms.size() < renders.size();

        bool _{true};

        // cause normal while (true) loops are so outdated
        for ( ;_; )
        {
            if (transformShortest)
            {
                if (transIt == transforms.end())
                    break;
            }
            else
            {
                if (renderIt == renders.end())
                    break;
            }

            // Increment lowest index
            if (!transIt->valid || transIt->entityId < renderIt->entityId)
            {
                ++transIt;
            }
            else if (!renderIt->valid || renderIt->entityId < transIt->entityId)
            {
                ++renderIt;
            }
            else
            {
                // They are the same
                if(!renderIt->isVisible)
                {
                    // Increment all
                    ++transIt;
                    ++renderIt;
                    continue;
                }

                auto camPos = camera.viewMatrix.getPosition();
                auto distance = std::abs((camPos - transIt->position).length());
                unsigned index = 0;
                if(distance > 10.f)
                {
                    index = 1;
                }
                else if(distance > 20.f)
                {
                    index = 2;
                }

                // Mesh data available
                auto meshData = renderIt->meshData;
                if(!meshData.mVerticesCounts[index])
                {
                    // Increment all
                    ++transIt;
                    ++renderIt;
                    continue;
                }

                // Entity can be drawn. Draw.

                glBindVertexArray(meshData.mVAOs[index]);

                auto mMatrix = gsl::mat4::modelMatrix(transIt->position, transIt->rotation, transIt->scale);
                auto MVP = camera.projectionMatrix * camera.viewMatrix * mMatrix;
                glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "MVP"), 1, true, MVP.constData());


                // Convert the entity ID, into an RGB color
                int r = (transIt->entityId & 0x000000FF) >>  0;
                int g = (transIt->entityId & 0x0000FF00) >>  8;
                int b = (transIt->entityId & 0x00FF0000) >> 16;
                gsl::vec3 color{ r / 255.f, g / 255.f, b / 255.f };
                glUniform3fv(glGetUniformLocation(shader->getProgram(), "idColor"), 1, color.xP());


                if(meshData.mIndicesCounts[index] > 0)
                {
                    glDrawElements(meshData.mRenderType, static_cast<GLsizei>(meshData.mIndicesCounts[index]), GL_UNSIGNED_INT, nullptr);
                }
                else
                {
                    glDrawArrays(meshData.mRenderType, 0, static_cast<GLsizei>(meshData.mVerticesCounts[index]));
                }

                // Increment all
                ++transIt;
                ++renderIt;
            }
        }

        checkForGLerrors();


        auto retinaScale = devicePixelRatio();
        // Read color value from framebuffer
        unsigned char data[4];
        glReadPixels(static_cast<int>(mouseScreenPos.x * retinaScale), static_cast<int>((height() - mouseScreenPos.y) * retinaScale), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

        unsigned int returnedId = data[0] + data[1] * 256 + data[2] * 256 * 256;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return returnedId;
    }

    return 0;
}

void Renderer::resizeGBuffer(double retinaScale)
{
    PROFILE_FUNCTION();
    gsl::ivec2 scrSize{static_cast<int>(width() * retinaScale), static_cast<int>(height() * retinaScale)};

    glBindTexture(GL_TEXTURE_2D, mGPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, scrSize.x, scrSize.y, 0, GL_RGB, GL_FLOAT, nullptr);

    glBindTexture(GL_TEXTURE_2D, mGNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, scrSize.x, scrSize.y, 0, GL_RGB, GL_FLOAT, nullptr);

    glBindTexture(GL_TEXTURE_2D, mGAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, scrSize.x, scrSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, mRboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, scrSize.x, scrSize.y);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Renderer::evaluateParams(Material& material)
{
    PROFILE_FUNCTION();
    if(auto shader = material.mShader)
    {
        auto params = material.mParameters;
        if(params.size())
        {
            for (auto it = params.begin(); it != params.end(); ++it)
            {
                auto name = it->first;
                GLint uniform = glGetUniformLocation(shader->getProgram(), name.c_str());
                if (uniform < 0)
                    continue;

                try {
                    if(std::holds_alternative<bool>(it->second))
                        glUniform1i(uniform, std::get<bool>(it->second));
                    else if (std::holds_alternative<int>(it->second))
                        glUniform1i(uniform, std::get<int>(it->second));
                    else if (std::holds_alternative<float>(it->second))
                        glUniform1f(uniform, std::get<float>(it->second));
                    else if (std::holds_alternative<gsl::vec2>(it->second))
                        glUniform2fv(uniform, 1, std::get<gsl::vec2>(it->second).data());
                    else if (std::holds_alternative<gsl::vec3>(it->second))
                        glUniform3fv(uniform, 1, std::get<gsl::vec3>(it->second).data());
                    else
                        glUniform4fv(uniform, 1, std::get<gsl::vec4>(it->second).data());
                }
                catch(...)
                {
                    std::cout << "Logical error!" << std::endl;
                    return;
                }
            }
        }
    }
}

void Renderer::renderQuad()
{
    PROFILE_FUNCTION();
    glBindVertexArray(mScreenSpacedQuadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Renderer::renderSkybox(const CameraComponent &camera)
{
    PROFILE_FUNCTION();
    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(mSkyboxMesh->mVAOs[0]);

    auto shader = mSkyboxMaterial->mShader;
    glUseProgram(shader->getProgram());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(mSkyboxMaterial->mTextures[0].second, static_cast<unsigned int>(mSkyboxMaterial->mTextures[0].first));

    glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "vMatrix"), 1, true, camera.viewMatrix.constData());
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "pMatrix"), 1, true, camera.projectionMatrix.constData());
    glUniform1i(glGetUniformLocation(shader->getProgram(), "cubemap"), 0);

    int uniform = glGetUniformLocation(shader->getProgram(), "sTime");
    if (0 <= uniform)
        glUniform1f(uniform, mTimeSinceStart);

    uniform = glGetUniformLocation(shader->getProgram(), "sResolution");
    if (0 <= uniform)
    {
        gsl::ivec2 res{width(), height()};
        glUniform2iv(uniform, 1, &res.x);
    }

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mSkyboxMesh->mVerticesCounts[0]));
    glDepthFunc(GL_LESS);

    checkForGLerrors();
}


void Renderer::renderAxis(const CameraComponent& camera)
{
    PROFILE_FUNCTION();
    glBindVertexArray(mAxisMesh->mVAOs[0]);
    auto shader = mAxisMaterial->mShader;
    glUseProgram(shader->getProgram());

    auto mMatrix = gsl::mat4(1);
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "mMatrix"), 1, true, mMatrix.constData());
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "vMatrix"), 1, true, camera.viewMatrix.constData());
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "pMatrix"), 1, true, camera.projectionMatrix.constData());
    glDrawArrays(mAxisMesh->mRenderType, 0, static_cast<GLsizei>(mAxisMesh->mVerticesCounts[0]));
}

void Renderer::drawEditorOutline()
{
    PROFILE_FUNCTION();
    /* Steps:
     * 1. Draw a color on fragments that matches stencil
     * 2. Blur image
     * 3. Draw only fragments that doesn't cover stencil
     * additive over main image.
     */

    glEnable(GL_STENCIL_TEST);
    // Pass fragments that are 1
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_EQUAL, 1, 0xFF);

    // Custom loop logic
    for (unsigned int i{0}; i < mOutlineeffect->steps.size(); ++i)
    {
        mOutlineeffect->RenderStep(i);
        if (i == 0)
            glStencilFunc(GL_ALWAYS, 1, 0xFF);

    }

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    *mPostprocessor += *mOutlineeffect;
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
}

void Renderer::exposeEvent(QExposeEvent *)
{
    PROFILE_FUNCTION();
    if(!isInitialized)
    {
        init();
        isInitialized = true;
    }

    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, static_cast<GLint>(width() * retinaScale), static_cast<GLint>(height() * retinaScale));
    resizeGBuffer(retinaScale);
    windowUpdated();
}

//Simple way to turn on/off wireframe mode
//Not totally accurate, but draws the objects with
//lines instead of filled polygons
void Renderer::toggleWireframe(bool value)
{
    PROFILE_FUNCTION();
    mGlobalWireframe = value;
}

/// Uses QOpenGLDebugLogger if this is present
/// Reverts to glGetError() if not
void Renderer::checkForGLerrors()
{
    PROFILE_FUNCTION();
    if(mOpenGLDebugLogger)
    {
        const QList<QOpenGLDebugMessage> messages = mOpenGLDebugLogger->loggedMessages();
        for (const QOpenGLDebugMessage &message : messages)
            qDebug() << message;
    }
    else
    {
        GLenum err = GL_NO_ERROR;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            qDebug() << "glGetError returns " << err;
        }
    }
}

/// Tries to start the extended OpenGL debugger that comes with Qt
void Renderer::startOpenGLDebugger()
{
    PROFILE_FUNCTION();
    QOpenGLContext * temp = this->getContext();
    if (temp)
    {
        QSurfaceFormat format = temp->format();
        if (! format.testOption(QSurfaceFormat::DebugContext))
            qDebug() << "This system can not use QOpenGLDebugLogger, so we revert to glGetError()";

        if(temp->hasExtension(QByteArrayLiteral("GL_KHR_debug")))
        {
            qDebug() << "System can log OpenGL errors!";
            mOpenGLDebugLogger = new QOpenGLDebugLogger(this);
            if (mOpenGLDebugLogger->initialize()) // initializes in the current context
                qDebug() << "Started OpenGL debug logger!";
        }

        if(mOpenGLDebugLogger)
            mOpenGLDebugLogger->disableMessages(QOpenGLDebugMessage::APISource, QOpenGLDebugMessage::OtherType, QOpenGLDebugMessage::NotificationSeverity);
    }
}
