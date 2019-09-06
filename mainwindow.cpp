#include "innpch.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSurfaceFormat>
#include <QDesktopWidget>

#include "renderer.h"
#include "Widgets/transformwidget.h"
#include "Widgets/renderwidget.h"
#include "componentdata.h"

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

    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->scrollArea->setWidgetResizable(true);

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

void MainWindow::setEntityManager(std::shared_ptr<EntityManager> entityManager)
{
    mEntityManager = entityManager;

    connect(this, &MainWindow::createObject, mEntityManager.get(), &EntityManager::createObject);
    connect(this, &MainWindow::getAllComponentsForEntity, mEntityManager.get(), &EntityManager::getAllComponents);

    connect(mEntityManager.get(), &EntityManager::updateUI, this, &MainWindow::updateUI);
}



// Called from entity manager when a new entity is added to the scene
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

// When an object is selected via the hierarchy
void MainWindow::on_objectList_activated(const QModelIndex &index)
{
    // get the index and check the cache
    auto i = static_cast<unsigned>(index.row());
    if(i < mEntityDataCache.size())
    {
        currentEntitySelected = &mEntityDataCache[i];
        // Update the selected object name
        ui->label_SelectedObject->setText(QString::fromStdString(currentEntitySelected->name));

        // Delete any previous component widgets
        if(ui->scrollArea->widget())
            delete ui->scrollArea->widget();

        // Get the components for this entity
        std::vector<Component*> components;
        if(getAllComponentsForEntity(currentEntitySelected->entityId, components))
        {
            // Components were found, add them

            QWidget* widget = new QWidget();
            ui->scrollArea->setWidget(widget);
            QVBoxLayout* layout = new QVBoxLayout();
            widget->setLayout(layout);

            for(auto& component: components)
            {
                switch (component->type)
                {
                case ComponentType::Render:
                {
                    auto render = static_cast<Render*>(component);
                    auto widget = new RenderWidget(this);

                    // Set up render widget here

                    widget->setName(render->meshData.mName);

                    layout->addWidget(widget);
                    break;
                }
                case ComponentType::Transform:
                {

                    auto transform = static_cast<Transform*>(component);
                    auto widget = new TransformWidget(this);

                    // Set up transform widget here

                    widget->setPosition(transform->position);
                    widget->setRotation(transform->rotation);
                    widget->setScale(transform->scale);

                    layout->addWidget(widget);
                    break;
                }
                default:
                    break;
                }
            }
        }
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


