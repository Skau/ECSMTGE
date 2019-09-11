#ifndef MESHWIDGET_H
#define MESHWIDGET_H

#include "componentwidget.h"

namespace Ui{
    class Mesh;
}

class MeshComponent;

class MeshWidget : public ComponentWidget
{
    Q_OBJECT

public:
    explicit MeshWidget(MainWindow* mainWindow, QWidget* parent = nullptr);

signals:
    void chooseMesh(unsigned int entity, const std::string& name);

public slots:
    void update(const std::string& name);

protected slots:
    void Remove() override;

private slots:
    void on_button_ChangeMesh_clicked();

    void on_checkBox_Visible_toggled(bool checked);


    void on_pushButton_ChangeMeshDropdown_clicked();

    void on_pushButton_ChangeShaderDropdown_clicked();

    void on_pushButton_ChangeTextureDropdown_clicked();

private:
    MeshComponent* getRenderComponent(unsigned int entity);

    Ui::Mesh* ui;
};

#endif // MESHWIDGET_H
