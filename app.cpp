#include "app.h"

#include <QDebug>
#include <QJsonDocument>

#include "inputhandler.h"
#include "scene.h"
#include "entitymanager.h"
#include "inputsystem.h"
#include "physicssystem.h"
#include "scriptsystem.h"

#include "Instrumentor.h"

App::App()
{
    PROFILE_BEGIN_SESSION("Startup", "Profile-Startup");

    PROFILE_FUNCTION();
    mSoundManager = std::unique_ptr<SoundManager>(new SoundManager());
    mSoundListener = std::make_unique<SoundListener>();

    mMainWindow = std::make_unique<MainWindow>();
    mRenderer = mMainWindow->getRenderer();

    connect(mMainWindow.get(), &MainWindow::toggleWireframe, mRenderer, &Renderer::toggleWireframe);
    connect(mMainWindow.get(), &MainWindow::shutUp, this, &App::toggleMute);
    connect(mMainWindow.get(), &MainWindow::play, this, &App::onPlay);
    connect(mMainWindow.get(), &MainWindow::stop, this, &App::onStop);
    connect(mMainWindow.get(), &MainWindow::quitting, this, &App::saveSession);

    connect(mRenderer, &Renderer::initDone, this, &App::initTheRest);
    connect(mRenderer, &Renderer::windowUpdated, this, &App::updatePerspective);

    mEventHandler = std::make_shared<InputHandler>(mRenderer);
    connect(mEventHandler.get(), &InputHandler::escapeKeyPressed, mMainWindow.get(), &MainWindow::close);
    connect(mEventHandler.get(), &InputHandler::mousePress, this, &App::mousePicking);
    mRenderer->installEventFilter(mEventHandler.get());
}

App::~App()
{
    PROFILE_END_SESSION();
}

