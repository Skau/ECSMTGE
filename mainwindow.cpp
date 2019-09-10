#include "innpch.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSurfaceFormat>
#include <QDesktopWidget>

#include "renderer.h"
#include "Widgets/transformwidget.h"
#include "Widgets/meshwidget.h"
#include "Widgets/inputwidget.h"
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

    ui->button_AddComponent->setEnabled(false);
    ui->comboBox_Components->setEnabled(false);

    show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showFPS(float deltaTime, float frameCounter)
{
    statusBar()->showMessage(" Time pr FrameDraw: " + QString::number(deltaTime, 'g', 4) + " ms  |  " + "FPS: " + QString::number(frameCounter, 'g', 4));
}

void MainWindow::setEntityManager(std::shared_ptr<EntityManager> entityManager)
{
    mEntityManager = entityManager;

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

    for(int i = 0; i < ui->objectList->count(); ++i)
    {
        auto item = ui->objectList->item(i);
        item->setFlags(item->flags() | Qt::ItemIsEditable);

    }

    ui->objectList->setEditTriggers(QAbstractItemView::DoubleClicked);

    mEntityDataCache = entityData;
}

// If a widget is removed we need to recreate the components
void MainWindow::onWidgetRemoved()
{
    updateComponentArea(currentEntitySelected->entityId);
}

// When an object is selected via the hierarchy
void MainWindow::on_objectList_clicked(const QModelIndex &index)
{
    // get the index and check the cache
    auto i = static_cast<unsigned>(index.row());
    if(i < mEntityDataCache.size())
    {
        // Cache currently selected entity
        currentEntitySelected = &mEntityDataCache[i];

        // Update the selected object name
        ui->lineEdit_SelectedObject->setText(QString::fromStdString(currentEntitySelected->name));

        // Update widgets
        updateComponentArea(currentEntitySelected->entityId);
    }
}

void MainWindow::on_actionEmpty_Object_triggered()
{
    mEntityManager->createObject(0);
}

void MainWindow::on_actionCube_triggered()
{
     mEntityManager->createObject(1);
}

void MainWindow::on_actionMonkey_triggered()
{
    mEntityManager->createObject(2);
}

void MainWindow::on_button_AddComponent_clicked()
{
    int index = ui->comboBox_Components->currentIndex();
    if(static_cast<size_t>(index) < mAvailableComponentsToAddCache.size() && currentEntitySelected)
    {
        auto id = currentEntitySelected->entityId;
        auto type = mAvailableComponentsToAddCache[static_cast<size_t>(index)];

        mEntityManager->addComponent(id, type);
        updateComponentArea(id);

        mAvailableComponentsToAddCache.erase(std::remove(mAvailableComponentsToAddCache.begin(), mAvailableComponentsToAddCache.end(), type), mAvailableComponentsToAddCache.end());
        updateAvailableComponents(mAvailableComponentsToAddCache);
    }
}

void MainWindow::updateComponentArea(unsigned int entityID)
{
    // Delete any previous component widgets
    if(ui->scrollArea->widget())
        delete ui->scrollArea->widget();

    // Get all available components
    mAvailableComponentsToAddCache = ComponentTypes;

    // Get the components for this entity
    std::vector<Component*> components;
    if(mEntityManager->getAllComponents(entityID, components))
    {
        // Components were found, add them

        QWidget* widget = new QWidget();
        ui->scrollArea->setWidget(widget);
        QVBoxLayout* layout = new QVBoxLayout(widget);
        widget->setLayout(layout);

        for(auto& component: components)
        {
            mAvailableComponentsToAddCache.erase(std::remove(mAvailableComponentsToAddCache.begin(), mAvailableComponentsToAddCache.end(), component->type), mAvailableComponentsToAddCache.end());

            switch (component->type)
            {
            case ComponentType::Mesh:
            {
                auto render = static_cast<MeshComponent*>(component);
                auto widget = new MeshWidget(this);
                connect(widget, &MeshWidget::widgetRemoved, this, &MainWindow::onWidgetRemoved);


                // Set up render widget here
                widget->update(render->meshData.mName);


                layout->addWidget(widget);
                break;
            }
            case ComponentType::Transform:
            {
                auto transform = static_cast<TransformComponent*>(component);
                auto widget = new TransformWidget(this);
                connect(widget, &TransformWidget::widgetRemoved, this, &MainWindow::onWidgetRemoved);


                // Set up transform widget here
                widget->update(transform->position, transform->rotation, transform->scale);


                layout->addWidget(widget);
                break;
            }
            case ComponentType::Input:
            {
                auto input = static_cast<InputComponent*>(component);
                auto widget = new InputWidget(this);
                connect(widget, &InputWidget::widgetRemoved, this, &MainWindow::onWidgetRemoved);


                // Set up input widget here
                widget->update(input->isCurrentlyControlled);


                layout->addWidget(widget);
                break;
            }
            default:
                break;
            }
        }
    }
    updateAvailableComponents(mAvailableComponentsToAddCache);
}

void MainWindow::updateAvailableComponents(std::vector<ComponentType> types)
{
    ui->comboBox_Components->clear();
    if(!types.size())
    {
        ui->button_AddComponent->setEnabled(false);
        ui->comboBox_Components->setEnabled(false);
        return;
    }

    ui->comboBox_Components->setEnabled(true);
    ui->button_AddComponent->setEnabled(true);

    for(auto& type : types)
    {
        switch (type)
        {
        case ComponentType::Mesh:
        {
            ui->comboBox_Components->addItem("Render");
            break;
        }
        case ComponentType::Transform:
        {
            ui->comboBox_Components->addItem("Transform");
            break;
        }
        case ComponentType::Input:
        {
            ui->comboBox_Components->addItem("Input");
            break;
        }
        default:
            break;
        }
    }
}

void MainWindow::on_lineEdit_SelectedObject_editingFinished()
{
    auto text = ui->lineEdit_SelectedObject->text();
    if(text.length())
    {
        if(currentEntitySelected)
        {
            currentEntitySelected->name = text.toStdString();
            for(unsigned i = 0; i < mEntityDataCache.size(); ++i)
            {
                if(mEntityDataCache[i].entityId == currentEntitySelected->entityId)
                {
                    if(i < static_cast<unsigned>(ui->objectList->count()))
                    {
                        ui->objectList->item(static_cast<int>(i))->setText(text);
                    }
                }
            }
        }
    }
}

void MainWindow::on_objectList_itemChanged(QListWidgetItem *item)
{
    for(int i = 0; i < ui->objectList->count(); ++i)
    {
        if(ui->objectList->item(i) == item)
        {
            if(static_cast<unsigned>(i) < mEntityDataCache.size())
            {
                auto entity = mEntityDataCache[static_cast<unsigned>(i)];
                entity.name = item->text().toStdString();
                if(currentEntitySelected)
                {
                    if(entity.entityId == currentEntitySelected->entityId)
                    {
                        ui->lineEdit_SelectedObject->setText(item->text());
                    }
                }
            }
        }
    }
}
