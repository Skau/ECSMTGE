#ifndef COMPONENTWIDGET_H
#define COMPONENTWIDGET_H

#include <QWidget>

class MainWindow;

/** Base component widget for widgets in the editor.
 * Never used directly, only as a base for all other
 * component widgets.
 * @brief Base component widget for widgets in the editor.
 */
class ComponentWidget : public QWidget
{
    Q_OBJECT
public:
    ComponentWidget(MainWindow* mainWindow, QWidget* parent = nullptr);

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