// Slot called from Renderer when its done with initialization
void App::initTheRest()
{
    PROFILE_FUNCTION();
    mResourceManager = std::unique_ptr<ResourceManager>(new ResourceManager{});

    mWorld = std::unique_ptr<World>(new World{});
    connect(mMainWindow.get(), &MainWindow::newScene, this, &App::newScene);
    connect(mMainWindow.get(), &MainWindow::saveScene, this, &App::saveScene);
    connect(mMainWindow.get(), &MainWindow::loadScene, this, &App::loadScene);
    connect(mWorld->getEntityManager().get(), &EntityManager::updateUI, mMainWindow.get(), &MainWindow::updateUI);
    connect(mWorld.get(), &World::updateSceneName, mMainWindow.get(), &MainWindow::setSceneName);

    // Load editor session data
    loadSession("session.json");

    if (!mWorld->isSceneValid())
        mWorld->initBlankScene();

    // Setup update loop
    connect(&mUpdateTimer, &QTimer::timeout, this, &App::update);

    mDeltaTimer.start();
    mFPSTimer.start();

    mSoundManager->checkOpenALError();

    // Send skybox data to renderer

    auto skyboxMesh = ResourceManager::instance().getMesh("skybox");
    auto skyboxMaterial = std::make_shared<Material>();
    auto skyShader = ResourceManager::instance().getShader("skybox");
    skyboxMaterial->mShader = skyShader;
    auto texture = ResourceManager::instance().getTexture("skybox");
    skyboxMaterial->mTextures.push_back({texture->id(), texture->mType});
    mRenderer->mSkyboxMesh = skyboxMesh;
    mRenderer->mSkyboxMaterial = skyboxMaterial;

    // Send axis data to renderer

    auto axisMesh = ResourceManager::instance().getMesh("axis");
    axisMesh->mRenderType = GL_LINES;
    auto axisMaterial = std::make_shared<Material>();
    axisMaterial->mShader = ResourceManager::instance().getShader("axis");
    mRenderer->mAxisMesh = axisMesh;
    mRenderer->mAxisMaterial = axisMaterial;


    // ---------- Postprocessing setup ---------------------------

    mRenderer->mPostprocessor->setTextureFormat(GL_RGBA16F);

    /** Gamma correction / tone mapping.
     * If game feels too dark / too bright, just tweak the exposure level
     * to a desired brightness / darkness.
     * Note: Settings currently tuned to own computer.
     */
    mRenderer->mPostprocessor->steps.emplace_back(
        std::make_shared<Material>(
            ResourceManager::instance().getShader("gammaCorrection"),
            std::map<std::string, ShaderParamType>{
                {"gamma", 1.4f},
                {"exposure", 0.8f}
            }
        )
    );

    // Bloom setup
    mRenderer->mBloomEffect->outputToDefault = false;
    mRenderer->mBloomEffect->setTextureFormat(GL_RGBA16F);
    mRenderer->mBloomEffect->steps.emplace_back (
        std::make_shared<Material>(
            ResourceManager::instance().getShader("extractThreshold"),
            std::map<std::string, ShaderParamType>{
                {"threshold", 1.f}
            }
        )
    );
    mRenderer->mBloomEffect->steps.emplace_back(
        std::make_shared<Material>(
            ResourceManager::instance().getShader("gaussianBlur"),
            std::map<std::string, ShaderParamType>{
                {"horizontal", false}
            }
        )
    );
    mRenderer->mBloomEffect->steps.emplace_back(
        std::make_shared<Material>(
            ResourceManager::instance().getShader("gaussianBlur"),
            std::map<std::string, ShaderParamType>{
                {"horizontal", true}
            }
        )
    );

    // Outline effect setup
    mRenderer->mOutlineeffect->outputToDefault = false;

    auto material = std::make_shared<Material>(ResourceManager::instance().getShader("ui_singleColor"));
    material->mParameters =
    {
        {"p_color", gsl::vec3{1.f, 1.f, 0.f}}
    };
    mRenderer->mOutlineeffect->steps.push_back(
    {
        material,
        0
    });

    material = std::make_shared<Material>(ResourceManager::instance().getShader("blur"));
    material->mParameters =
    {
        {"radius", 2}
    };
    mRenderer->mOutlineeffect->steps.push_back(
    {
        material,
        0
    });
    PROFILE_END_SESSION();
    mUpdateTimer.start(16); // Simulates 60ish fps
    PROFILE_BEGIN_SESSION("Editor", "Profile-Editor");
}

void App::toggleMute(bool mode)
{
    if (mSoundListener)
        mSoundListener->setMute(mode);
}

void App::mousePicking()
{
    if(mCurrentlyPlaying)
        return;

    PROFILE_FUNCTION();
    auto mousePoint = mRenderer->mapFromGlobal(QCursor::pos());
    auto cameras = mWorld->getEntityManager()->getCameraComponents();
    auto meshes = mWorld->getEntityManager()->getMeshComponents();
    auto transforms = mWorld->getEntityManager()->getTransformComponents();

    if (!cameras.empty())
    {
        auto entity = mRenderer->getMouseHoverObject(gsl::ivec2{mousePoint.x(), mousePoint.y()}, meshes, transforms, cameras.front());
        bool found{false};
        for (auto it = mMainWindow->mTreeDataCache.begin(); it != mMainWindow->mTreeDataCache.end(); ++it)
        {
            if (it->second.entityId == entity)
            {
                mMainWindow->setSelected(&it->second);
                found = true;
                break;
            }
        }

        if(!found)
        {
            mMainWindow->setSelected(nullptr);
        }
    }
}

