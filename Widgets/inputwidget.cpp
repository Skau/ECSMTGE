#include "inputwidget.h"
#include "mainwindow.h"
#include "ui_input.h"
#include "entitymanager.h"
#include <QMenu>

InputWidget::InputWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent), ui(new Ui::Input), mMainWindow(mainWindow)
{
    ui->setupUi(this);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &InputWidget::ProvideContextMenu);
}

void InputWidget::update(bool isControlled)
{
    auto checkState = isControlled ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
    ui->checkBox_IsBeingControlled->setCheckState(checkState);
}

void InputWidget::ProvideContextMenu(const QPoint &point)
{
    QMenu subMenu;
    subMenu.addAction("Remove", this, &InputWidget::Remove);

    QPoint globalPos = mapToGlobal(point);

    subMenu.exec(globalPos);
}

void InputWidget::Remove()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(mMainWindow->getEntityManager()->removeComponent<InputComponent>(entity->entityId))
        {
            widgetRemoved();
        }
    }
}

void InputWidget::on_checkBox_IsBeingControlled_toggled(bool checked)
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto comp = mMainWindow->getEntityManager()->getComponent<InputComponent>(entity->entityId))
        {
            comp->isCurrentlyControlled = checked;
        }
    }
}
