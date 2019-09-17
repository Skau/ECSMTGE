#include "innpch.h"
#include "camerasystem.h"

std::vector<unsigned int> CameraSystem::updateCameras(std::vector<TransformComponent> transforms, std::vector<CameraComponent>& cameras)
{
    std::vector<unsigned int> usedTransforms{};
    usedTransforms.reserve(cameras.size());

    unsigned int transIt{0};
    auto camIt = cameras.begin();

    bool transformShortest = transforms.size() < cameras.size();

    bool _{true};

    // cause normal while (true) loops are so outdated
    for ( ;_; )
    {
        if (transformShortest)
        {
            if (transIt == transforms.size())
                break;
        }
        else
        {
            if (camIt == cameras.end())
                break;
        }

        // Increment lowest index
        if (!transforms[transIt].valid || transforms[transIt].entityId < camIt->entityId)
        {
            ++transIt;
        }
        else if (!camIt->valid || camIt->entityId < transforms[transIt].entityId)
        {
            ++camIt;
        }
        else
        {
            // If transform isn't updated,
            // matrices doesn't need to be updated
            if(!transforms[transIt].updated)
            {
                // Increment all
                ++transIt;
                ++camIt;
                continue;
            }

            camIt->viewMatrix = transforms[transIt].rotation.toMat() * gsl::mat4::translation(-transforms[transIt].position);

            usedTransforms.push_back(transIt);

            // Increment all
            ++transIt;
            ++camIt;
        }
    }
    return usedTransforms;
}

void CameraSystem::updateCameras(std::vector<CameraComponent>& cameras, const gsl::mat4 &projectionMatrix)
{
    for (auto& comp : cameras)
        comp.projectionMatrix = projectionMatrix;
}

void CameraSystem::updateCamera(CameraComponent *camera, const gsl::mat4 &projectionMatrix)
{
    if (camera != nullptr)
        camera->projectionMatrix = projectionMatrix;
}
