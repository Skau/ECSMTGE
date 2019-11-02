#include "componentwidget.h"
#include <QMenu>
#include "mainwindow.h"


ComponentWidget::ComponentWidget(MainWindow* mainWindow, QWidget* parent)
    : QWidget(parent), mMainWindow(mainWindow)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &ComponentWidget::ProvideContextMenu);
}

void ComponentWidget::ProvideContextMenu(const QPoint &point)
{
    QMenu subMenu;
    subMenu.addAction("Remove", this, &ComponentWidget::Remove);

    QPoint globalPos = mapToGlobal(point);

    subMenu.exec(globalPos);
}

