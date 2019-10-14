#include "app.h"

#include <QDebug>

#include "inputhandler.h"

#include "scene.h"
#include "entitymanager.h"

#include "inputsystem.h"
#include "physicssystem.h"
#include "scriptsystem.h"

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
    connect(mMainWindow.get(), &MainWindow::newScene, mWorld.get(), &World::newScene);
    connect(mMainWindow.get(), &MainWindow::saveScene, mWorld.get(), &World::saveScene);
    connect(mMainWindow.get(), &MainWindow::loadScene, mWorld.get(), &World::loadScene);
    mMainWindow->setEntityManager(mWorld->getEntityManager());

    // Temp solution, this is just for script system experimentation
    connect(mWorld->getEntityManager().get(), &EntityManager::refreshWidgets, mMainWindow.get(), &MainWindow::refreshWidgets);

    // Script System needs the entity manager so data is available in scripts
    ScriptSystem::get()->setEntityManager(mWorld->getEntityManager());

    mWorld->initScene();

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

    auto material = std::make_shared<Material>(ResourceManager::instance()->getShader("singleColor"));
    material->mParameters =
    {
        {"color", gsl::vec3{1.f, 1.f, 0.f}}
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

    // Time since last frame in seconds
    mDeltaTime = mDeltaTimer.restart() / 1000.f;
    mRenderer->mTimeSinceStart += mDeltaTime;

    calculateFrames();

    // Tick scripts if playing
    if(mCurrentlyPlaying)
    {
        auto& scripts = mWorld->getEntityManager()->getScriptComponents();
        ScriptSystem::get()->tick(mDeltaTime, scripts);
    }

    // Get all necessary components that are reused for systems
    const auto& inputs = mWorld->getEntityManager()->getInputComponents();
    auto& transforms = mWorld->getEntityManager()->getTransformComponents();
    auto& cameras = mWorld->getEntityManager()->getCameraComponents();
    auto& physics = mWorld->getEntityManager()->getPhysicsComponents();
    auto& colliders = mWorld->getEntityManager()->getColliderComponents();
    auto& sounds = mWorld->getEntityManager()->getSoundComponents();

    // Input:
    mEventHandler->updateMouse();
    InputSystem::HandleInput(mDeltaTime, inputs, transforms);
    InputSystem::HandleCameraInput(mDeltaTime, inputs, transforms, cameras);

    // Physics:
    /* Note: Physics calculation should be happening on a separate thread
     * and instead of sending references to the lists we should take copies
     * and then later apply those copies to the original lists.
     */
     PhysicsSystem::UpdatePhysics(transforms, physics, colliders, mDeltaTime);

    // Sound
    // Sound listener is using the active camera view matrix (for directions) and transform (for position)
    for (const auto& camera : cameras)
    {
        if(camera.isCurrentActive)
        {
            mSoundListener->update(camera, *mWorld->getEntityManager()->getComponent<TransformComponent>(camera.entityId));
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
        if(camera.isCurrentActive)
        {
            auto& renders = mWorld->getEntityManager()->getMeshComponents();
            mRenderer->render(renders, transforms, camera,
                                      mWorld->getEntityManager()->getDirectionalLightComponents(),
                                      mWorld->getEntityManager()->getSpotLightComponents(),
                                      mWorld->getEntityManager()->getPointLightComponents());
            break;
        }
    }

    currentlyUpdating = false;
}

void App::quit()
{
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
