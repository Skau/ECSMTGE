#include "inputwidget.h"
#include "mainwindow.h"
#include "ui_input.h"
#include "entitymanager.h"

InputWidget::InputWidget(MainWindow *mainWindow, QWidget *parent)
    : ComponentWidget(mainWindow, parent), ui(new Ui::Input)
{
    ui->setupUi(this);
}


void InputWidget::update(bool isControlled)
{
    auto checkState = isControlled ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
    ui->checkBox_IsBeingControlled->setCheckState(checkState);
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
