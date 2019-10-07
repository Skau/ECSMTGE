#include "innpch.h"
#include "renderer.h"
#include <QTimer>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLDebugLogger>
#include <QKeyEvent>
#include <QStatusBar>
#include <chrono>
#include <QTime>
#include <QCoreApplication>
#include <QThread>

#include "inputhandler.h"

#include "Renderables/xyz.h"
#include "Renderables/octahedronball.h"
#include "Renderables/skybox.h"
#include "Renderables/billboard.h"
#include "Renderables/trianglesurface.h"
#include "Renderables/objmesh.h"
#include "Renderables/light.h"

#include "resourcemanager.h"


Renderer::Renderer()
{
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

    mPostprocessor = std::make_unique<Postprocessor>(this);
}

Renderer::~Renderer()
{
}

// Called when first exposed
void Renderer::init()
{
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
    glGenFramebuffers(1, &mGBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer);

    // - position color buffer
    glGenTextures(1, &mGPosition);
    glBindTexture(GL_TEXTURE_2D, mGPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width(), height(), 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mGPosition, 0);

    // - normal color buffer
    glGenTextures(1, &mGNormal);
    glBindTexture(GL_TEXTURE_2D, mGNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width(), height(), 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mGNormal, 0);

    // - color + specular color buffer
    glGenTextures(1, &mGAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, mGAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width(),  height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mGAlbedoSpec, 0);

    // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &mRboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, mRboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width(), height());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "Framebuffer not complete!";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


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


    // Called to tell App that it can continue initializing
    initDone();
}

void Renderer::render(const std::vector<MeshComponent>& renders, const std::vector<TransformComponent>& transforms, const CameraComponent& camera)
{
    /* Note: For å gjøre dette enda raskere kunne det vært
     * mulig å gjøre at dataArraysene alltid resizer til nærmeste
     * tall delelig på 8, også kan man unwrappe denne loopen til å
     * gjøre 8 parallelle handlinger. Kan mulig gjøre prosessen raskere.
     */

    if(isExposed())
    {
        mContext->makeCurrent(this);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // mCurrentCamera->update(deltaTime);

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

                // Mesh data available
                auto meshData = renderIt->meshData;
                if(!meshData.mVerticesCount)
                {
                    // Increment all
                    ++transIt;
                    ++renderIt;
                    continue;
                }

                // Entity can be drawn. Draw.

                glBindVertexArray(meshData.mVAO);

                auto shader = meshData.mMaterial.mShader;
                if(!shader)
                {
                    shader = ResourceManager::instance()->getShader("color");
                    meshData.mMaterial.mShader = shader;
                }

                glUseProgram(shader->getProgram());

                if(meshData.mMaterial.mShader && meshData.mMaterial.mShader->mName.length())
                {
                    if(meshData.mMaterial.mShader->mName == "texture" && meshData.mMaterial.mTexture > -1)
                    {
                        glActiveTexture(GL_TEXTURE0 + static_cast<GLuint>(meshData.mMaterial.mTexture));
                        glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(meshData.mMaterial.mTexture));

                        glUniform1i(glGetUniformLocation(shader->getProgram(), "textureSampler"), meshData.mMaterial.mTexture);
                    }
                }

                auto mMatrix = gsl::mat4::modelMatrix(transIt->position, transIt->rotation, transIt->scale);

                glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "mMatrix"), 1, true, mMatrix.constData());
                glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "vMatrix"), 1, true, camera.viewMatrix.constData());
                glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "pMatrix"), 1, true, camera.projectionMatrix.constData());

                if(meshData.mIndicesCount > 0)
                {
                    glDrawElements(meshData.mRenderType, static_cast<GLsizei>(meshData.mIndicesCount), GL_UNSIGNED_INT, nullptr);
                }
                else
                {
                    glDrawArrays(meshData.mRenderType, 0, static_cast<GLsizei>(meshData.mVerticesCount));
                }

                // Increment all
                ++transIt;
                ++renderIt;


            }
        }

        renderSkybox(camera);

        checkForGLerrors();

        mContext->swapBuffers(this);
    }
}

