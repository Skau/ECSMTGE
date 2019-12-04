#include "innpch.h"
#include "camerasystem.h"

void CameraSystem::updateLookAtRotation(TransformComponent& transform, CameraComponent& camera)
{
    transform.setRotation(gsl::quat::lookAt(gsl::deg2radf(camera.pitch), gsl::deg2radf(camera.yaw)));
}

void CameraSystem::updateCameraViewMatrices(std::vector<TransformComponent>& transforms, std::vector<CameraComponent>& cameras)
{
    auto transIt = transforms.begin();
    auto camIt = cameras.begin();

    bool transformShortest = transforms.size() < cameras.size();

    bool _{true};

    // cause normal while (true) loops are so outdated
    for ( ;_; )
    {
        if (transformShortest)
        {
            if (transIt == transforms.end())
                break;
        }
        else
        {
            if (camIt == cameras.end())
                break;
        }

        // Increment lowest index
        if (!transIt->valid || transIt->entityId < camIt->entityId)
        {
            ++transIt;
        }
        else if (!camIt->valid || camIt->entityId < transIt->entityId)
        {
            ++camIt;
        }
        else
        {
            // If transform isn't updated,
            // matrices doesn't need to be updated
            if(!transIt->updated)
            {
                // Increment all
                ++transIt;
                ++camIt;
                continue;
            }

            camIt->viewMatrix = gsl::mat4::viewMatrix(transIt->rotation, transIt->position);
            transIt->updated = false;

            // Increment all
            ++transIt;
            ++camIt;
        }
    }
}

void CameraSystem::updateCameraProjMatrices(std::vector<CameraComponent> &cameras, float FOV, float aspectRatio, float nearplane, float farplane)
{
    for (auto it{cameras.begin()}; it != cameras.end(); ++it)
    {
        it->projectionMatrix = gsl::mat4::persp(FOV, aspectRatio, nearplane, farplane);
        it->invProjectionMatrix = gsl::mat4::perspInv(FOV, aspectRatio, nearplane, farplane);
    }
}

void CameraSystem::updateCamera(CameraComponent *camera, const gsl::mat4 &projectionMatrix)
{
    if (camera != nullptr)
        camera->projectionMatrix = projectionMatrix;
}
