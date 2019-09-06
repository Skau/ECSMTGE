#ifndef CAMERA_H
#define CAMERA_H

#include "innpch.h"
#include "entitymanager.h"

class CameraSystem
{
public:
    CameraSystem();

    void pitch(float degrees);
    void yaw(float degrees);
    void updateRightVector();
    void updateForwardVector();
    void update(double deltaTime);

    gsl::Matrix4x4 mViewMatrix;
    gsl::Matrix4x4 mProjectionMatrix;

    void setPosition(const gsl::Vector3D &position);

    void setSpeed(float speed);
    void updateHeigth(float deltaHeigth);

    void moveRight(float delta);

    gsl::Vector3D position() const;
    gsl::Vector3D up() const;

    gsl::Vector3D forward() const;

    static void updateCameras(std::vector<Transform> transforms, std::vector<Camera> cameras);
    static void updateCameras(std::vector<Camera> cameras, const gsl::mat4& projectionMatrix = gsl::mat4::persp(45.f, 3.f / 4.f, 1.f, 100.f));
    static void updateCamera(Camera* camera, const gsl::mat4& projectionMatrix);

private:
    gsl::Vector3D mForward{0.f, 0.f, -1.f};
    gsl::Vector3D mRight{1.f, 0.f, 0.f};
    gsl::Vector3D mUp{0.f, 1.f, 0.f};

    gsl::Vector3D mPosition{0.f, 0.f, 0.f};
    float mPitch{0.f};
    float mYaw{0.f};

    gsl::Matrix4x4 mYawMatrix;
    gsl::Matrix4x4 mPitchMatrix;

    float mSpeed{0.f}; //camera will move by this speed along the mForward vector
};

#endif // CAMERA_H
