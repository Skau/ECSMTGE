#ifndef COMPONENTWIDGET_H
#define COMPONENTWIDGET_H

#include <QWidget>

class MainWindow;

class ComponentWidget : public QWidget
{
    Q_OBJECT
public:
    ComponentWidget(MainWindow* mainWindow, QWidget* parent = nullptr);

    virtual void updateData()=0;

signals:
    void widgetRemoved(ComponentWidget* widget);

protected slots:
    virtual void Remove()=0;

private slots:
    void ProvideContextMenu(const QPoint& point);

protected:
    MainWindow* mMainWindow;
    bool isUpdating{false};
};

#endif // COMPONENTWIDGET_H
