#ifndef SCRIPTWIDGET_H
#define SCRIPTWIDGET_H

#include "componentwidget.h"

namespace Ui {
class ScriptWidget;
}

/** Widget for scriptcomponents in the editor.
 * @brief Widget for scriptcomponents in the editor.
 */
class ScriptWidget : public ComponentWidget
{
    Q_OBJECT

public:
    explicit ScriptWidget(MainWindow* mainWindow, QWidget *parent = nullptr);
    ~ScriptWidget();

protected slots:
    void Remove() override;

private slots:
    void on_toolButton_clicked();

    void on_button_ExecuteJS_clicked();

    void on_button_NewFile_clicked();

private:
    Ui::ScriptWidget *ui;

};

#endif // SCRIPTWIDGET_H