void Renderer::renderDeferred(const std::vector<MeshComponent>& renders, const std::vector<TransformComponent>& transforms, const CameraComponent& camera,
                              const std::vector<DirectionalLightComponent>& dirLights,
                              const std::vector<SpotLightComponent>& spotLights,
                              const std::vector<PointLightComponent>& pointLights)
{
    if(isExposed())
    {
        mContext->makeCurrent(this);

        deferredGeometryPass(renders, transforms, camera);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

        // ** Forward shading ** //

        // copy content of geometry's depth buffer to default framebuffer's depth buffer
        glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer

        // blit to default framebuffer
        glBlitFramebuffer(0, 0, width(), height(), 0, 0, width(), height(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Draw foward here

        // Skybox
        renderSkybox(camera);

        checkForGLerrors();

        mContext->swapBuffers(this);
    }
}

unsigned int Renderer::getMouseHoverObject(gsl::ivec2 mouseScreenPos, const std::vector<MeshComponent> &renders, const std::vector<TransformComponent> &transforms,
                                           const CameraComponent &camera)
{
    if(isExposed() && mouseScreenPos.x < width() && mouseScreenPos.y < height())
    {
        mContext->makeCurrent(this);
        mPostprocessor->clear();

        // Using the postprocessor input framebuffer as an offscreen render.
        glBindFramebuffer(GL_FRAMEBUFFER, mPostprocessor->input());

        auto shader = ResourceManager::instance()->getShader("mousepicking");
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

                // Mesh data available
                auto meshData = renderIt->meshData;
                if(!meshData.mVerticesCount)
                {
                    // Increment all
                    ++transIt;
                    ++renderIt;
                    continue;
                }

                // Entity can be drawn. Draw.

                glBindVertexArray(meshData.mVAO);

                auto mMatrix = gsl::mat4::modelMatrix(transIt->position, transIt->rotation, transIt->scale);
                auto MVP = camera.projectionMatrix * camera.viewMatrix * mMatrix;
                glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "MVP"), 1, true, MVP.constData());


                // Convert the entity ID, into an RGB color
                int r = (transIt->entityId & 0x000000FF) >>  0;
                int g = (transIt->entityId & 0x0000FF00) >>  8;
                int b = (transIt->entityId & 0x00FF0000) >> 16;
                gsl::vec3 color{ r / 255.f, g / 255.f, b / 255.f };
                glUniform3fv(glGetUniformLocation(shader->getProgram(), "idColor"), 1, color.xP());


                if(meshData.mIndicesCount > 0)
                {
                    glDrawElements(meshData.mRenderType, static_cast<GLsizei>(meshData.mIndicesCount), GL_UNSIGNED_INT, nullptr);
                }
                else
                {
                    glDrawArrays(meshData.mRenderType, 0, static_cast<GLsizei>(meshData.mVerticesCount));
                }

                // Increment all
                ++transIt;
                ++renderIt;
            }
        }

        checkForGLerrors();


        // Read color value from framebuffer
        unsigned char data[4];
        glReadPixels(mouseScreenPos.x, height() - mouseScreenPos.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

        unsigned int returnedId = data[0] + data[1] * 256 + data[2] * 256 * 256;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return returnedId;
    }

    return 0;
}

void Renderer::deferredGeometryPass(const std::vector<MeshComponent> &renders, const std::vector<TransformComponent> &transforms, const CameraComponent &camera)
{
    auto transIt = transforms.begin();
    auto renderIt = renders.begin();

    bool transformShortest = transforms.size() < renders.size();

    bool _{true};

    // ** Geometry pass ** //
    glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

            // Mesh data available
            auto meshData = renderIt->meshData;
            if(!meshData.mVerticesCount)
            {
                // Increment all
                ++transIt;
                ++renderIt;
                continue;
            }

            // Entity can be drawn. Draw.

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


            glBindVertexArray(meshData.mVAOs[index]);


            // ** NEEDS TO BE A DEFERRED SHADER ** //

            auto shader = meshData.mMaterial.mShader;
            if(!shader)
            {
                shader = ResourceManager::instance()->getShader("defaultDeferred");
                meshData.mMaterial.mShader = shader;
            }

            glUseProgram(shader->getProgram());

            auto mMatrix = gsl::mat4::modelMatrix(transIt->position, transIt->rotation, transIt->scale);
            glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "mMatrix"), 1, true, mMatrix.constData());
            glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "vMatrix"), 1, true, camera.viewMatrix.constData());
            glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "pMatrix"), 1, true, camera.projectionMatrix.constData());

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
}

void Renderer::deferredLightningPass(const std::vector<TransformComponent> &transforms, const CameraComponent &camera,
                                     const std::vector<DirectionalLightComponent>& dirLights,
                                     const std::vector<SpotLightComponent>& spotLights,
                                     const std::vector<PointLightComponent>& pointLights)
{
    if(dirLights.size())
    {
        if(mDirectionalLightShader == nullptr)
        {
            mDirectionalLightShader = ResourceManager::instance()->getShader("directionalLight");
        }
        directionalLightPass(transforms, camera, dirLights);
    }


    if(spotLights.size())
    {
        if(mSpotLightShader == nullptr)
        {
            mSpotLightShader = ResourceManager::instance()->getShader("spotLight");
        }
        spotLightPass(transforms, camera, spotLights);
    }

    if(pointLights.size())
    {
        if(mPointLightShader == nullptr)
        {
            mPointLightShader = ResourceManager::instance()->getShader("pointLight");
        }
        pointLightPass(transforms, camera, pointLights);
    }
}

