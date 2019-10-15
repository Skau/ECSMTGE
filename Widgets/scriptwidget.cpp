#include "scriptwidget.h"
#include "ui_script.h"
#include "mainwindow.h"
#include "entitymanager.h"
#include <QFileDialog>
#include "constants.h"
#include "scriptsystem.h"
#include <QDesktopServices>

ScriptWidget::ScriptWidget(MainWindow* mainWindow, QWidget *parent) :
   ComponentWidget(mainWindow, parent), ui(new Ui::ScriptWidget)
{
    ui->setupUi(this);

    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto comp = mMainWindow->getEntityManager()->getComponent<ScriptComponent>(entity->entityId))
        {
            auto filePath = comp->getFilePath();
            if(filePath.size())
            {
                ui->lineEdit->setText(QString::fromStdString(comp->getFilePath()));
                ui->button_NewFile->setText("Open file");
            }
            else
            {
                ui->button_NewFile->setText("Create file");
            }
        }
    }
}

ScriptWidget::~ScriptWidget()
{
    delete ui;
}

void ScriptWidget::updateData()
{
//    auto entity = mMainWindow->currentEntitySelected;
//    if(entity)
//    {
//        if(auto comp = mMainWindow->getEntityManager()->getComponent<ScriptComponent>(entity->entityId))
//        {
//            isUpdating = true;
//            isUpdating = false;
//        }
//    }
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
    if(!fileName.size())
    {
        return;
    }

    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto comp = mMainWindow->getEntityManager()->getComponent<ScriptComponent>(entity->entityId))
        {
            ui->lineEdit->setText(fileName);
            comp->load(fileName.toStdString());
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

            /**
            * To make this work we have to do two things:
            * 1. Wrap the raw js code in a function (here just called foo)
            * 2. Create a temp file in the working directory (here called foo.js) and add the text
            * After the two steps are done, we can call the function
            */

            QString s =
            "function getComponent(name)"\
            "{"\
            "var comp = entity.getComponent(name);"\
            "if(comp)"\
            "{"\
            "accessedComponents.push(comp);"\
            "}"\
            "return comp"\
            "}";

            text.prepend(s);
            text.prepend("function foo(){");
            text.append("}");
            QFile file("foo.js");
            file.open(QIODevice::ReadWrite);
            QTextStream stream(&file);
            stream << text;
            file.close();

            if(comp->execute("foo", text, "foo.js"))
            {
                if(ui->label_Status)
                    ui->label_Status->setText("Execute successful");
            }
            else
            {
                if(ui->label_Status)
                    ui->label_Status->setText("Execute failed");
            }
        }
    }

}

// Will either open an existing file or create a new one and give it to the entity if none are available
void ScriptWidget::on_button_NewFile_clicked()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto comp = mMainWindow->getEntityManager()->getComponent<ScriptComponent>(entity->entityId))
        {
            // If a .js file already exists, open it instead in the default text editor
            auto filePath = ui->lineEdit->text();
            if(filePath.length())
            {
                QFileInfo info(filePath);
                QDesktopServices::openUrl(QUrl::fromLocalFile(info.absoluteFilePath()));
                return;
            }

            if(!QDir(QString::fromStdString(gsl::scriptsFilePath)).exists())
            {
                QDir().mkdir(QString::fromStdString(gsl::scriptsFilePath));
            }

            // Simple way to append a unique number to the end of the file, so it doesn't overwrite existing files
            QString fileName = "NewScript";
            if(QFile::exists(QString::fromStdString(gsl::scriptsFilePath) + fileName + ".js"))
            {
                for(int i = 1; ;i++)
                {
                     if(i > 1)
                     {
                         fileName = fileName.remove(fileName.length()-1, 1);
                         if(i > 10)
                         {
                             fileName = fileName.remove(fileName.length()-1, 1);
                         }

                     }
                     fileName.append(QString::number(i));
                     if(!QFile::exists(QString::fromStdString(gsl::scriptsFilePath) + fileName + ".js"))
                     {
                         break;
                     }
                }
            }

            // Creates a new file with the given name and adds base functionality to it
            QFile file(QString::fromStdString(gsl::scriptsFilePath) + fileName + ".js");
            if(!file.open(QIODevice::WriteOnly))
            {
                qDebug() << "Failed to open file " << file;
                return;
            }
            QTextStream stream(&file);
            // Base template for new files. Includes functions beginPlay, tick and endPlay.
            stream << "// This will be run once when play button is pressed\n"
                   << "function beginPlay()\n{\n\tconsole.log(\"Begin play called on entity \" + entity.ID);\n}\n\n"
                   << "// This will be once run every frame\n"
                   << "function tick(deltaTime)\n{\n\tconsole.log(\"Tick called on entity \" + entity.ID);\n}\n\n"
                   << "// This will be run once when stop button is pressed\n"
                   << "function endPlay()\n{\n\tconsole.log(\"End play called on entity \" + entity.ID);\n}";
            file.close();
            QFileInfo fileInfo(file.fileName());
            comp->load(fileInfo.filePath().toStdString());
            ui->lineEdit->setText(QString::fromStdString(comp->getFilePath()));
            ui->button_NewFile->setText("Open file");
        }
    }
}
