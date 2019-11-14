#include "colliderwidget.h"
#include "ui_collider.h"
#include "mainwindow.h"
#include "world.h"
#include <QDoubleSpinBox>


ColliderWidget::ColliderWidget(MainWindow *mainWindow, QWidget *parent)
    : ComponentWidget(mainWindow, parent), ui(new Ui::Collider)
{
    ui->setupUi(this);

    isUpdating = true;

    for (auto type : ColliderComponent::typeNames)
    {
       ui->comboBox_Colliders->addItem(type);
    }

    auto comp = getColliderComponent();
    if (comp)
    {
        ui->comboBox_Colliders->setCurrentIndex(static_cast<int>(comp->collisionType));
        updateParameters();
    }
    isUpdating = false;
}

void ColliderWidget::updateData()
{
//    auto entity = mMainWindow->currentEntitySelected;
//    if(entity)
//    {
//        if(auto comp =  getColliderComponent())
//        {
//            isUpdating = true;

//            isUpdating = false;
//        }
//    }
}

void ColliderWidget::Remove()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(World::getWorld().getEntityManager()->removeComponent<ColliderComponent>(entity->entityId))
        {
            widgetRemoved(this);
        }
    }
}

void ColliderWidget::on_comboBox_Colliders_currentIndexChanged(int index)
{
    if(isUpdating) return;

    if(auto comp = getColliderComponent())
    {
        comp->collisionType = static_cast<ColliderComponent::Type>(index);
        switch (comp->collisionType)
        {
            case ColliderComponent::AABB:
            case ColliderComponent::BOX:
                comp->extents = gsl::vec3{1.f, 1.f, 1.f};
                break;
            case ColliderComponent::SPHERE:
                comp->extents = 0.5f;
                break;
            case ColliderComponent::CAPSULE:
                comp->extents = std::pair<float, float>{0.5f, 1.f};
                break;
            default:
                break;
        }

        // Remember to update bounds
        if (auto trans = getTransformComponent())
            trans->colliderBoundsOutdated = true;

        updateParameters();
    }
}

void ColliderWidget::updateParameters()
{
    if (ui->widget_Parameters->children().size())
    {
        for (auto& child : ui->widget_Parameters->children())
        {
            delete child;
        }
    }

    if (ui->widget_Parameters->isHidden())
        ui->widget_Parameters->show();

    if (auto comp = getColliderComponent())
    {
        QVBoxLayout* layout = new QVBoxLayout(ui->widget_Parameters);
        ui->widget_Parameters->setLayout(layout);
        layout->setMargin(0);

        std::vector<QWidget*> widgets{};

        switch (comp->collisionType)
        {
            case ColliderComponent::AABB:
            case ColliderComponent::BOX:
            {
                widgets.push_back(new QLabel{"Centre:", ui->widget_Parameters});

                widgets.push_back(new QWidget{ui->widget_Parameters});
                auto &widget = widgets.back();
                QHBoxLayout* hLayout = new QHBoxLayout();
                widget->setLayout(hLayout);

                auto extents = std::get<gsl::vec3>(comp->extents);

                for (int i = 0; i < 3; ++i)
                {
                    auto name = std::string{static_cast<char>('x' + i)}.append(":");
                    QLabel* label = new QLabel{name.c_str(), widget};
                    hLayout->addWidget(label);
                    auto spinBox = new QDoubleSpinBox{widget};
                    spinBox->setValue(static_cast<double>(extents[i]));
                    connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                    [=](double d)
                    {
                        if (auto collider = getColliderComponent())
                            if (auto extents = std::get_if<gsl::vec3>(&collider->extents))
                            {
                                (*extents)[i] = static_cast<float>(d);
                                if (auto trans = getTransformComponent())
                                    trans->colliderBoundsOutdated = true;
                            }
                    });
                    hLayout->addWidget(spinBox);
                }

                ui->widget_Parameters->setMinimumHeight(20);
            }
            break;
            case ColliderComponent::SPHERE:
            {
                widgets.push_back(new QWidget{ui->widget_Parameters});
                auto &widget = widgets.back();
                QHBoxLayout* hLayout = new QHBoxLayout();
                widget->setLayout(hLayout);

                auto extents = std::get<float>(comp->extents);

                QLabel* label = new QLabel{"Radius: ", widget};
                hLayout->addWidget(label);
                auto spinBox = new QDoubleSpinBox{widget};
                spinBox->setValue(static_cast<double>(extents));
                connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                [=](double d)
                {
                    if (auto collider = getColliderComponent())
                        if (auto extents = std::get_if<float>(&collider->extents))
                        {
                            *extents = static_cast<float>(d);
                            if (auto trans = getTransformComponent())
                                trans->colliderBoundsOutdated = true;
                        }
                });
                hLayout->addWidget(spinBox);

                ui->widget_Parameters->setMinimumHeight(10);
            }
            break;
            case ColliderComponent::CAPSULE:
            {
                auto& extents = std::get<std::pair<float, float>>(comp->extents);
                float* extentsFloats[]{&extents.first, &extents.second};
                const char* labelNames[]{"Radius:", "Half-height:"};

                for (int i = 0; i < 2; ++i)
                {

                    widgets.push_back(new QWidget{ui->widget_Parameters});
                    auto &widget = widgets.back();
                    QHBoxLayout* hLayout = new QHBoxLayout();
                    widget->setLayout(hLayout);


                    QLabel* label = new QLabel{labelNames[i], widget};
                    hLayout->addWidget(label);
                    auto spinBox = new QDoubleSpinBox{widget};
                    spinBox->setValue(static_cast<double>(*extentsFloats[i]));
                    connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                    [=](double d)
                    {
                        if (auto collider = getColliderComponent())
                            if (auto extents = std::get_if<std::pair<float, float>>(&collider->extents))
                            {
                                *(extentsFloats[i]) = static_cast<float>(d);
                                if (auto trans = getTransformComponent())
                                    trans->colliderBoundsOutdated = true;
                            }
                    });
                    hLayout->addWidget(spinBox);
                }

                ui->widget_Parameters->setMinimumHeight(20);
            }
            break;
            default:
                break;
        }

        for (auto widget : widgets)
            if (widget != nullptr)
                layout->addWidget(widget);
    }
}

ColliderComponent *ColliderWidget::getColliderComponent()
{
    if (auto entity = mMainWindow->currentEntitySelected)
        if (auto comp = World::getWorld().getEntityManager()->getComponent<ColliderComponent>(entity->entityId))
            return comp;

    return nullptr;
}

TransformComponent *ColliderWidget::getTransformComponent()
{
    if (auto entity = mMainWindow->currentEntitySelected)
        if (auto comp = World::getWorld().getEntityManager()->getComponent<TransformComponent>(entity->entityId))
            return comp;

    return nullptr;
}