void App::update()
{
    // Not exactly needed now, but maybe this should be here? Timer does call this function every 16 ms.
    if(currentlyUpdating)
        return;

    PROFILE_FUNCTION();
    currentlyUpdating = true;

    mDeltaTime = mDeltaTimer.restart() / 1000.f;
    mRenderer->mTimeSinceStart += mDeltaTime;

    calculateFrames();

    auto& transforms    = mWorld->getEntityManager()->getTransformComponents();
    auto& physics       = mWorld->getEntityManager()->getPhysicsComponents();
    auto& colliders     = mWorld->getEntityManager()->getColliderComponents();

    auto currentCamera = mWorld->getCurrentCamera(mCurrentlyPlaying);
    auto cameraTransform = mWorld->getEntityManager()->getComponent<TransformComponent>(currentCamera->entityId);
    {
        PROFILE_SCOPE("Camera");
        // Camera:
        /* Note: Current camera depends on if the engine is in editor-mode or not.
         * Editor camera is handled in C++, game camera is handled through script.
         */
        mEventHandler->updateMouse(mCurrentlyPlaying);

        if(!mCurrentlyPlaying)
        {
            InputSystem::HandleEditorCameraInput(mDeltaTime, *cameraTransform, *currentCamera);
        }
        CameraSystem::updateLookAtRotation(*cameraTransform, *currentCamera);
        CameraSystem::updateCameraViewMatrices(transforms, mWorld->getEntityManager()->getCameraComponents());
    }

    std::vector<HitInfo> hitInfos;

    {
        PROFILE_SCOPE("Physics");
        // Physics:
        /* Note: Physics calculation should be happening on a separate thread
         * and instead of sending references to the lists we should take copies
         * and then later apply those copies to the original lists.
         */
        if (mCurrentlyPlaying)
            hitInfos = PhysicsSystem::UpdatePhysics(transforms, physics, colliders, mDeltaTime);

    }
    {
        PROFILE_SCOPE("Sounds");
        // Sound:
        // Note: Sound listener is using the active camera view matrix (for directions) and transform (for position)
        auto& sounds = mWorld->getEntityManager()->getSoundComponents();
        mSoundListener->update(*currentCamera, *cameraTransform);
        mSoundManager->UpdatePositions(transforms, sounds);
        mSoundManager->UpdateVelocities(physics, sounds);
    }


    {
        PROFILE_SCOPE("Bounds");
        // Calculate mesh bounds
        // Note: This is only done if the transform has changed.
        mWorld->getEntityManager()->UpdateBounds();
        // -------- Frustum culling here -----------
    }

    {
        PROFILE_SCOPE("Rendering");
        // Rendering
        auto& renders = mWorld->getEntityManager()->getMeshComponents();
        mRenderer->render(renders, transforms, *currentCamera,
                          mWorld->getEntityManager()->getDirectionalLightComponents(),
                          mWorld->getEntityManager()->getSpotLightComponents(),
                          mWorld->getEntityManager()->getPointLightComponents(),
                          mWorld->getEntityManager()->getParticleComponents());
    }



    // JAVASCRIPT HERE
    if(mCurrentlyPlaying)
    {
        PROFILE_SCOPE("Scripting");
        auto& scripts = mWorld->getEntityManager()->getScriptComponents();

        ScriptSystem::get()->updateJSComponents(scripts);

        auto& inputs = mWorld->getEntityManager()->getInputComponents();
        ScriptSystem::get()->update(scripts, inputs, mEventHandler->inputPressedStrings,
                                    mEventHandler->inputReleasedStrings, mEventHandler->MouseOffset,
                                    hitInfos, mDeltaTime);

        ScriptSystem::get()->updateCPPComponents(scripts);

        mEventHandler->inputReleasedStrings.clear();

        mWorld->getEntityManager()->removeEntitiesMarked();

        static unsigned int garbageCounter{0};
        garbageCounter++;
        if (garbageCounter - 1 > ScriptSystem::get()->garbageCollectionFrequency)
        {
            garbageCounter = 0;
            ScriptSystem::get()->takeOutTheTrash(scripts);
        }
    }

    currentlyUpdating = false;
}

void App::quit()
{
    saveSession();

    mMainWindow->close();
}

