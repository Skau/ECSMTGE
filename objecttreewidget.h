#ifndef OBJECTTREEWIDGET_H
#define OBJECTTREEWIDGET_H

#include <QTreeWidget>

class MainWindow;

class ObjectTreeWidget : public QTreeWidget
{
public:
    ObjectTreeWidget(QWidget *parent = nullptr);

    void setMainWindow(MainWindow* mainWindow) { mMainWindow = mainWindow; }

protected:
    void dropEvent(QDropEvent *event) override;

private:
    MainWindow* mMainWindow;


};

#endif // OBJECTTREEWIDGET_H
