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
            if(!inputIt->isCurrentlyControlled)
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

void InputSystem::HandleCameraInput(float deltaTime, std::vector<InputComponent> &inputComponents,
                                    std::vector<TransformComponent> &transformComponents, std::vector<CameraComponent> &cameraComponents)
{
    auto keys = InputHandler::Keys;

    auto inputIt = inputComponents.begin();
    auto cameraIt = cameraComponents.begin();
    auto transIt = transformComponents.begin();


    bool cameraShortest = cameraComponents.size() < inputComponents.size();

    bool _{true};

    // cause normal while (true) loops are so outdated
    for ( ;_; )
    {
        if (cameraShortest)
        {
            if (cameraIt == cameraComponents.end())
                break;
        }
        else
        {
            if (inputIt == inputComponents.end())
                break;
        }

        // Increment lowest index
        if (!cameraIt->valid || cameraIt->entityId < inputIt->entityId)
        {
            ++cameraIt;
        }
        else if (!inputIt->valid || inputIt->entityId < cameraIt->entityId)
        {
            ++inputIt;
        }
        else
        {
            if(!inputIt->isCurrentlyControlled)
            {
                // Increment all
                ++cameraIt;
                ++inputIt;
                continue;
            }

            while (transIt->entityId < cameraIt->entityId)
                ++transIt;

            if (transIt->entityId > cameraIt->entityId)
                continue;

            if(keys[Qt::MouseButton::RightButton] == true)
            {
                auto mouseOffset = InputHandler::MouseOffset;

                float deltaX = mouseOffset.x() * 5 * deltaTime;
                float deltaY = mouseOffset.y() * 5 * deltaTime;

                cameraIt->pitch += deltaY;
                cameraIt->yaw += deltaX;

                if(cameraIt->pitch > 89.0f)
                    cameraIt->pitch = 89.0f;
                if(cameraIt->pitch < -89.0f)
                    cameraIt->pitch = -89.0f;

                transIt->setRotation(gsl::quat::lookAt(gsl::deg2radf(cameraIt->pitch), gsl::deg2radf(cameraIt->yaw)));
            }

            inputIt->cameraMovement = true;

            // Increment all
            ++cameraIt;
            ++inputIt;
        }
    }
}
