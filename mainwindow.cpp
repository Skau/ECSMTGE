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
#include "Widgets/soundwidget.h"
#include "Widgets/directionallightwidget.h"
#include "Widgets/pointlightwidget.h"
#include "Widgets/spotlightwidget.h"
#include "Widgets/scriptwidget.h"
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

    ui->treeWidget_ObjectList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeWidget_ObjectList->setDragEnabled(true);
    ui->treeWidget_ObjectList->viewport()->setAcceptDrops(true);
    ui->treeWidget_ObjectList->setDropIndicatorShown(true);
    ui->treeWidget_ObjectList->setDragDropMode(QAbstractItemView::InternalMove);
    ui->treeWidget_ObjectList->setMainWindow(this);

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

void MainWindow::updateStatusBar(int vertices, float deltaTime, float frameCounter)
{
    statusBar()->showMessage("Vertices drawn: " +
                             QString::number(vertices) +
                             " | Time pr FrameDraw: "
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

EntityInfo* MainWindow::getEntityAt(QTreeWidgetItem* item)
{
    if(mTreeDataCache.find(item) != mTreeDataCache.end())
    {
        return &mTreeDataCache[item];
    }

    return nullptr;
}

void MainWindow::updateComponentWidgets()
{
    if(currentEntitySelected)
    {
        for(auto& component : mCurrentComponentWidgets)
        {
            component->updateData();
        }
    }
}

// If a widget is removed we need to recreate the components
void MainWindow::onWidgetRemoved(ComponentWidget* widget)
{
    mCurrentComponentWidgets.erase(std::remove(mCurrentComponentWidgets.begin(), mCurrentComponentWidgets.end(), widget), mCurrentComponentWidgets.end());
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

void MainWindow::on_actionPlay_triggered(bool value)
{
    if(value)
    {
        play();
        ui->actionPlay->setText("Stop");
    }
    else
    {
        stop();
        ui->actionPlay->setText("Play");
    }
}

// Called from entity manager when a new entity is added to the scene
void MainWindow::updateUI(const std::vector<EntityInfo> &entityData)
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

    mCurrentComponentWidgets.clear();

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

        ComponentWidget* componentWidget = nullptr;

        for(auto& component: components)
        {
            mAvailableComponentsToAddCache.erase(std::remove(mAvailableComponentsToAddCache.begin(), mAvailableComponentsToAddCache.end(), component->type), mAvailableComponentsToAddCache.end());

            switch (component->type)
            {
            case ComponentType::Mesh:
            {
                componentWidget = new MeshWidget(this);
                break;
            }
            case ComponentType::Transform:
            {
                componentWidget = new TransformWidget(this);
                break;
            }
            case ComponentType::Physics:
            {
                componentWidget = new PhysicsWidget(this);
                break;
            }
            case ComponentType::Input:
            {
                componentWidget = new InputWidget(this);
                break;
            }
            case ComponentType::Sound:
            {
                componentWidget = new SoundWidget(this);
                break;
            }
            case ComponentType::LightSpot:
            {
                componentWidget = new SpotLightWidget(this);
                break;
            }
            case ComponentType::LightPoint:
            {
                componentWidget = new PointLightWidget(this);
                break;
            }
            case ComponentType::LightDirectional:
            {
                componentWidget = new DirectionalLightWidget(this);
                break;
            }
            case ComponentType::Script:
            {
                componentWidget = new ScriptWidget(this);
                break;
            }
            default:
                break;
            }

            if(componentWidget)
            {
                connect(componentWidget, &ComponentWidget::widgetRemoved, this, &MainWindow::onWidgetRemoved);
                componentWidget->updateData();
                layout->addWidget(componentWidget);
                mCurrentComponentWidgets.push_back(componentWidget);
                componentWidget = nullptr;
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
        case ComponentType::Sound:
        {
            ui->comboBox_Components->addItem("Sound");
            break;
        }
        case ComponentType::LightSpot:
        {
            ui->comboBox_Components->addItem("Spot light");
            break;
        }
        case ComponentType::LightPoint:
        {
            ui->comboBox_Components->addItem("Point light");
            break;
        }
        case ComponentType::LightDirectional:
        {
            ui->comboBox_Components->addItem("Directional light");
            break;
        }
        case ComponentType::Script:
        {
            ui->comboBox_Components->addItem("Script");
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
            currentEntitySelected->name = item->text(0).toStdString();
        }
    }
}

// When left clicking on an item in the tree (selecting an entity)
void MainWindow::setSelected(EntityInfo* entityInfo)
{
    if (entityInfo == nullptr)
        return;

    // Cache currently selected entity
    currentEntitySelected = entityInfo;

    // Update the selected object name
    ui->lineEdit_SelectedObject->setText(QString::fromStdString(currentEntitySelected->name));

    // Update widgets
    updateComponentArea(currentEntitySelected->entityId);

    if (auto renderer = getRenderer())
        renderer->EditorCurrentEntitySelected = currentEntitySelected;
}

void MainWindow::on_treeWidget_ObjectList_itemClicked(QTreeWidgetItem *item, int /*column*/)
{
    if(mTreeDataCache.find(item) != mTreeDataCache.end())
    {
        setSelected(&mTreeDataCache[item]);
    }
}

void MainWindow::on_actionToggle_wireframe_triggered(bool checked)
{
    toggleWireframe(checked);
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionToggle_shutup_triggered(bool checked)
{
    shutUp(checked);
}

void MainWindow::on_actionSave_triggered()
{
    save();
}

void MainWindow::on_actionLoad_triggered()
{
    load();
}
