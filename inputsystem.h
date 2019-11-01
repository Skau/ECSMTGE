#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include <QObject>

#include "inputhandler.h"
#include "componentdata.h"

class InputSystem : public QObject
{
    Q_OBJECT

public:
    InputSystem();

    static void HandleInput(float deltaTime, std::vector<InputComponent> &inputComponents, std::vector<TransformComponent>& transformComponents);
    static void HandleCameraInput(float deltaTime, std::vector<InputComponent> &inputComponents, std::vector<TransformComponent>& transformComponents,
                                  std::vector<CameraComponent>& cameraComponents);
};

#endif // INPUTSYSTEM_H
