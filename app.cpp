#include "app.h"

#include <QDebug>

#include "inputhandler.h"

#include "scene.h"
#include "entitymanager.h"

#include "inputsystem.h"
#include "physicssystem.h"

#include "ui_mainwindow.h"

App::App()
{
    mSoundManager = std::make_unique<SoundManager>();
    mSoundListener = std::make_unique<SoundListener>();

    mMainWindow = std::make_unique<MainWindow>();
    mRenderer = mMainWindow->getRenderer();

    mEventHandler = std::make_shared<InputHandler>(mRenderer);

    mRenderer->installEventFilter(mEventHandler.get());

    connect(mRenderer, &Renderer::initDone, this, &App::initTheRest);

    connect(mMainWindow->ui->actionToggle_wireframe, &QAction::triggered, mRenderer, &Renderer::toggleWireframe);
    connect(mEventHandler.get(), &InputHandler::escapeKeyPressed, mMainWindow.get(), &MainWindow::close);
    connect(mMainWindow->ui->actionExit, &QAction::triggered, mMainWindow.get(), &MainWindow::close);
    connect(mRenderer, &Renderer::windowUpdated, this, &App::updatePerspective);
    connect(mRenderer, &Renderer::windowUpdated, mRenderer, &Renderer::resizeGBuffer);
    connect(mEventHandler.get(), &InputHandler::mousePress, this, &App::mousePicking);
}

// Slot called from Renderer when its done with initialization
void App::initTheRest()
{ 
    connect(mMainWindow->ui->actionToggle_shutup, &QAction::toggled, this, &App::toggleMute);

    mWorld = std::make_unique<World>();

    mMainWindow->setEntityManager(mWorld->getEntityManager());

    mWorld->initCurrentScene();

    connect(&mUpdateTimer, &QTimer::timeout, this, &App::update);

    mUpdateTimer.start(16); // Simulates 60ish fps

    mDeltaTimer.start();
    mFPSTimer.start();

    SoundManager::checkOpenALError();

    // Send skybox to renderer.
    mRenderer->mSkybox = ResourceManager::instance()->getMesh("skybox");

//    mRenderer->mPostprocessor->steps.push_back({
//            ResourceManager::instance()->getShader("passthrough"),
//            1
//    });
    mRenderer->mPostprocessor->steps.push_back({ResourceManager::instance()->getShader("passthrough")});

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

    // Input:
    const auto& inputs = mWorld->getEntityManager()->getInputComponents();
    auto& transforms = mWorld->getEntityManager()->getTransformComponents();
    auto& cameras = mWorld->getEntityManager()->getCameraComponents();

    mEventHandler->updateMouse();
    InputSystem::HandleInput(mDeltaTime, inputs, transforms);
    InputSystem::HandleCameraInput(mDeltaTime, inputs, transforms, cameras);

    // Sound listener is using the active camera view matrix (for directions) and transform (for position)
    for (const auto& camera : cameras)
    {
        if(camera.isCurrentActive)
        {
            mSoundListener->update(camera, *mWorld->getEntityManager()->getComponent<TransformComponent>(camera.entityId));
        }
    }

    // Physics:
    /* Note: Physics calculation should be happening on a separate thread
     * and instead of sending references to the lists we should take copies
     * and then later apply those copies to the original lists.
     */
    auto& physics = mWorld->getEntityManager()->getPhysicsComponents();

    PhysicsSystem::UpdatePhysics(transforms, physics, mDeltaTime);

    auto& sounds = mWorld->getEntityManager()->getSoundComponents();

    SoundManager::UpdatePositions(transforms, sounds);
    SoundManager::UpdateVelocities(physics, sounds);

    mMainWindow->updateComponentWidgets();

    // Rendering:
    const auto& renders = mWorld->getEntityManager()->getMeshComponents();


    auto usedTrans = CameraSystem::updateCameras(transforms, cameras);
    // Set all used transforms's "updated" to false so that updateCameras
    // won't need to calculate more viewmatrixes than it needs to.
    for (auto index : usedTrans)
        transforms[index].updated = false;


    for (const auto& camera : cameras)
    {
        if(camera.isCurrentActive)
        {
            // mRenderer->render(renders, transforms, camera);
            mRenderer->renderDeferred(renders, transforms, camera,
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

void App::calculateFrames()
{
    ++mFrameCounter;
    mTotalDeltaTime += mDeltaTime;
    double elapsed = mFPSTimer.elapsed();
    if(elapsed >= 100)
    {
        mMainWindow->showFPS(mTotalDeltaTime / mFrameCounter, mFrameCounter / mTotalDeltaTime);
        mFrameCounter = 0;
        mTotalDeltaTime = 0;
        mFPSTimer.restart();
    }
}
