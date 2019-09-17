#include "inputsystem.h"

InputSystem::InputSystem()
{

}

void InputSystem::HandleInput(float deltaTime, const std::vector<InputComponent> &inputComponents, std::vector<TransformComponent> &transformComponents)
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
                if(keys[Qt::Key_W] == true)
                {
                    transIt->position += (gsl::vec3(0, 0, -1) * deltaTime);
                }

                if(keys[Qt::Key_A] == true)
                {
                    transIt->position += (gsl::vec3(-1, 0, 0) * deltaTime);
                }

                if(keys[Qt::Key_S] == true)
                {
                    transIt->position += (gsl::vec3(0, 0, 1) * deltaTime);
                }

                if(keys[Qt::Key_D] == true)
                {
                    transIt->position += (gsl::vec3(1, 0, 0) * deltaTime);
                }

                if(keys[Qt::Key_Q] == true)
                {
                    transIt->position += (gsl::vec3(0, -1, 0) * deltaTime);
                }

                if(keys[Qt::Key_E] == true)
                {
                    transIt->position += (gsl::vec3(0, 1, 0) * deltaTime);
                }

                transIt->updated = true;
            }

            // Increment all
            ++transIt;
            ++inputIt;
        }
    }
}

void InputSystem::HandleCameraInput(float deltaTime, const std::vector<InputComponent> &inputComponents,
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

            // Increment all
            ++cameraIt;
            ++inputIt;
        }
    }
}
