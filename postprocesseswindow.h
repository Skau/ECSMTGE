#ifndef POSTPROCESSESWINDOW_H
#define POSTPROCESSESWINDOW_H

#include <QDialog>
#include <string>
#include <memory>
#include <map>
#include "postprocessor.h"

class Postprocessor;
class Material;

namespace Ui {
class PostProcessesWindow;
}

/**
 * @brief Hooks into the global post processing steps, enabling editing them at runtime.
 */
class PostProcessesWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PostProcessesWindow(QWidget *parent = nullptr);
    ~PostProcessesWindow();

    void addPostProcessors(const std::vector<std::pair<std::string, Postprocessor *>> &postprocessors);

private:
    Ui::PostProcessesWindow *ui;

    void addPostprocessor(std::pair<std::string, Postprocessor*> postprocess);

    std::map<std::string, Postprocessor*> cachedPostprocesses;
    std::map<Postprocessor*, std::vector<Postprocessor::Setting>> cachedSettings;
};

#endif // POSTPROCESSESWINDOW_H
