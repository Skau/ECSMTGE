#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>

namespace Ui{
    class Render;
}

class MainWindow;

class RenderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RenderWidget(MainWindow* mainWindow, QWidget* parent = nullptr);
    ~RenderWidget();

signals:
    void chooseMesh(unsigned int entity, const std::string& name);

public slots:
        void setName(const std::string& name);

private slots:
    void on_button_ChangeMesh_clicked();

private:
    Ui::Render* ui;
    MainWindow* mMainWindow;
};

#endif // RENDERWIDGET_H
