#include "inputsystem.h"

InputSystem::InputSystem()
{

}

void InputSystem::HandleInput(float deltaTime, std::vector<InputComponent> &inputComponents, std::vector<TransformComponent> &transformComponents)
{
    auto keys = InputHandler::Keys;

    auto inputIt = inputComponents.begin();
    auto transIt = transformComponents.begin();


    bool transformShortest = transformComponents.size() < inputComponents.size();

    bool _{true};

    // cause normal while (true) loops are so outdated
    for ( ;_; )
    {
        if (transformShortest)
        {
            if (transIt == transformComponents.end())
                break;
        }
        else
        {
            if (inputIt == inputComponents.end())
                break;
        }

        // Increment lowest index
        if (!transIt->valid || transIt->entityId < inputIt->entityId)
        {
            ++transIt;
        }
        else if (!inputIt->valid || inputIt->entityId < transIt->entityId)
        {
            ++inputIt;
        }
        else
        {
            if(!inputIt->controlledWhilePlaying)
            {
                // Increment all
                ++transIt;
                ++inputIt;
                continue;
            }

            if(keys[Qt::MouseButton::RightButton] == true)
            {
                gsl::vec3 forward{};
                gsl::vec3 right{};
                gsl::vec3 up{};

                if (inputIt->cameraMovement)
                {
                    forward = transIt->rotation.inverse().forwardVector();
                    right = transIt->rotation.inverse().rightVector();
                    up = transIt->rotation.inverse().upVector();
                }
                else
                {
                    forward = transIt->rotation.forwardVector();
                    right = transIt->rotation.rightVector();
                    up = transIt->rotation.upVector();
                }


                if(keys[Qt::Key_W] == true)
                {
                    transIt->position += -forward * deltaTime;
                }

                if(keys[Qt::Key_A] == true)
                {
                    transIt->position += -right * deltaTime;
                }

                if(keys[Qt::Key_S] == true)
                {
                    transIt->position += forward * deltaTime;
                }

                if(keys[Qt::Key_D] == true)
                {
                    transIt->position += right * deltaTime;
                }

                if(keys[Qt::Key_Q] == true)
                {
                    transIt->position += -up * deltaTime;
                }

                if(keys[Qt::Key_E] == true)
                {
                    transIt->position += up * deltaTime;
                }

                transIt->updated = true;

                // Disable camera movement in case the entity
                // is not a camera anymore. (Not quite sure if necessary)
                inputIt->cameraMovement = false;
            }

            // Increment all
            ++transIt;
            ++inputIt;
        }
    }
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

        transformComponent.setRotation(gsl::quat::lookAt(gsl::deg2radf(cameraComponent.pitch), gsl::deg2radf(cameraComponent.yaw)));

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
