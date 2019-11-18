#include "scriptwidget.h"
#include "ui_script.h"
#include "mainwindow.h"
#include "world.h"
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
        if(auto comp = World::getWorld().getEntityManager()->getComponent<ScriptComponent>(entity->entityId))
        {
            auto filePath = comp->filePath;
            if(filePath.size())
            {
                ui->lineEdit->setText(QString::fromStdString(comp->filePath));
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
        if(World::getWorld().getEntityManager()->removeComponent<ScriptComponent>(entity->entityId))
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
        if(auto comp = World::getWorld().getEntityManager()->getComponent<ScriptComponent>(entity->entityId))
        {
            QFileInfo info(fileName);
            auto name = QString::fromStdString(gsl::scriptsFilePath) + info.baseName() + ".js";
            ui->lineEdit->setText(info.baseName() + ".js");
            ScriptSystem::get()->load(*comp, info.baseName().toStdString() + ".js");
        }
    }
}

void ScriptWidget::on_button_ExecuteJS_clicked()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto comp = World::getWorld().getEntityManager()->getComponent<ScriptComponent>(entity->entityId))
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
            text.prepend(ScriptSystem::get()->getEntityHelperFunctions());
            text.prepend("function foo(){");
            text.append("}");
            QFile file("foo.js");
            file.open(QIODevice::ReadWrite);
            QTextStream stream(&file);
            stream << text;
            file.close();

            if(ScriptSystem::get()->execute(*comp, "foo", text, "foo.js"))
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
        if(auto comp = World::getWorld().getEntityManager()->getComponent<ScriptComponent>(entity->entityId))
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
            // Base template for new files. Includes the functions beginPlay, tick, endPlay. keyPressed and onHit.
            stream << "// This will be run once when play button is pressed\n"
                   << "function beginPlay()\n{\n\t//console.log(\"Begin play called on entity \" + me.ID);\n}\n\n"
                   << "// This will be run once every frame\n"
                   << "function tick(deltaTime)\n{\n\t//console.log(\"Tick called on entity \" + me.ID);\n}\n\n"
                   << "// This will be run once when stop button is pressed\n"
                   << "function endPlay()\n{\n\t//console.log(\"End play called on entity \" + me.ID);\n}\n\n"
                   << "// This will be run when input is pressed\n"
                   << "// NOTE:\n// This requires that the entity has an Input component\n// and control while playing is true\n"
                   << "function inputPressed(inputs)\n{\n\t//for (let i = 0; i < inputs.length; i++)\n\t//{\n\t\t//console.log(inputs[i] + \" pressed\");\n\t//}\n}\n\n"
                   << "// This will be run when input is relased\n"
                   << "// NOTE:\n// This requires that the entity has an Input component\n// and control while playing is true\n"
                   << "function inputReleased(inputs)\n{\n\t//for (let i = 0; i < inputs.length; i++)\n\t//{\n\t\t//console.log(inputs[i] + \" released\");\n\t//}\n}\n\n"
                   << "function mouseMoved(offset)\n{\n\t//console.log(\"Mouse offset: x: \" + offset[0] + \", y: \" + offset[1]);\n}\n\n"
                   << "// This will be run when collision with another entity occurs\n"
                   << "function onHit(hitInfo)\n{\n\t//console.log(\"Collided with entity ID: \" + hitInfo.ID);\n}\n";
            file.close();
            QFileInfo fileInfo(file.fileName());
            ScriptSystem::get()->load(*comp, fileInfo.baseName().toStdString() + ".js");
            ui->lineEdit->setText(fileInfo.baseName());
            ui->button_NewFile->setText("Open file");
        }
    }
}
