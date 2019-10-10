#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include "innpch.h"
#include "entitymanager.h"

class CameraSystem
{
public:
    CameraSystem() = delete;

    static void updateCameras(std::vector<TransformComponent>& transforms, std::vector<CameraComponent> &cameras);
    static void updateCameras(std::vector<CameraComponent> &cameras, const gsl::mat4& projectionMatrix = gsl::mat4::persp(45.f, 3.f / 4.f, 1.f, 100.f));
    static void updateCamera(CameraComponent* camera, const gsl::mat4& projectionMatrix);


};

#endif // CAMERASYSTEM_H
