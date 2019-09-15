#include "innpch.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSurfaceFormat>
#include <QDesktopWidget>

#include <QTreeView>
#include <QTreeWidgetItem>

#include "renderer.h"
#include "Widgets/transformwidget.h"
#include "Widgets/meshwidget.h"
#include "Widgets/physicswidget.h"
#include "Widgets/inputwidget.h"
#include "componentdata.h"

#include <QSplitter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mRenderer = new Renderer();

    mRenderWindowContainer = QWidget::createWindowContainer(mRenderer);

    QVBoxLayout* layout = new QVBoxLayout(ui->OpenGLLayout);
    ui->OpenGLLayout->setLayout(layout);
    ui->OpenGLLayout->layout()->addWidget(mRenderWindowContainer);

    //ui->OpenGLLayout->addWidget(mRenderWindowContainer);
    mRenderWindowContainer->setFocus();

    resize(QDesktopWidget().availableGeometry(this).size() * 0.7);

    ui->treeWidget_ObjectList->setEditTriggers(QAbstractItemView::DoubleClicked);

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
    statusBar()->showMessage(" Time pr FrameDraw: "
                             + QString::number(static_cast<double>(deltaTime), 'g', 4)
                             + " s  |  " + "FPS: "
                             + QString::number(static_cast<double>(frameCounter), 'g', 4));
}

void MainWindow::setEntityManager(std::shared_ptr<EntityManager> entityManager)
{
    // Cach
    mEntityManager = entityManager;

    // Called from Entitymanager to refresh the tree widget showing all entities
    connect(mEntityManager.get(), &EntityManager::updateUI, this, &MainWindow::updateUI);
}

// If a widget is removed we need to recreate the components
void MainWindow::onWidgetRemoved()
{
    updateComponentArea(currentEntitySelected->entityId);
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

// Called from entity manager when a new entity is added to the scene
void MainWindow::updateUI(const std::vector<EntityData> &entityData)
{
    // Clear the map and widget
    mTreeDataCache.clear();
    ui->treeWidget_ObjectList->clear();


    // Create the new tree root
    QTreeWidgetItem* root = new QTreeWidgetItem(ui->treeWidget_ObjectList);
    root->setText(0, "SceneName");
    ui->treeWidget_ObjectList->addTopLevelItem(root);
    ui->treeWidget_ObjectList->expandAll();

    // Add all the entities
    for(unsigned i = 0; i < entityData.size(); ++i)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget_ObjectList->topLevelItem(0));
        item->setText(0, QString::fromStdString(entityData[i].name));
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        mTreeDataCache[item] = entityData[i];
    }
}

// Add the given entity components to the component area
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
        layout->setMargin(0);

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
                // (We send in the rotations as euler angles so that they are easy to work with in the editor)
                widget->update(transform->position, transform->rotation.toEuler(), transform->scale);


                layout->addWidget(widget);
                break;
            }
            case ComponentType::Physics:
            {
                auto physics = static_cast<PhysicsComponent*>(component);
                auto widget = new PhysicsWidget(this);
                connect(widget, &PhysicsWidget::widgetRemoved, this, &MainWindow::onWidgetRemoved);


                // Set up physics widget here
                widget->update(physics->velocity, physics->acceleration, physics->mass);


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

// Update the combo box at the bottom of the component area with the given types)
void MainWindow::updateAvailableComponents(std::vector<ComponentType> types)
{
    // Clear the currnet types
    ui->comboBox_Components->clear();

    // If none are given (aka the entity has all available components), disable
    // the combo box and button
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
        case ComponentType::Physics:
        {
            ui->comboBox_Components->addItem("Physics");
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

// Add the component in the combo box to the selected entity
void MainWindow::on_button_AddComponent_clicked()
{
    int index = ui->comboBox_Components->currentIndex();
    if(static_cast<size_t>(index) < mAvailableComponentsToAddCache.size() && currentEntitySelected)
    {
        // Get the entity id and the component type to add
        auto id = currentEntitySelected->entityId;
        auto type = mAvailableComponentsToAddCache[static_cast<size_t>(index)];

        // Add the component
        mEntityManager->addComponent(id, type);

        // Refresh the UI with the components that the entity noew has
        updateComponentArea(id);

        // Remove the just added component from the cache of available components to add
        mAvailableComponentsToAddCache.erase(std::remove(mAvailableComponentsToAddCache.begin(), mAvailableComponentsToAddCache.end(), type), mAvailableComponentsToAddCache.end());

        // Refresh the combo box
        updateAvailableComponents(mAvailableComponentsToAddCache);
    }
}

// When editing the name in the line edit under Selected Object
void MainWindow::on_lineEdit_SelectedObject_editingFinished()
{
    auto text = ui->lineEdit_SelectedObject->text();
    if(text.length())
    {
        if(currentEntitySelected)
        {
            currentEntitySelected->name = text.toStdString();

            // Loop through all QTreeWidgetItems and find the correct one
            for(auto data : mTreeDataCache)
            {
                // If found, set the text in the QTreeWidgetItem to the new name
                if(data.second.entityId == currentEntitySelected->entityId)
                {
                    data.first->setText(0, text);
                }
            }
        }
    }
}

// When editing the name directly in the tree widget
void MainWindow::on_treeWidget_ObjectList_itemChanged(QTreeWidgetItem *item, int /*column*/)
{
    if(mTreeDataCache.find(item) != mTreeDataCache.end())
    {
        // Get the entity data
        auto entity = mTreeDataCache[item];

        // If this is the selected object get the name and then set it in line edit under Selected Object text
        entity.name = item->text(0).toStdString();
        if(currentEntitySelected && entity.entityId == currentEntitySelected->entityId)
        {
        ui->lineEdit_SelectedObject->setText(item->text(0));
        }
    }
}

// When left clicking on an item in the tree (selecting an entity)
void MainWindow::on_treeWidget_ObjectList_itemClicked(QTreeWidgetItem *item, int /*column*/)
{
    if(mTreeDataCache.find(item) != mTreeDataCache.end())
    {
        // Cache currently selected entity
        currentEntitySelected = &mTreeDataCache[item];

        // Update the selected object name
        ui->lineEdit_SelectedObject->setText(QString::fromStdString(currentEntitySelected->name));

        // Update widgets
        updateComponentArea(currentEntitySelected->entityId);
    }
}
