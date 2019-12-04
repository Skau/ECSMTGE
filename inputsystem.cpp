#include "inputsystem.h"

InputSystem::InputSystem()
{

}

void InputSystem::HandleEditorCameraInput(float deltaTime, TransformComponent& transformComponent, CameraComponent& cameraComponent)
{
    auto keys = InputHandler::Keys;

    if(keys[Qt::MouseButton::RightButton] == true)
    {
        // Rotation

        auto mouseOffset = InputHandler::MouseOffset;

        float deltaX = mouseOffset.x() * 5 * deltaTime;
        float deltaY = mouseOffset.y() * 5 * deltaTime;

        cameraComponent.pitch += deltaY;
        cameraComponent.yaw += deltaX;

        if(cameraComponent.pitch > 89.0f)
            cameraComponent.pitch = 89.0f;
        if(cameraComponent.pitch < -89.0f)
            cameraComponent.pitch = -89.0f;

        // Position

        gsl::vec3 forward = transformComponent.rotation.inverse().forwardVector();
        gsl::vec3 right = transformComponent.rotation.inverse().rightVector();
        gsl::vec3 up = transformComponent.rotation.inverse().upVector();


        if(keys[Qt::Key_W] == true)
        {
            transformComponent.position += -forward * deltaTime;
        }

        if(keys[Qt::Key_A] == true)
        {
            transformComponent.position += -right * deltaTime;
        }

        if(keys[Qt::Key_S] == true)
        {
            transformComponent.position += forward * deltaTime;
        }

        if(keys[Qt::Key_D] == true)
        {
            transformComponent.position += right * deltaTime;
        }

        if(keys[Qt::Key_Q] == true)
        {
            transformComponent.position += -up * deltaTime;
        }

        if(keys[Qt::Key_E] == true)
        {
            transformComponent.position += up * deltaTime;
        }

        transformComponent.updated = true;
    }
}
