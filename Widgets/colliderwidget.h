#ifndef COLLIDERWIDGET_H
#define COLLIDERWIDGET_H

#include "componentwidget.h"

class ColliderComponent;
class TransformComponent;

namespace Ui{
    class Collider;
}

/** Widget for collidercomponents for the editor.
 * @brief Widget for collidercomponents for the editor.
 */
class ColliderWidget : public ComponentWidget
{
    Q_OBJECT

public:
    explicit ColliderWidget(MainWindow *mainWindow, QWidget* parent = nullptr);

private slots:
    void Remove() override;

    void on_comboBox_Colliders_currentIndexChanged(int index);

private:
    Ui::Collider* ui;

    void updateParameters();
    ColliderComponent* getColliderComponent();
    TransformComponent* getTransformComponent();
};

#endif // COLLIDERWIDGET_H
