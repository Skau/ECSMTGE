#include "innpch.h"
#include "camerasystem.h"

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
