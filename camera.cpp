#include "innpch.h"
#include "camera.h"

CameraSystem::CameraSystem()
{
    mViewMatrix.setToIdentity();
    mProjectionMatrix.setToIdentity();

    mYawMatrix.setToIdentity();
    mPitchMatrix.setToIdentity();
}

void CameraSystem::pitch(float degrees)
{
    //  rotate around mRight
    mPitch -= degrees;
    updateForwardVector();
}

void CameraSystem::yaw(float degrees)
{
    // rotate around mUp
    mYaw -= degrees;
    updateForwardVector();
}

void CameraSystem::updateRightVector()
{
    mRight = mForward^mUp;
    mRight.normalize();
//    qDebug() << "Right " << mRight;
}

void CameraSystem::updateForwardVector()
{
    mRight = gsl::Vector3D(1.f, 0.f, 0.f);
    mRight.rotateY(mYaw);
    mRight.normalize();
    mUp = gsl::Vector3D(0.f, 1.f, 0.f);
    mUp.rotateX(mPitch);
    mUp.normalize();
    mForward = mUp^mRight;

    updateRightVector();
}

void CameraSystem::update(double deltaTime)
{
    mYawMatrix.setToIdentity();
    mPitchMatrix.setToIdentity();

    mPitchMatrix.rotateX(mPitch);
    mYawMatrix.rotateY(mYaw);

    mPosition -= mForward * mSpeed;

    mViewMatrix = mPitchMatrix* mYawMatrix;
    mViewMatrix.translate(-mPosition);
}

void CameraSystem::setPosition(const gsl::Vector3D &position)
{
    mPosition = position;
}

void CameraSystem::setSpeed(float speed)
{
    mSpeed = speed;
}

void CameraSystem::updateHeigth(float deltaHeigth)
{
    mPosition.y += deltaHeigth;
}

void CameraSystem::moveRight(float delta)
{
    //This fixes a bug in the up and right calculations
    //so camera always holds its height when straifing
    //should be fixed thru correct right calculations!
    gsl::Vector3D right = mRight;
    right.y = 0.f;
    mPosition += right * delta;
}

gsl::Vector3D CameraSystem::position() const
{
    return mPosition;
}

gsl::Vector3D CameraSystem::up() const
{
    return mUp;
}

gsl::Vector3D CameraSystem::forward() const
{
    return mForward;
}

void CameraSystem::updateCameras(std::vector<Transform> transforms, std::vector<Camera> cameras)
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

            // If transform is updated, viewMatrix needs to be updated
            camIt->viewMatrix = gsl::mat4::viewMatrix(transIt->position, gsl::vec3{0.f, 0.f, 0.f});


            // Increment all
            ++transIt;
            ++camIt;
        }
    }
}

void CameraSystem::updateCameras(std::vector<Camera> cameras, const gsl::mat4 &projectionMatrix)
{
    for (auto comp : cameras)
        comp.projectionMatrix = projectionMatrix;
}

void CameraSystem::updateCamera(Camera *camera, const gsl::mat4 &projectionMatrix)
{
    if (camera != nullptr)
        camera->projectionMatrix = projectionMatrix;
}
