#include "innpch.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSurfaceFormat>
#include <QDesktopWidget>

#include "renderer.h"
#include "Widgets/transformwidget.h"
#include "Widgets/renderwidget.h"

#include <QSplitter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mRenderer = new Renderer();

    mRenderWindowContainer = QWidget::createWindowContainer(mRenderer);
    ui->OpenGLLayout->addWidget(mRenderWindowContainer);
    mRenderWindowContainer->setFocus();

    resize(QDesktopWidget().availableGeometry(this).size() * 0.7);

//    {
//        QWidget * containment = new QWidget(this);
//        containment->setLayout( new QVBoxLayout(this) );
//        QWidget * currentStructures = new QWidget(this);
//        currentStructures->setLayout( new QVBoxLayout(this) );
//        currentStructures->layout()->setAlignment( Qt::AlignTop );
//        currentStructures->layout()->addWidget(new TransformWidget(this));
//        QScrollArea * scroll = new QScrollArea(this);
//        scroll->setWidget( currentStructures );
//        scroll->setWidgetResizable( true );
//        containment->layout()->addWidget(scroll);
//        ui->scrollAreaWidgetContents->layout()->addWidget( containment );
//    }


//    {
//        QWidget * containment = new QWidget(this);
//        containment->setLayout( new QVBoxLayout(this) );
//        QWidget * currentStructures = new QWidget(this);
//        currentStructures->setLayout( new QVBoxLayout(this) );
//        currentStructures->layout()->setAlignment( Qt::AlignTop );
//        currentStructures->layout()->addWidget(new RenderWidget(this));
//        QScrollArea * scroll = new QScrollArea(this);
//        scroll->setWidget( currentStructures );
//        scroll->setWidgetResizable( true );
//        containment->layout()->addWidget(scroll);
//        ui->scrollAreaWidgetContents->layout()->addWidget( containment );

//    }

    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->scrollArea->setWidgetResizable(true);

//    QWidget* widget = new QWidget();
//    ui->scrollArea->setWidget(widget);

//    QVBoxLayout* layout = new QVBoxLayout();
//    widget->setLayout(layout);
//    for(int i = 0; i < 8; ++i)
//    {
//        layout->addWidget(new TransformWidget(this));
//    }
//    for(int i = 0; i < 3; ++i)
//    {
//        layout->addWidget(new RenderWidget(this));
//    }


//    for(int i = 0; i < 10; ++i)
//    {
//        QWidget * containment = new QWidget(this);
//        containment->setLayout( new QVBoxLayout(this) );
//        QWidget * currentStructures = new QWidget(this);
//        currentStructures->setLayout( new QVBoxLayout(this) );
//        currentStructures->layout()->setAlignment( Qt::AlignTop );
//        currentStructures->layout()->addWidget(new TransformWidget(this));
//        QScrollArea * scroll = new QScrollArea(this);
//        scroll->setWidget( currentStructures );
//        scroll->setWidgetResizable( true );
//        containment->layout()->addWidget(scroll);
//        ui->scrollAreaWidgetContents->layout()->addWidget( containment );
//    }


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

void MainWindow::updateUI(const std::vector<EntityData> &entityData)
{
    if(ui->objectList->count())
    {
        ui->objectList->clear();
    }

    for(auto& entity : entityData)
    {
        ui->objectList->addItem(QString::fromStdString(entity.name));
    }

    mEntityDataCache = entityData;
}

void MainWindow::on_objectList_activated(const QModelIndex &index)
{
    auto i = static_cast<unsigned>(index.row());
    if(i < mEntityDataCache.size())
    {
        ui->label_SelectedObject->setText(QString::fromStdString(mEntityDataCache[i].name));

        if(ui->scrollArea->widget())
            delete ui->scrollArea->widget();

        QWidget* widget = new QWidget();
        ui->scrollArea->setWidget(widget);

        QVBoxLayout* layout = new QVBoxLayout();
        widget->setLayout(layout);

        layout->addWidget(new TransformWidget(this));
        layout->addWidget(new RenderWidget(this));
    }
}

void MainWindow::on_actionCube_triggered()
{
     createObject(0);
}

void MainWindow::on_actionMonkey_triggered()
{
    createObject(1);
}


