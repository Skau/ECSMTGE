#ifndef MESHWIDGET_H
#define MESHWIDGET_H

#include "componentwidget.h"
#include <memory>
#include "meshdata.h"

namespace Ui{
    class Mesh;
}

class MeshComponent;
class Shader;

class MeshWidget : public ComponentWidget
{
    Q_OBJECT

public:
    explicit MeshWidget(MainWindow* mainWindow, QWidget* parent = nullptr);

signals:
    void chooseMesh(unsigned int entity, const std::string& name);

protected slots:
    void Remove() override;

private slots:
    void on_button_ChangeMesh_clicked();

    void on_checkBox_Visible_toggled(bool checked);

    void on_pushButton_ChangeMeshDropdown_clicked();

    void on_pushButton_ChangeShaderDropdown_clicked();

    void on_pushButton_ChangeTextureDropdown_clicked();

    void on_checkBox_Wireframe_toggled(bool checked);

private:
    MeshComponent* getRenderComponent();

    void updateShaderParameters(Material& material);

    Ui::Mesh* ui;
};

#endif // MESHWIDGET_H
