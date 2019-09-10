#ifndef COMPONENTWIDGET_H
#define COMPONENTWIDGET_H

#include <QWidget>

class MainWindow;

class ComponentWidget : public QWidget
{
    Q_OBJECT
public:
    ComponentWidget(MainWindow* mainWindow, QWidget* parent = nullptr);

signals:
    void widgetRemoved();

protected slots:
    virtual void Remove()=0;

private slots:
    void ProvideContextMenu(const QPoint& point);


protected:
    MainWindow* mMainWindow;
};

#endif // COMPONENTWIDGET_H
