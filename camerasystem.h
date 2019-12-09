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

    /**
     * @brief Updates the rotation of all cameras based on their given pitch and yaw values
     */
    static void updateLookAtRotation(TransformComponent &transform, CameraComponent &camera);

    /**
     * @brief Updates the camera view matrices on all cameras
     */
    static void updateCameraViewMatrices(std::vector<TransformComponent>& transforms, std::vector<CameraComponent> &cameras);

    /**
     * @brief Updates all camera projection matrices based on the paramaters.
     */
    static void updateCameraProjMatrices(std::vector<CameraComponent> &cameras,
                                         float FOV = 45.f, float aspectRatio = 4.f/3.f, float nearplane = 1.f, float farplane = 100.f);

    /**
     * @brief Updates the given camera projection matrix based on the given matrix
     */
    static void updateCameraProjMatrix(CameraComponent* camera, const gsl::mat4& projectionMatrix);

    static gsl::mat4 calcViewFrustum(const CameraComponent& camera, const TransformComponent& trans);
};

#endif // CAMERASYSTEM_H
