#include "postprocesseswindow.h"
#include "ui_postprocesseswindow.h"
#include "gsl_math.h"
#include "GSL/vector2d.h"
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>

PostProcessesWindow::PostProcessesWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PostProcessesWindow)
{
    ui->setupUi(this);
    setWindowTitle("Global Post Processes");
}

PostProcessesWindow::~PostProcessesWindow()
{
    delete ui;
}

void PostProcessesWindow::addPostProcessors(const std::vector<std::pair<std::string, Postprocessor*>> &postprocessors)
{
    if(ui->mainWidget->children().size())
    {
        for(auto& child : ui->mainWidget->children())
        {
            delete child;
        }
    }

    if(ui->mainWidget->isHidden())
        ui->mainWidget->show();

    QVBoxLayout* layout = new QVBoxLayout(ui->mainWidget);
    ui->mainWidget->setLayout(layout);
    layout->setMargin(0);

    for(auto& pair : postprocessors)
    {
        QLabel* label = new QLabel(QString::fromStdString(pair.first), ui->mainWidget);
        label->setAlignment(Qt::AlignHCenter);
        QFont font;
        font.setPointSize(16);
        label->setFont(font);
        layout->addWidget(label);
        addPostprocessor(pair);
    }

    QPushButton* saveButton = new QPushButton("Save", ui->mainWidget);
    layout->addWidget(saveButton);
    saveButton->setMinimumHeight(25);
    connect(saveButton, &QPushButton::clicked, this, &PostProcessesWindow::on_button_Save_clicked);
    ui->mainWidget->setMinimumHeight(ui->mainWidget->minimumHeight() + 25);

    ui->mainWidget->setFixedSize(ui->mainWidget->sizeHint());
    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->setAlignment(Qt::AlignCenter);
}

void PostProcessesWindow::on_button_Save_clicked()
{
    onSaveClicked(cachedSteps);
    hide();
}

