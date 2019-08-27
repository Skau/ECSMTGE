#include "app.h"

#include <QDebug>

App::App()
{
    mMainWindow = std::make_unique<MainWindow>();
    mRenderer = std::make_unique<Renderer>();
    mMainWindow->addViewport(mRenderer.get());
    mRenderer->init();

    connect(mRenderer.get(), &Renderer::escapeKeyPressed, this, &App::quit);

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


    mRenderer->render(mDeltaTime);


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