void Renderer::directionalLightPass(const std::vector<TransformComponent> &transforms, const CameraComponent& camera, const std::vector<DirectionalLightComponent> &dirLights)
{
    auto v = camera.viewMatrix;
    v.inverse();
    auto pos = gsl::vec3(v.at(0, 3), v.at(1, 3), v.at(2, 3));
    auto location = mDirectionalLightShader->getProgram();
    glUseProgram(location);
    glUniform1i(glGetUniformLocation(location, "gPosition"),   0);
    glUniform1i(glGetUniformLocation(location, "gNormal"),     1);
    glUniform1i(glGetUniformLocation(location, "gAlbedoSpec"), 2);

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
            glUniform3fv(glGetUniformLocation(location, "viewPos"), 1, pos.xP());
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
    auto v = camera.viewMatrix;
    v.inverse();
    auto pos = gsl::vec3(v.at(0, 3), v.at(1, 3), v.at(2, 3));
    auto location = mPointLightShader->getProgram();
    glUseProgram(location);
    glUniform1i(glGetUniformLocation(location, "gPosition"),   0);
    glUniform1i(glGetUniformLocation(location, "gNormal"),     1);
    glUniform1i(glGetUniformLocation(location, "gAlbedoSpec"), 2);

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
            glUniform3fv(glGetUniformLocation(location, "viewPos"), 1, pos.xP());
            glUniform3fv(glGetUniformLocation(location, "light.Color"), 1, lightIt->color.xP());
            glUniform3fv(glGetUniformLocation(location, "light.Position"), 1, transIt->position.xP());
            glUniform1f(glGetUniformLocation(location, "light.Linear"), lightIt->linear);
            glUniform1f(glGetUniformLocation(location, "light.Quadratic"), lightIt->quadratic);
            glUniform1f(glGetUniformLocation(location, "light.Radius"), lightIt->calculateRadius());
            renderQuad();

            // Increment all
            ++transIt;
            ++lightIt;
        }
    }
}

void Renderer::spotLightPass(const std::vector<TransformComponent> &transforms, const CameraComponent& camera, const std::vector<SpotLightComponent> &spotLights)
{
    auto v = camera.viewMatrix;
    v.inverse();
    auto pos = gsl::vec3(v.at(0, 3), v.at(1, 3), v.at(2, 3));
    auto location = mSpotLightShader->getProgram();
    glUseProgram(location);
    glUniform1i(glGetUniformLocation(location, "gPosition"),   0);
    glUniform1i(glGetUniformLocation(location, "gNormal"),     1);
    glUniform1i(glGetUniformLocation(location, "gAlbedoSpec"), 2);

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
            glUniform3fv(glGetUniformLocation(location, "viewPos"), 1, pos.xP());
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

void Renderer::resizeGBuffer()
{
    glBindTexture(GL_TEXTURE_2D, mGPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width(), height(), 0, GL_RGB, GL_FLOAT, nullptr);

    glBindTexture(GL_TEXTURE_2D, mGNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width(), height(), 0, GL_RGB, GL_FLOAT, nullptr);

    glBindTexture(GL_TEXTURE_2D, mGAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width(),  height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, mRboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width(), height());

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Renderer::renderQuad()
{
    glBindVertexArray(mScreenSpacedQuadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Renderer::renderSkybox(const CameraComponent &camera)
{

    if (mSkybox->mMaterial.mTexture < 0)
        return;

    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(mSkybox->mVAO);

    auto shader = mSkybox->mMaterial.mShader;
    glUseProgram(shader->getProgram());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, static_cast<unsigned int>(mSkybox->mMaterial.mTexture));

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

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mSkybox->mVerticesCount));
    glDepthFunc(GL_LESS);
}

void Renderer::exposeEvent(QExposeEvent *)
{
    if(!isInitialized)
    {
        init();
        isInitialized = true;
    }

    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, static_cast<GLint>(width() * retinaScale), static_cast<GLint>(height() * retinaScale));
    windowUpdated();
}

//Simple way to turn on/off wireframe mode
//Not totally accurate, but draws the objects with
//lines instead of filled polygons
void Renderer::toggleWireframe()
{
    mWireframe = !mWireframe;
    if (mWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);    //turn on wireframe mode
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    //turn off wireframe mode
        glEnable(GL_CULL_FACE);
    }
}

/// Uses QOpenGLDebugLogger if this is present
/// Reverts to glGetError() if not
void Renderer::checkForGLerrors()
{
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
