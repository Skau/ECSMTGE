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
                    transIt->position += (gsl::vec3(0, 0, -1) * (deltaTime / 1000.f));
                }

                if(keys[Qt::Key_A] == true)
                {
                    transIt->position += (gsl::vec3(-1, 0, 0) * (deltaTime / 1000.f));
                }

                if(keys[Qt::Key_S] == true)
                {
                    transIt->position += (gsl::vec3(0, 0, 1) * (deltaTime / 1000.f));
                }

                if(keys[Qt::Key_D] == true)
                {
                    transIt->position += (gsl::vec3(1, 0, 0) * (deltaTime / 1000.f));
                }

                if(keys[Qt::Key_Q] == true)
                {
                    transIt->position += (gsl::vec3(0, 1, 0) * (deltaTime / 1000.f));
                }

                if(keys[Qt::Key_E] == true)
                {
                    transIt->position += (gsl::vec3(0, -1, 0) * (deltaTime / 1000.f));
                }

                auto mouseOffset = InputHandler::MouseOffset;

                float deltaX = mouseOffset.x() * 5 * deltaTime / 1000;
                float deltaY = mouseOffset.y() * 5 * deltaTime / 1000;

                auto& rot = transIt->rotation;
                rot.x += deltaY;
                rot.y += deltaX;

                if(rot.x > 89.0f)
                    rot.x = 89.0f;
                if(rot.x < -89.0f)
                    rot.x = -89.0f;

                transIt->updated = true;
            }

            // Increment all
            ++transIt;
            ++inputIt;
        }
    }
}
