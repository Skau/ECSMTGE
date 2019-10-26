#include "colliderwidget.h"
#include "ui_collider.h"
#include "mainwindow.h"
#include "entitymanager.h"
#include <QDoubleSpinBox>
#include <memory>

ColliderWidget::ColliderWidget(MainWindow *mainWindow, QWidget *parent)
    : ComponentWidget(mainWindow, parent), ui(new Ui::Collider)
{
    ui->setupUi(this);

    for (auto type : ColliderComponent::typeNames)
    {
       ui->comboBox_Colliders->addItem(type);
    }
}

void ColliderWidget::updateData()
{

}

void ColliderWidget::onSelected()
{
    if (auto entity = mMainWindow->currentEntitySelected)
    {
        auto comp = mMainWindow->getEntityManager()->getComponent<ColliderComponent>(entity->entityId);
        if (comp) {
            auto index = static_cast<int>(comp->collisionType);
            lastHighlighted = index;
            ui->comboBox_Colliders->setCurrentIndex(index);
            ready = true;
        }
    }
}

void ColliderWidget::Remove()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(mMainWindow->getEntityManager()->removeComponent<ColliderComponent>(entity->entityId))
        {
            widgetRemoved(this);
        }
    }
}

void ColliderWidget::on_comboBox_Colliders_currentIndexChanged(int index)
{
    if (!ready)
        return;

    if (auto entity = mMainWindow->currentEntitySelected)
    {
        auto comp = mMainWindow->getEntityManager()->getComponent<ColliderComponent>(entity->entityId);
        auto currentIndex = static_cast<int>(comp->collisionType);
        if (lastHighlighted == index && currentIndex != index)
        {
            qDebug() << entity->entityId << "'s colldertype changed to " << index;
            comp->collisionType = static_cast<ColliderComponent::Type>(index);
            updateParameters();
        }
    }
}

void ColliderWidget::on_comboBox_Colliders_highlighted(int index)
{
    lastHighlighted = index;
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

    if (auto comp = getColliderComponent()) {

        QVBoxLayout* layout = new QVBoxLayout(ui->widget_Parameters);
        ui->widget_Parameters->setLayout(layout);
        layout->setMargin(0);

    //    float minimumHeight = 0.f;

        QWidget* widget = new QWidget();
        QHBoxLayout* hLayout = new QHBoxLayout();
        widget->setLayout(hLayout);

        switch (comp->collisionType)
        {
            case ColliderComponent::AABB:
            {
                qDebug() << "type was AABB!";

                auto extents = std::get<gsl::vec3>(comp->extents);

                int i = 0;

                QLabel* label = new QLabel{"x:", widget};
                hLayout->addWidget(label);
                auto spinBox = new QDoubleSpinBox{widget};
                spinBox->setValue(static_cast<double>(extents[i]));
                connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                [=](double d)
                {
                    if (auto collider = getColliderComponent())
                        if (auto extents = std::get_if<gsl::vec3>(&collider->extents))
                            (*extents)[i] = static_cast<float>(d);
                });
                hLayout->addWidget(spinBox);

                i = 1;

                label = new QLabel{"y:", widget};
                hLayout->addWidget(label);
                spinBox = new QDoubleSpinBox{widget};
                spinBox->setValue(static_cast<double>(extents[i]));
                connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                [=](double d)
                {
                    if (auto collider = getColliderComponent())
                        if (auto extents = std::get_if<gsl::vec3>(&collider->extents))
                            (*extents)[i] = static_cast<float>(d);
                });
                hLayout->addWidget(spinBox);

                i = 2;

                label = new QLabel{"z:", widget};
                hLayout->addWidget(label);
                spinBox = new QDoubleSpinBox{widget};
                spinBox->setValue(static_cast<double>(extents[i]));
                connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                [=](double d)
                {
                    if (auto collider = getColliderComponent())
                        if (auto extents = std::get_if<gsl::vec3>(&collider->extents))
                            (*extents)[i] = static_cast<float>(d);
                });
                hLayout->addWidget(spinBox);
                break;
            }
            case ColliderComponent::BOX:
                qDebug() << "type was Box!";
            break;
            case ColliderComponent::SPHERE:
            qDebug() << "type was Sphere!";
            break;
            case ColliderComponent::CAPSULE:
            qDebug() << "type was Capsule!";
            break;
            default:
            qDebug() << "No type!";
                break;
        }

        layout->addWidget(widget);
        ui->widget_Parameters->setMinimumHeight(100);
    }
}

ColliderComponent *ColliderWidget::getColliderComponent()
{
    if (auto entity = mMainWindow->currentEntitySelected)
        if (auto comp = mMainWindow->getEntityManager()->getComponent<ColliderComponent>(entity->entityId))
            return comp;

    return nullptr;
}
