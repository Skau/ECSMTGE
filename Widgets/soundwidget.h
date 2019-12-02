#ifndef SOUNDWIDGET_H
#define SOUNDWIDGET_H

#include "componentwidget.h"

namespace Ui{
    class Sound;
}

class SoundComponent;

class SoundWidget : public ComponentWidget
{
    Q_OBJECT

public:
    SoundWidget(MainWindow *mainWindow, QWidget* parent = nullptr);

protected slots:
    void Remove() override;

private slots:
    void on_horizontalSlider_Gain_valueChanged(int value);

    void on_checkBox_Loop_toggled(bool checked);

    void on_checkBox_Mute_toggled(bool checked);

    void on_pushButton_Start_clicked();

    void on_pushButton_Pause_clicked();

    void on_pushButton_Stop_clicked();

    void on_pushButton_ChangeSound_clicked();

    void on_doubleSpinBox_Pitch_valueChanged(double arg1);

    void on_pushButton_soundFromFile_clicked();

    void on_checkBox_Autoplay_toggled(bool checked);

private:
    Ui::Sound* ui;
    bool isPlaying = false;
    SoundComponent* getSoundComponent();
};

#endif // SOUNDWIDGET_H
