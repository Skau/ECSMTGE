#include "app.h"

#include <QDebug>

#include "scene.h"
#include "entitymanager.h"

#include "ui_mainwindow.h"

App::App()
{
    mMainWindow = std::make_unique<MainWindow>();
    mRenderer = mMainWindow->getRenderer();

    connect(mRenderer, &Renderer::initDone, this, &App::initTheRest);

    connect(mMainWindow->ui->button_toggleWireframe, &QPushButton::clicked, mRenderer, &Renderer::toggleWireframe);

    connect(mRenderer, &Renderer::escapeKeyPressed, mMainWindow.get(), &MainWindow::close);
}

void App::initTheRest()
{
    mWorld = std::make_unique<World>();

    mRenderer->setupCamera();

    connect(mRenderer, &Renderer::escapeKeyPressed, this, &App::quit);

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


    mRenderer->handleInput(mDeltaTime);


 //   mRenderer->render(mWorld->getCurrentScene()->getVisualObjects(), mDeltaTime);

    auto transforms = mWorld->getEntityManager()->getTransforms();
    auto renders = mWorld->getEntityManager()->getRenders();

    mRenderer->render(renders, transforms, mDeltaTime);

    currentlyUpdating = false;
}

void App::quit()
{
    mMainWindow->close();
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
