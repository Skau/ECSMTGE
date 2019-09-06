#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

class QWidget;
class Renderer;
class EntityData;
class Component;

namespace Ui {
class MainWindow;
}

class EntityManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void showFPS(double deltaTime, double frameCounter);

    Renderer* getRenderer() { return mRenderer; }

    Ui::MainWindow *ui;

    void setEntityManager(std::shared_ptr<EntityManager> entityManager);

    std::shared_ptr<EntityManager> getEntityManager() { return mEntityManager; }

    EntityData* currentEntitySelected{nullptr};



signals:
    void createObject(int index);
    bool getAllComponentsForEntity(unsigned int entity, std::vector<Component*>& outComponents);

public slots:
    void updateUI(const std::vector<EntityData>& entityData);

private slots:
    void on_objectList_activated(const QModelIndex &index);

    void on_actionCube_triggered();

    void on_actionMonkey_triggered();

private:
    Renderer* mRenderer;
    QWidget *mRenderWindowContainer;

    std::shared_ptr<EntityManager> mEntityManager;

    std::vector<EntityData> mEntityDataCache;
};

#endif // MAINWINDOW_H
