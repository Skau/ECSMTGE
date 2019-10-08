#include "soundwidget.h"
#include "mainwindow.h"
#include "ui_sound.h"
#include "entitymanager.h"
#include "soundmanager.h"
#include "resourcemanager.h"
#include <QFileDialog>

SoundWidget::SoundWidget(MainWindow* mainWindow, QWidget* parent)
    : ComponentWidget(mainWindow, parent), ui(new Ui::Sound)
{
    ui->setupUi(this);

    ui->comboBox_AvailableSounds->addItem("None");
    for(auto& name : ResourceManager::instance()->getAllWavFileNames())
    {
        ui->comboBox_AvailableSounds->addItem(QString::fromStdString(name));
    }

    if(auto sound = getSoundComponent())
    {
        auto name = sound->name;
        if(name.size())
        {
            ui->label_CurrentSound->setText(QString::fromStdString(name));
            ui->comboBox_AvailableSounds->setCurrentText(QString::fromStdString(name));
        }

        float value = sound->gain;
        ui->horizontalSlider_Gain->setValue(static_cast<int>(value*10.f));
        ui->label_GainValue->setText(QString::number(static_cast<double>(value)));

        ui->doubleSpinBox_Pitch->setValue(static_cast<double>(sound->pitch));

        ui->checkBox_Loop->setCheckState(sound->isLooping ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
        ui->checkBox_Mute->setCheckState(sound->isMuted ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    }
}


void SoundWidget::updateData()
{
//    if(auto sound = getSoundComponent())
//    {
//        isUpdating = true;
//        isUpdating = false;
//    }
}

void SoundWidget::Remove()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(mMainWindow->getEntityManager()->removeComponent<SoundComponent>(entity->entityId))
        {
            widgetRemoved(this);
        }
    }
}

void SoundWidget::on_horizontalSlider_Gain_valueChanged(int value)
{
    if(isUpdating)
        return;

    if(auto sound = getSoundComponent())
    {
        if(sound->mSource != -1)
        {
            float v = value / 10.f;
            sound->gain = v;
            SoundManager::changeGain(static_cast<unsigned>(sound->mSource), v);
            ui->label_GainValue->setText(QString::number(static_cast<double>(v)));
        }
    }
}

void SoundWidget::on_checkBox_Loop_toggled(bool checked)
{
    if(isUpdating)
        return;

    if(auto sound = getSoundComponent())
    {
        if(sound->mSource != -1)
        {
            SoundManager::setLooping(static_cast<unsigned>(sound->mSource), checked);
            sound->isLooping = checked;
            ui->checkBox_Loop->setCheckState(checked ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
        }
    }
}

void SoundWidget::on_checkBox_Mute_toggled(bool checked)
{
    if(isUpdating)
        return;

    if(auto sound = getSoundComponent())
    {
        if(sound->mSource != -1)
        {
            sound->isMuted = checked;
            SoundManager::setMute(static_cast<unsigned>(sound->mSource), checked, sound->gain);
            ui->checkBox_Mute->setCheckState(sound->isMuted ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
        }
    }
}

void SoundWidget::on_pushButton_Start_clicked()
{
    if(isUpdating)
        return;

    if(auto sound = getSoundComponent())
    {
        if(sound->mSource != -1)
        {
            SoundManager::play(static_cast<unsigned>(sound->mSource));
            isPlaying = true;
        }
    }
}

void SoundWidget::on_pushButton_Pause_clicked()
{
    if(isUpdating)
        return;

    if(auto sound = getSoundComponent())
    {
        if(sound->mSource != -1)
        {
            SoundManager::pause(static_cast<unsigned>(sound->mSource));
            isPlaying = false;
        }
    }
}

void SoundWidget::on_pushButton_Stop_clicked()
{
    if(auto sound = getSoundComponent())
    {
        if(sound->mSource != -1)
        {
            SoundManager::stop(static_cast<unsigned>(sound->mSource));
            isPlaying = false;
        }
    }
}

void SoundWidget::on_pushButton_ChangeSound_clicked()
{
    auto name = ui->comboBox_AvailableSounds->currentText();

    if(name.size() && name != "None")
    {
        if(auto sound = getSoundComponent())
        {
            if(sound->mSource != -1)
            {
                SoundManager::cleanupSource(static_cast<unsigned>(sound->mSource));
                sound->mSource = -1;
            }

            SoundManager::createSource(sound, name.toStdString());

            ui->label_CurrentSound->setText(QString::fromStdString(sound->name));
            ui->checkBox_Loop->setCheckState(sound->isLooping ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
            ui->checkBox_Mute->setCheckState(sound->isMuted ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
            ui->label_GainValue->setText(QString::number(static_cast<double>(sound->gain)));
            ui->horizontalSlider_Gain->setValue(static_cast<int>(sound->gain*10.f));
            ui->doubleSpinBox_Pitch->setValue(static_cast<double>(sound->pitch));

            if(isPlaying)
            {
                SoundManager::play(static_cast<unsigned>(sound->mSource));
            }
        }
    }
}

SoundComponent* SoundWidget::getSoundComponent()
{
    auto entity = mMainWindow->currentEntitySelected;
    if(entity)
    {
        if(auto sound = mMainWindow->getEntityManager()->getComponent<SoundComponent>(entity->entityId))
        {
           return sound;
        }
    }

    return nullptr;
}

void SoundWidget::on_doubleSpinBox_Pitch_valueChanged(double arg1)
{
    if(isUpdating) return;

    if(auto sound = getSoundComponent())
    {
        if(sound->mSource != -1)
        {
            sound->pitch = static_cast<float>(arg1);
            SoundManager::changePitch(static_cast<unsigned>(sound->mSource), static_cast<float>(arg1));
        }
    }
}

void SoundWidget::on_pushButton_soundFromFile_clicked()
{
    auto fileName = QFileDialog::getOpenFileName(this, tr("Choose"), QString::fromStdString(gsl::assetFilePath), tr("Wav files (*.wav)"));
    auto splits = fileName.split('/');
    auto last = splits[splits.size()-1];
    splits = last.split('.');
    auto name = splits[0];
    if(name.length())
    {
        if(auto sound = getSoundComponent())
        {
            if(sound->mSource != -1)
            {
                SoundManager::cleanupSource(static_cast<unsigned>(sound->mSource));
                sound->mSource = -1;
            }

            bool found = false;
            for(int i = 0; i < ui->comboBox_AvailableSounds->count(); ++i)
            {
                if(name == ui->comboBox_AvailableSounds->itemText(0))
                {
                    found = true;
                    break;
                }
            }

            if(!found)
            {
                ResourceManager::instance()->loadWav(name.toStdString(), fileName.toStdString());
                ui->comboBox_AvailableSounds->addItem(name);
            }

            SoundManager::createSource(sound, name.toStdString());

            ui->label_CurrentSound->setText(QString::fromStdString(sound->name));
            ui->checkBox_Loop->setCheckState(sound->isLooping ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
            ui->checkBox_Mute->setCheckState(sound->isMuted ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
            ui->label_GainValue->setText(QString::number(static_cast<double>(sound->gain)));
            ui->horizontalSlider_Gain->setValue(static_cast<int>(sound->gain*10.f));
            ui->doubleSpinBox_Pitch->setValue(static_cast<double>(sound->pitch));
            ui->comboBox_AvailableSounds->setCurrentText(QString::fromStdString(sound->name));

            if(isPlaying)
            {
                SoundManager::play(static_cast<unsigned>(sound->mSource));
            }
        }
    }
}
