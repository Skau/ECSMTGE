#include "app.h"

#include "mainwindow.h"
#include "renderwindow.h"
#include <QDebug>

App::App()
{
    mMainWindow = new MainWindow();
    mRenderWindow = mMainWindow->getRenderWindow();
}

void App::init()
{
    mMainWindow->show();
    mRenderWindow->init();
    mElapsedTime.start();

//    QObject::connect(mRenderWindow, &RenderWindow::finished, this, &App::update);
//    update();

    mTimer.start(0);
    QObject::connect(&mTimer, &QTimer::timeout, this, &App::update);
}

void App::update()
{
    deltaTime = lag / MS_PER_UPDATE;

    processInput();

    double current = mElapsedTime.elapsed();
    double elapsed = current - previous;
    previous = current;
    lag += elapsed;

    while(lag >= MS_PER_UPDATE)
    {
        lag -= MS_PER_UPDATE;
    }

    render();
}

void App::processInput()
{
    mRenderWindow->handleInput(deltaTime);
}

void App::render()
{
    mRenderWindow->render(deltaTime);
}
