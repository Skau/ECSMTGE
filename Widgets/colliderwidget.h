#ifndef COLLIDERWIDGET_H
#define COLLIDERWIDGET_H

#include "componentwidget.h"

class ColliderComponent;
class TransformComponent;

namespace Ui{
    class Collider;
}

class ColliderWidget : public ComponentWidget
{
    Q_OBJECT

public:
    explicit ColliderWidget(MainWindow *mainWindow, QWidget* parent = nullptr);

    void updateData() override;
    void onSelected() override;

private slots:

    void Remove() override;

    void on_comboBox_Colliders_currentIndexChanged(int index);

    void on_comboBox_Colliders_highlighted(int index);

private:
    Ui::Collider* ui;

    bool ready{false};
    int lastHighlighted{0};

    void updateParameters();
    ColliderComponent* getColliderComponent();
    TransformComponent* getTransformComponent();
};

#endif // COLLIDERWIDGET_H
