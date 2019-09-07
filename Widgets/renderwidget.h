#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>

namespace Ui{
    class Render;
}

class MainWindow;
class Render;

class RenderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RenderWidget(MainWindow* mainWindow, QWidget* parent = nullptr);
    ~RenderWidget();

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
    Render* getRenderComponent(unsigned int entity);

    Ui::Render* ui;
    MainWindow* mMainWindow;
};

#endif // RENDERWIDGET_H
