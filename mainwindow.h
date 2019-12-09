#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <map>
#include "postprocessor.h"

class QWidget;
class Renderer;
class EntityInfo;
class Component;

class PostProcessesWindow;

namespace Ui {
class MainWindow;
}

enum class ComponentType;

class QTreeWidgetItem;
class ComponentWidget;

/**
 * @brief Holds all UI widgets and the renderer, helps control the lifetime of the app. When this window is closed the application quits.
 */
class MainWindow : public QMainWindow
{
    friend class App;
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    /**
     * @brief Updates the window status bar based on vertices renderer, FPS and time in ms between frames.
     */
    void updateStatusBar(int vertices, float deltaTime, float frameCounter);

    /**
     * @brief Returns the render window.
     */
    Renderer* getRenderer() { return mRenderer; }

    Ui::MainWindow *ui;

    EntityInfo* currentEntitySelected{nullptr};

    EntityInfo* getEntityAt(QTreeWidgetItem* item);

    /**
     * @brief Sends the postprocessor info to the post processes window for setup.
     */
    void addGlobalPostProcessing(const std::vector<std::pair<std::string, Postprocessor*>>& postprocessors);

signals:
    void play();
    void stop();
    void toggleWireframe(bool value);
    void shutUp(bool value);
    void saveScene(const std::string& filePath);
    void loadScene(const std::string& filePath);
    void newScene();
    void quitting();
public slots:
    /**
     * @brief Recreates the world outliner based on the entity info given.
     */
    void updateUI(const std::vector<EntityInfo>& entityData);
    void onWidgetRemoved(ComponentWidget* widget);
    void refreshWidgets();
    void setSceneName(const std::string& name);

private slots:
    void on_actionEmpty_Object_triggered();

    void on_actionCube_triggered();

    void on_actionMonkey_triggered();

    void on_actionPlay_triggered(bool value);

    void on_button_AddComponent_clicked();

    void on_lineEdit_SelectedObject_editingFinished();

    void on_treeWidget_ObjectList_itemChanged(QTreeWidgetItem *item, int);

    void on_treeWidget_ObjectList_itemClicked(QTreeWidgetItem *item, int);

    void on_actionToggle_wireframe_triggered(bool checked);

    void on_actionExit_triggered();

    void on_actionToggle_shutup_triggered(bool checked);

    void on_actionSave_triggered();

    void on_actionLoad_triggered();

    void on_actionNew_triggered();

    void on_actionPost_Processes_triggered();

    /**
     * @brief Connected to the onSaveClicked signal in PostprocessorWindow. Updates the postprocessorsettings.json.
     */
    void onPostprocessorSaved(const std::map<Postprocessor*, std::vector<Postprocessor::Setting>>& steps);

private:
    /**
     * @brief Recreates the component widgets given the entity.
     */
    void updateComponentArea(unsigned int entityID);

    /**
     * @brief Updates the add component button and combobox with the types given
     */
    void updateAvailableComponents(std::vector<ComponentType> types);

    Renderer* mRenderer;
    QWidget *mRenderWindowContainer;

    std::vector<EntityInfo> mEntityDataCache;
    std::vector<ComponentType> mAvailableComponentsToAddCache;

    std::map<QTreeWidgetItem*, EntityInfo> mTreeDataCache;

    std::vector<ComponentWidget*> mCurrentComponentWidgets;
    void setSelected(EntityInfo *entityInfo);
    void updateSelectedInTreeWidget(EntityInfo* entityInfo);

    void updateEntityName(unsigned entity, const std::string &name);

    PostProcessesWindow* mPostProcessesWindow;

protected:
    void closeEvent(QCloseEvent* event) override;
};

#endif // MAINWINDOW_H
