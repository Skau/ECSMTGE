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

enum class ComponentType;

class EntityManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void showFPS(float deltaTime, float frameCounter);

    Renderer* getRenderer() { return mRenderer; }

    Ui::MainWindow *ui;

    void setEntityManager(std::shared_ptr<EntityManager> entityManager);

    std::shared_ptr<EntityManager> getEntityManager() { return mEntityManager; }

    EntityData* currentEntitySelected{nullptr};

public slots:
    void updateUI(const std::vector<EntityData>& entityData);
    void onWidgetRemoved();

private slots:
    void on_objectList_clicked(const QModelIndex &index);

    void on_actionEmpty_Object_triggered();

    void on_actionCube_triggered();

    void on_actionMonkey_triggered();

    void on_button_AddComponent_clicked();

    void on_lineEdit_SelectedObject_editingFinished();

    void on_objectList_itemChanged(class QListWidgetItem* item);

private:
    void updateComponentArea(unsigned int entityID);

    /**
     * @brief Updates the add component button and combobox with the types given
     */
    void updateAvailableComponents(std::vector<ComponentType> types);

    Renderer* mRenderer;
    QWidget *mRenderWindowContainer;

    std::shared_ptr<EntityManager> mEntityManager;

    std::vector<EntityData> mEntityDataCache;
    std::vector<ComponentType> mAvailableComponentsToAddCache;
};

#endif // MAINWINDOW_H
