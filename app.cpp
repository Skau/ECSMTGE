#include "app.h"

#include <QDebug>

#include "inputhandler.h"

#include "scene.h"
#include "entitymanager.h"

#include "inputsystem.h"

#include "ui_mainwindow.h"

App::App()
{
    mMainWindow = std::make_unique<MainWindow>();
    mRenderer = mMainWindow->getRenderer();

    mEventHandler = std::make_shared<InputHandler>(mRenderer);

    mRenderer->installEventFilter(mEventHandler.get());

    connect(mRenderer, &Renderer::initDone, this, &App::initTheRest);

    connect(mMainWindow->ui->actionToggle_wireframe, &QAction::triggered, mRenderer, &Renderer::toggleWireframe);
    connect(mEventHandler.get(), &InputHandler::escapeKeyPressed, mMainWindow.get(), &MainWindow::close);
    connect(mMainWindow->ui->actionExit, &QAction::triggered, mMainWindow.get(), &MainWindow::close);
    connect(mRenderer, &Renderer::windowUpdated, this, &App::updatePerspective);
}

// Slot called from Renderer when its done with initialization
void App::initTheRest()
{
    mWorld = std::make_unique<World>();

    mMainWindow->setEntityManager(mWorld->getEntityManager());

    mWorld->initCurrentScene();

    connect(&mUpdateTimer, &QTimer::timeout, this, &App::update);

    mUpdateTimer.start(16); // Simulates 60ish fps

    mDeltaTimer.start();
    mFPSTimer.start();
}


void App::update()
{
    // Not exactly needed now, but maybe this should be here? Timer does call this function every 16 ms.
    if(currentlyUpdating)
        return;
    currentlyUpdating = true;


    mDeltaTime = mDeltaTimer.restart();


    calculateFrames();

    const auto& inputs = mWorld->getEntityManager()->getInputComponents();
    auto& transforms = mWorld->getEntityManager()->getTransforms();

    mEventHandler->updateMouse();

    InputSystem::HandleInput(mDeltaTime, inputs, transforms);

    const auto& renders = mWorld->getEntityManager()->getMeshComponents();
    auto& cameras = mWorld->getEntityManager()->getCameraComponents();

    CameraSystem::updateCameras(transforms, cameras);

    for (const auto& camera : cameras) {
        mRenderer->render(renders, transforms, camera);
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

    CameraSystem::updateCameras(cameras, gsl::mat4::persp(FOV, static_cast<float>(mRenderer->width()) / mRenderer->height(), 1.f, 100.f));
}

void App::calculateFrames()
{
    ++mFrameCounter;
    mTotalDeltaTime += mDeltaTime;
    double elapsed = mFPSTimer.elapsed();
    if(elapsed >= 100)
    {
        mMainWindow->showFPS(mTotalDeltaTime / mFrameCounter, mFrameCounter / mTotalDeltaTime * 1000);
        mFrameCounter = 0;
        mTotalDeltaTime = 0;
        mFPSTimer.restart();
    }
}
