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

private:
    Ui::Collider* ui;
};

#endif // COLLIDERWIDGET_H
