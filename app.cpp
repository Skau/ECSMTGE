#include "app.h"

#include <QDebug>

#include "inputhandler.h"

#include "scene.h"
#include "entitymanager.h"

#include "inputsystem.h"
#include "physicssystem.h"
#include "scriptsystem.h"

#include <QJsonDocument>

App::App()
{
    mSoundManager = std::make_unique<SoundManager>();
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

// Slot called from Renderer when its done with initialization
void App::initTheRest()
{
    mWorld = std::make_unique<World>();
    connect(mMainWindow.get(), &MainWindow::newScene, this, &App::newScene);
    connect(mMainWindow.get(), &MainWindow::saveScene, this, &App::saveScene);
    connect(mMainWindow.get(), &MainWindow::loadScene, this, &App::loadScene);
    mMainWindow->setEntityManager(mWorld->getEntityManager());

    // Temp solution, this is just for script system experimentation
    connect(mWorld->getEntityManager().get(), &EntityManager::refreshWidgets, mMainWindow.get(), &MainWindow::refreshWidgets);

    // Script System needs the entity manager so data is available in scripts
    ScriptSystem::get()->setEntityManager(mWorld->getEntityManager());

    // Load editor session data
    loadSession("session.json");

    if (!mWorld->isSceneValid())
        mWorld->initBlankScene();

    // Setup update loop
    connect(&mUpdateTimer, &QTimer::timeout, this, &App::update);
    mUpdateTimer.start(16); // Simulates 60ish fps

    mDeltaTimer.start();
    mFPSTimer.start();

    SoundManager::checkOpenALError();

    // Send skybox data to renderer

    auto skyboxMesh = ResourceManager::instance()->getMesh("skybox");

    auto skyboxMaterial = std::make_shared<Material>();
    auto skyShader = ResourceManager::instance()->getShader("skybox");
    skyboxMaterial->mShader = skyShader;
    auto texture = ResourceManager::instance()->getTexture("skybox");
    skyboxMaterial->mTextures.push_back({texture->id(), texture->mType});

    mRenderer->mSkyboxMesh = skyboxMesh;
    mRenderer->mSkyboxMaterial = skyboxMaterial;

    // Send axis data to renderer

    auto axisMesh = ResourceManager::instance()->getMesh("axis");
    axisMesh->mRenderType = GL_LINES;

    auto axisMaterial = std::make_shared<Material>();
    axisMaterial->mShader = ResourceManager::instance()->getShader("axis");

    mRenderer->mAxisMesh = axisMesh;
    mRenderer->mAxisMaterial = axisMaterial;

    // mRenderer->mPostprocessor->steps.push_back({ResourceManager::instance()->getShader("passthrough")});
    mRenderer->mOutlineeffect->outputToDefault = false;

    auto material = std::make_shared<Material>(ResourceManager::instance()->getShader("ui_singleColor"));
    material->mParameters =
    {
        {"p_color", gsl::vec3{1.f, 1.f, 0.f}}
    };
    mRenderer->mOutlineeffect->steps.push_back(
    {
        material,
        0
    });

    material = std::make_shared<Material>(ResourceManager::instance()->getShader("blur"));
    material->mParameters =
    {
        {"radius", 2}
    };
    mRenderer->mOutlineeffect->steps.push_back(
    {
        material,
        0
    });
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

    auto mousePoint = mRenderer->mapFromGlobal(QCursor::pos());
    auto cameras = mWorld->getEntityManager()->getCameraComponents();
    auto meshes = mWorld->getEntityManager()->getMeshComponents();
    auto transforms = mWorld->getEntityManager()->getTransformComponents();

    if (!cameras.empty())
    {
        auto entity = mRenderer->getMouseHoverObject(gsl::ivec2{mousePoint.x(), mousePoint.y()}, meshes, transforms, cameras.front());
        for (auto it = mMainWindow->mTreeDataCache.begin(); it != mMainWindow->mTreeDataCache.end(); ++it)
        {
            if (it->second.entityId == entity)
            {
                mMainWindow->setSelected(&it->second);
                break;
            }
        }
    }
}

void App::update()
{
    // Not exactly needed now, but maybe this should be here? Timer does call this function every 16 ms.
    if(currentlyUpdating)
        return;
    currentlyUpdating = true;

    mDeltaTime = mDeltaTimer.restart() / 1000.f;
    mRenderer->mTimeSinceStart += mDeltaTime;

    calculateFrames();

    // Get all necessary components that are reused for systems
    auto& cameras       = mWorld->getEntityManager()->getCameraComponents();
    auto& colliders     = mWorld->getEntityManager()->getColliderComponents();
    auto& inputs        = mWorld->getEntityManager()->getInputComponents();
    auto& physics       = mWorld->getEntityManager()->getPhysicsComponents();
    auto& sounds        = mWorld->getEntityManager()->getSoundComponents();
    auto& scripts       = mWorld->getEntityManager()->getScriptComponents();
    auto& transforms    = mWorld->getEntityManager()->getTransformComponents();


    auto scriptSystem = ScriptSystem::get();

    // Tick scripts if playing
    if(mCurrentlyPlaying)
    {
        scriptSystem->tick(mDeltaTime, scripts);
    }

    // Input

    // Editor Camera handles input if not playing
    if(!mCurrentlyPlaying)
    {
        mEventHandler->updateMouse();

        for(auto& camera : cameras)
        {
            if(camera.isEditorCamera)
            {
                auto transform = mWorld->getEntityManager()->getComponent<TransformComponent>(camera.entityId);
                InputSystem::HandleEditorCameraInput(mDeltaTime, *transform, camera);
            }
        }
    }
    // Send input to scripts if playing
    else
    {
        auto stringsPressed = mEventHandler->inputPressedStrings;
        auto stringsReleased = mEventHandler->inputReleasedStrings;
        for(auto& input : inputs)
        {
            if(input.controlledWhilePlaying)
            {
                if(auto comp = mWorld->getEntityManager()->getComponent<ScriptComponent>(input.entityId))
                {
                    scriptSystem->runKeyPressedEvent(*comp, stringsPressed);
                    scriptSystem->runKeyReleasedEvent(*comp, stringsReleased);
                }
            }
        }
        mEventHandler->inputReleasedStrings.clear();
    }
    // Physics:
    /* Note: Physics calculation should be happening on a separate thread
     * and instead of sending references to the lists we should take copies
     * and then later apply those copies to the original lists.
     */
     auto hitInfos = PhysicsSystem::UpdatePhysics(transforms, physics, colliders, mDeltaTime);

     if(mCurrentlyPlaying && hitInfos.size())
     {
         scriptSystem->runHitEvents(scripts, hitInfos);
     }

    // Sound
    // Sound listener is using the active camera view matrix (for directions) and transform (for position)
    for (const auto& camera : cameras)
    {
        if(camera.isEditorCamera)
        {
            if(!mCurrentlyPlaying)
            {
                mSoundListener->update(camera, *mWorld->getEntityManager()->getComponent<TransformComponent>(camera.entityId));
                break;
            }
        }
        else
        {
            if(mCurrentlyPlaying)
            {
                mSoundListener->update(camera, *mWorld->getEntityManager()->getComponent<TransformComponent>(camera.entityId));
                break;
            }
        }
    }
    SoundManager::UpdatePositions(transforms, sounds);
    SoundManager::UpdateVelocities(physics, sounds);

    // UI
    mMainWindow->updateComponentWidgets();

    // Cameras
    CameraSystem::updateCameras(transforms, cameras);

    // Calculate mesh bounds
    mWorld->getEntityManager()->UpdateBounds();
    // -------- Frustum culling here -----------

    // Rendering
    for (const auto& camera : cameras)
    {
        if(camera.isEditorCamera)
        {
            if(!mCurrentlyPlaying)
            {
                auto& renders = mWorld->getEntityManager()->getMeshComponents();
                mRenderer->render(renders, transforms, camera,
                                  mWorld->getEntityManager()->getDirectionalLightComponents(),
                                  mWorld->getEntityManager()->getSpotLightComponents(),
                                  mWorld->getEntityManager()->getPointLightComponents(),
                                  mWorld->getEntityManager()->getParticleComponents());
                break;
            }
        }
        else
        {
            if(mCurrentlyPlaying)
            {
                auto& renders = mWorld->getEntityManager()->getMeshComponents();
                mRenderer->render(renders, transforms, camera,
                                  mWorld->getEntityManager()->getDirectionalLightComponents(),
                                  mWorld->getEntityManager()->getSpotLightComponents(),
                                  mWorld->getEntityManager()->getPointLightComponents(),
                                  mWorld->getEntityManager()->getParticleComponents());
                break;
            }
        }
    }

    // Update JS comps
    scriptSystem->updateJSComponents(scripts);

    currentlyUpdating = false;
}

void App::quit()
{
    saveSession();

    mMainWindow->close();
}

void App::updatePerspective()
{
    auto& cameras = mWorld->getEntityManager()->getCameraComponents();
    CameraSystem::updateCameras(cameras, gsl::mat4::persp(FOV, static_cast<float>(mRenderer->width()) / mRenderer->height(), 0.1f, 100.f));
}

// Called when play action is pressed while not playing in UI
void App::onPlay()
{
    mCurrentlyPlaying = true;

    mWorld->saveTemp();

    for(auto camera : mWorld->getEntityManager()->getCameraComponents())
    {
        if(!camera.isEditorCamera)
        {
            if(auto mesh = mWorld->getEntityManager()->getComponent<MeshComponent>(camera.entityId))
            {
                mesh->isVisible = false;
            }
        }
    }

    mMainWindow->setSelected(nullptr);

    auto& scripts = mWorld->getEntityManager()->getScriptComponents();
    ScriptSystem::get()->beginPlay(scripts);

    auto sounds = mWorld->getEntityManager()->getSoundComponents();
    SoundManager::play(sounds);
}

// Called when play action is pressed while playing in UI
void App::onStop()
{
    mCurrentlyPlaying = false;

    auto& scripts = mWorld->getEntityManager()->getScriptComponents();
    ScriptSystem::get()->endPlay(scripts);

    auto sounds = mWorld->getEntityManager()->getSoundComponents();
    SoundManager::stop(sounds);

    mRenderer->EditorCurrentEntitySelected = nullptr;

    for(auto camera : mWorld->getEntityManager()->getCameraComponents())
    {
        if(!camera.isEditorCamera)
        {
            if(auto mesh = mWorld->getEntityManager()->getComponent<MeshComponent>(camera.entityId))
            {
                mesh->isVisible = true;
            }
        }
    }

    mWorld->loadTemp();
}

void App::calculateFrames()
{
    ++mFrameCounter;
    mTotalDeltaTime += mDeltaTime;
    double elapsed = mFPSTimer.elapsed();
    if(elapsed >= 100)
    {
        mMainWindow->updateStatusBar(mRenderer->getNumberOfVerticesDrawn(), mTotalDeltaTime / mFrameCounter, mFrameCounter / mTotalDeltaTime);
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
    updatePerspective();
}