void PostProcessesWindow::addPostprocessor(std::pair<std::string, Postprocessor*> postprocess)
{
    auto layout = static_cast<QVBoxLayout*>(ui->mainWidget->layout());
    unsigned currentIndex = 0;
    for(auto& step : postprocess.second->steps)
    {
        cachedSteps[postprocess.second].push_back(step);

        auto material = step.material;

        if(material->mParameters.size())
        {
            QWidget* widget = new QWidget();
            ui->mainWidget->layout()->addWidget(widget);

            QLabel* label = new QLabel("Step: " + QString::number(currentIndex+1), ui->mainWidget);
            label->setAlignment(Qt::AlignHCenter);
            layout->addWidget(label);

            label = new QLabel("Shader: " + QString::fromStdString(material->mShader->mName), ui->mainWidget);
            label->setAlignment(Qt::AlignHCenter);
            layout->addWidget(label);

            float minimumHeight = 0.f;
            for(auto& param : material->mParameters)
            {
                QHBoxLayout* hLayout = new QHBoxLayout();
                auto name = QString::fromStdString(param.first);
                name = name.replace("p_", "");
                QLabel* label = new QLabel(name, widget);
                hLayout->addWidget(label);

                if(std::holds_alternative<bool>(param.second))
                {
                    QCheckBox* checkBox = new QCheckBox(widget);
                    checkBox->setCheckState(std::get<bool>(param.second) ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
                    connect(checkBox, &QCheckBox::stateChanged, [=](bool state)
                    {
                        cachedSteps[postprocess.second][currentIndex].material->mParameters[param.first] = state;
                    });
                    hLayout->addWidget(checkBox);
                    minimumHeight += 25.33f;
                }
                else if (std::holds_alternative<int>(param.second))
                {
                    QSpinBox* spinBox = new QSpinBox(widget);
                    spinBox->setValue(std::get<int>(param.second));
                    connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                            [=](int i)
                    {
                        cachedSteps[postprocess.second][currentIndex].material->mParameters[param.first] = i;
                    });
                    hLayout->addWidget(spinBox);
                    minimumHeight += 25.33f;
                }
                else if (std::holds_alternative<float>(param.second))
                {
                    QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox(widget);
                    doubleSpinBox->setSingleStep(0.1);
                    doubleSpinBox->setValue(static_cast<double>(std::get<float>(param.second)));
                    connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                            [=](double d)
                    {
                        cachedSteps[postprocess.second][currentIndex].material->mParameters[param.first] = static_cast<float>(d);
                    });
                    hLayout->addWidget(doubleSpinBox);
                    minimumHeight += 25.33f;
                }
                else if (std::holds_alternative<gsl::vec2>(param.second))
                {
                    for(int i = 0; i <= 1; ++i)
                    {
                        QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox(widget);
                        doubleSpinBox->setSingleStep(0.1);
                        float value = 0;
                        switch (i)
                        {
                        case 0:
                            value = std::get<gsl::vec2>(param.second).x;
                            connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                                    [=](double d)
                            {
                                std::get<gsl::vec2>(cachedSteps[postprocess.second][currentIndex].material->mParameters[param.first]).setX(static_cast<float>(d));
                            });
                            break;
                        case 1:
                            value = std::get<gsl::vec2>(param.second).y;
                            connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                                    [=](double d)
                            {
                                std::get<gsl::vec2>(cachedSteps[postprocess.second][currentIndex].material->mParameters[param.first]).setY(static_cast<float>(d));
                            });
                            break;
                        }
                        doubleSpinBox->setValue(static_cast<double>(value));
                        hLayout->addWidget(doubleSpinBox);
                    }
                    minimumHeight += 25.33f;
                }
                else if (std::holds_alternative<gsl::vec3>(param.second))
                {
                    for(int i = 0; i <= 2; ++i)
                    {
                        QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox(widget);
                        doubleSpinBox->setSingleStep(0.1);
                        float value = 0;
                        switch (i)
                        {
                        case 0:
                            value = std::get<gsl::vec3>(param.second).x;
                            connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                                    [=](double d)
                            {
                                std::get<gsl::vec3>(cachedSteps[postprocess.second][currentIndex].material->mParameters[param.first]).setX(static_cast<float>(d));
                            });
                            break;
                        case 1:
                            value = std::get<gsl::vec3>(param.second).y;
                            connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                                    [=](double d)
                            {
                                std::get<gsl::vec3>(cachedSteps[postprocess.second][currentIndex].material->mParameters[param.first]).setY(static_cast<float>(d));
                            });
                            break;
                        case 2:
                            value = std::get<gsl::vec3>(param.second).z;
                            connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                                    [=](double d)
                            {
                                std::get<gsl::vec3>(cachedSteps[postprocess.second][currentIndex].material->mParameters[param.first]).setZ(static_cast<float>(d));
                            });
                            break;
                        }
                        doubleSpinBox->setValue(static_cast<double>(value));
                        hLayout->addWidget(doubleSpinBox);
                    }
                    minimumHeight += 25.33f;
                }
                else if (std::holds_alternative<gsl::vec4>(param.second))
                {
                    for(int i = 0; i <= 3; ++i)
                    {
                        QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox(widget);
                        doubleSpinBox->setSingleStep(0.1);
                        float value = 0;
                        switch (i)
                        {
                        case 0:
                            value = std::get<gsl::vec4>(param.second).x;
                            connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                                    [=](double d)
                            {
                                std::get<gsl::vec4>(cachedSteps[postprocess.second][currentIndex].material->mParameters[param.first]).setX(static_cast<float>(d));
                            });
                            break;
                        case 1:
                            value = std::get<gsl::vec4>(param.second).y;
                            connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                                    [=](double d)
                            {
                                std::get<gsl::vec4>(cachedSteps[postprocess.second][currentIndex].material->mParameters[param.first]).setY(static_cast<float>(d));
                            });
                            break;
                        case 2:
                            value = std::get<gsl::vec4>(param.second).z;
                            connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                                    [=](double d)
                            {
                                std::get<gsl::vec4>(cachedSteps[postprocess.second][currentIndex].material->mParameters[param.first]).setZ(static_cast<float>(d));
                            });
                            break;
                        case 3:
                            value = std::get<gsl::vec4>(param.second).w;
                            connect(doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                                    [=](double d)
                            {
                                std::get<gsl::vec4>(cachedSteps[postprocess.second][currentIndex].material->mParameters[param.first]).setW(static_cast<float>(d));
                            });
                            break;
                        }
                        doubleSpinBox->setValue(static_cast<double>(value));
                        hLayout->addWidget(doubleSpinBox);
                    }
                    minimumHeight += 25.33f;
                }
                layout->addLayout(hLayout);
            }
            ui->mainWidget->setMinimumHeight(static_cast<int>(minimumHeight));

        }
        else
        {
            if(!ui->mainWidget->isHidden())
                ui->mainWidget->hide();
        }
        currentIndex++;
    }
}
