#ifndef COLLIDERWIDGET_H
#define COLLIDERWIDGET_H

#include "componentwidget.h"

namespace Ui{
    class Collider;
}

class ColliderWidget : public ComponentWidget
{
    Q_OBJECT

public:
    explicit ColliderWidget(MainWindow *mainWindow, QWidget* parent = nullptr);

    void updateData() override;

private slots:

    void Remove() override;

    void on_comboBox_Colliders_currentIndexChanged(int index);

    void on_comboBox_Colliders_highlighted(int index);

private:
    Ui::Collider* ui;

    int lastHighlighted{0};
    int currentIndex{0};

    void updateParameters();
};

#endif // COLLIDERWIDGET_H
