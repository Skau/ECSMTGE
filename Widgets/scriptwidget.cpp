#include "scriptwidget.h"
#include "ui_script.h"
#include "mainwindow.h"
#include "entitymanager.h"
#include <QFileDialog>
#include "scriptsystem.h"

ScriptWidget::ScriptWidget(MainWindow* mainWindow, QWidget *parent) :
   ComponentWidget(mainWindow, parent), ui(new Ui::ScriptWidget)
{
    ui->setupUi(this);
}

ScriptWidget::~ScriptWidget()
{
    delete ui;
}

void ScriptWidget::updateData()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto comp = mMainWindow->getEntityManager()->getComponent<ScriptComponent>(entity->entityId))
        {
            isUpdating = true;
            ui->lineEdit->setText(QString::fromStdString(comp->filePath));
            isUpdating = false;
        }
    }
}

void ScriptWidget::Remove()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(mMainWindow->getEntityManager()->removeComponent<ScriptComponent>(entity->entityId))
        {
            widgetRemoved(this);
        }
    }
}

void ScriptWidget::on_toolButton_clicked()
{
    auto fileName = QFileDialog::getOpenFileName(this, tr("Choose"), QString::fromStdString(gsl::scriptsFilePath), tr("Script files (*.js)"));
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto comp = mMainWindow->getEntityManager()->getComponent<ScriptComponent>(entity->entityId))
        {
            ui->lineEdit->setText(fileName);
            comp->filePath = fileName.toStdString();
        }
    }
}

void ScriptWidget::on_button_ExecuteJS_clicked()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto comp = mMainWindow->getEntityManager()->getComponent<ScriptComponent>(entity->entityId))
        {
            ui->label_Status->clear();
            auto text = ui->textEdit_JSCode->toPlainText();
            if(!text.size())
            {
                ui->label_Status->setText("Error: No text entered.");
            }

            text.prepend("function foo(){");
            text.append("}");
            QFile file("foo.js");
            file.open(QIODevice::ReadWrite);
            QTextStream stream(&file);
            stream << text;
            file.close();

            ScriptSystem::get()->call("foo", text, "foo.js", *comp);

            //ui->label_Status->setText("Successfully called");
        }
    }

}