void App::updatePerspective()
{
    PROFILE_FUNCTION();
    auto& cameras = mWorld->getEntityManager()->getCameraComponents();
    CameraSystem::updateCameraViewMatrices(cameras, gsl::mat4::persp(FOV, static_cast<float>(mRenderer->width()) / mRenderer->height(), 0.1f, 100.f));
}

// Called when play action is pressed while not playing in UI
void App::onPlay()
{
    PROFILE_END_SESSION();
    PROFILE_BEGIN_SESSION("Play", "Profile-Play");
    PROFILE_FUNCTION();
    mCurrentlyPlaying = true;

    saveSession();
    mWorld->saveTemp();

    auto currentCamera = mWorld->getCurrentCamera(mCurrentlyPlaying);
    if(auto mesh = mWorld->getEntityManager()->getComponent<MeshComponent>(currentCamera->entityId))
    {
        mesh->isVisible = false;
    }

    mMainWindow->setSelected(nullptr);

    auto sounds = mWorld->getEntityManager()->getSoundComponents();
    mSoundManager->play(sounds);
}

// Called when play action is pressed while playing in UI
void App::onStop()
{
    PROFILE_END_SESSION();
    PROFILE_BEGIN_SESSION("Editor", "Profile-Editor");
    PROFILE_FUNCTION();
    mCurrentlyPlaying = false;

    auto& scripts = mWorld->getEntityManager()->getScriptComponents();
    ScriptSystem::get()->endPlay(scripts);

    auto sounds = mWorld->getEntityManager()->getSoundComponents();
    mSoundManager->stop(sounds);

    mRenderer->EditorCurrentEntitySelected = nullptr;

    auto currentCamera = mWorld->getCurrentCamera(mCurrentlyPlaying);
    if(auto mesh = mWorld->getEntityManager()->getComponent<MeshComponent>(currentCamera->entityId))
    {
        mesh->isVisible = true;
    }

    ScriptSystem::get()->initGarbageCollection();

    // Reset JS states.
    for(auto& comp : mWorld->getEntityManager()->getScriptComponents())
    {
        if(comp.JSEntity)
        {
            delete comp.JSEntity;
            comp.JSEntity = nullptr;
        }
    }

    mWorld->loadTemp();
    updatePerspective();
}

void App::calculateFrames()
{
    ++mFrameCounter;
    mTotalDeltaTime += mDeltaTime;
    double elapsed = mFPSTimer.elapsed();
    if(elapsed >= 1000)
    {
        mMainWindow->updateStatusBar(mRenderer->getNumberOfVerticesDrawn(), (mTotalDeltaTime / mFrameCounter) * 1000.f, mFrameCounter / mTotalDeltaTime);
        mFrameCounter = 0;
        mTotalDeltaTime = 0;
        mFPSTimer.restart();
    }
}

void App::loadSession(const std::string &path)
{
    QFile file(QString::fromStdString(path));
    if(!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    if(!document.isObject())
    {
        return;
    }

    QJsonObject mainObject = document.object();
    if(mainObject.isEmpty())
    {
        return;
    }

    if (mWorld)
    {
        auto value = mainObject["DefaultMap"];
        if (value.isString())
        {
            mWorld->loadScene(value.toString().toStdString());
        }
    }
}

void App::saveSession()
{
    QFile file{"session.json"};
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        qDebug() << "Failed to save session file.";
        return;
    }

    QJsonObject mainObject{};

    if (mWorld)
        if (auto path = mWorld->sceneFilePath())
            mainObject["DefaultMap"] = QString::fromStdString(path.value());

    file.write(QJsonDocument{mainObject}.toJson());
}

void App::newScene()
{
    mWorld->newScene();
    updatePerspective();
}

void App::loadScene(const std::string& path)
{
    mWorld->loadScene(path);
    updatePerspective();
}

void App::saveScene(const std::string& path)
{
    mWorld->saveScene(path);
    saveSession();
    updatePerspective();
}
