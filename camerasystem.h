#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include "innpch.h"
#include "entitymanager.h"

/**
 * @brief Holds static functions for updating the camera.
 */
class CameraSystem
{
public:
    CameraSystem() = delete;

    static void updateLookAtRotation(TransformComponent &transform, CameraComponent &camera);
    static void updateCameraViewMatrices(std::vector<TransformComponent>& transforms, std::vector<CameraComponent> &cameras);
    static void updateCameraProjMatrices(std::vector<CameraComponent> &cameras,
                                         float FOV = 45.f, float aspectRatio = 4.f/3.f, float nearplane = 1.f, float farplane = 100.f);
    static void updateCamera(CameraComponent* camera, const gsl::mat4& projectionMatrix);

    static gsl::mat4 calcViewFrustum(const CameraComponent& camera, const TransformComponent& trans);


};

#endif // CAMERASYSTEM_H
