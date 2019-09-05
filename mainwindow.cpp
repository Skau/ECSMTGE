#include "innpch.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSurfaceFormat>
#include <QDesktopWidget>

#include "renderer.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mRenderer = new Renderer();

    mRenderWindowContainer = QWidget::createWindowContainer(mRenderer);
    ui->OpenGLLayout->addWidget(mRenderWindowContainer);
    mRenderWindowContainer->setFocus();

    resize(QDesktopWidget().availableGeometry(this).size() * 0.7);

    show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showFPS(double deltaTime, double frameCounter)
{
    statusBar()->showMessage(" Time pr FrameDraw: " + QString::number(deltaTime, 'g', 4) + " ms  |  " + "FPS: " + QString::number(frameCounter, 'g', 4));
}
