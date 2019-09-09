#ifndef MESHWIDGET_H
#define MESHWIDGET_H

#include <QWidget>

namespace Ui{
    class Mesh;
}

class MainWindow;
class MeshComponent;

class MeshWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MeshWidget(MainWindow* mainWindow, QWidget* parent = nullptr);
    ~MeshWidget();

signals:
    void chooseMesh(unsigned int entity, const std::string& name);
    void widgetRemoved();

public slots:
    void update(const std::string& name);

private slots:
    void on_button_ChangeMesh_clicked();

    void on_checkBox_Visible_toggled(bool checked);

    void ProvideContextMenu(const QPoint& point);

    void Remove();

private:
    MeshComponent* getRenderComponent(unsigned int entity);

    Ui::Mesh* ui;
    MainWindow* mMainWindow;
};

#endif // MESHWIDGET_H
