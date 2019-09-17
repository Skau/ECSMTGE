#ifndef SOUNDWIDGET_H
#define SOUNDWIDGET_H

#include "componentwidget.h"

class SoundWidget : public ComponentWidget
{
public:
    SoundWidget(MainWindow *mainWindow, QWidget* parent = nullptr);

    void updateData();

protected slots:
    void Remove();
};

#endif // SOUNDWIDGET_H
