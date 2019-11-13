#include "inputwidget.h"
#include "mainwindow.h"
#include "ui_input.h"
#include "world.h"

InputWidget::InputWidget(MainWindow *mainWindow, QWidget *parent)
    : ComponentWidget(mainWindow, parent), ui(new Ui::Input)
{
    ui->setupUi(this);

    isUpdating = true;
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto input = World::getWorld().getEntityManager()->getComponent<InputComponent>(entity->entityId))
        {
            auto checkState = input->controlledWhilePlaying ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
            ui->checkBox_IsBeingControlled->setCheckState(checkState);
        }
    }
    isUpdating = false;
}

void InputWidget::updateData()
{
//    auto entity = mMainWindow->currentEntitySelected;
//    if(entity)
//    {
//        if(auto input = mMainWindow->getEntityManager()->getComponent<InputComponent>(entity->entityId))
//        {
//            isUpdating = true;

//            isUpdating = false;
//        }
//    }
}

void InputWidget::on_checkBox_IsBeingControlled_toggled(bool checked)
{
    if(isUpdating) return;

    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto comp = World::getWorld().getEntityManager()->getComponent<InputComponent>(entity->entityId))
        {
            comp->controlledWhilePlaying = checked;
        }
    }
}

void InputWidget::Remove()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(World::getWorld().getEntityManager()->removeComponent<InputComponent>(entity->entityId))
        {
            widgetRemoved(this);
        }
    }
}
