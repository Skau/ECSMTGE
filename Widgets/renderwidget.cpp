#include "renderwidget.h"
#include "ui_render.h"
#include "QFileDialog"
#include "constants.h"
#include <QDebug>
#include <QMenu>
#include "mainwindow.h"
#include "entitymanager.h"
#include "componentdata.h"

RenderWidget::RenderWidget(MainWindow *mainWindow, QWidget* parent)
    : QWidget(parent), ui(new Ui::Render), mMainWindow(mainWindow)
{
    ui->setupUi(this);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &RenderWidget::ProvideContextMenu);
}

RenderWidget::~RenderWidget()
{

}

void RenderWidget::update(const std::string &name)
{
    ui->label_Name->setText(QString::fromStdString(name));

    if(auto render = getRenderComponent(mMainWindow->currentEntitySelected->entityId))
    {
        if(render->isVisible)
        {
             ui->checkBox_Visible->setCheckState(Qt::CheckState::Checked);
        }
        else
        {
            ui->checkBox_Visible->setCheckState(Qt::CheckState::Unchecked);
        }
    }
}

void RenderWidget::on_button_ChangeMesh_clicked()
{
    if(mMainWindow->currentEntitySelected)
    {
        auto fileName = QFileDialog::getOpenFileName(this, tr("Choose"), QString::fromStdString(gsl::assetFilePath), tr("Mesh files (*.obj *.txt)"));
        auto splits = fileName.split('/');
        auto last = splits[splits.size()-1];
        splits = last.split('.');
        auto name = splits[0];
        if(name.length())
        {
            mMainWindow->getEntityManager()->setMesh(mMainWindow->currentEntitySelected->entityId, name.toStdString());
            update(name.toStdString());
        }
    }
}

void RenderWidget::on_checkBox_Visible_toggled(bool checked)
{
    if(mMainWindow->currentEntitySelected)
    {
        if(auto render = getRenderComponent(mMainWindow->currentEntitySelected->entityId))
        {
            render->isVisible = checked;
        }
    }
}

void RenderWidget::ProvideContextMenu(const QPoint &point)
{
    QMenu subMenu;
    subMenu.addAction("Remove", this, &RenderWidget::Remove);

    QPoint globalPos = mapToGlobal(point);

    subMenu.exec(globalPos);
}

void RenderWidget::Remove()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(mMainWindow->getEntityManager()->removeComponent<Render>(entity->entityId))
        {
            widgetRemoved();
        }
    }
}

Render *RenderWidget::getRenderComponent(unsigned int entity)
{
    return mMainWindow->getEntityManager()->getComponent<Render>(entity);
}
