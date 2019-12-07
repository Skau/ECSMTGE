#ifndef OBJECTTREEWIDGET_H
#define OBJECTTREEWIDGET_H

#include <QTreeWidget>

class MainWindow;

/** Widget showing contents of the scene and parent-child coherence.
 * @brief Widget showing contents of the scene and parent-child coherence.
 */
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
