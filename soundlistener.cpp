#include "soundlistener.h"

#ifdef _WIN32
#include <al.h>
#include <alc.h>
#endif
#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif

#include "soundmanager.h"

SoundListener::SoundListener(const gsl::vec3& pos, const gsl::vec3& vel, const gsl::vec3& dir, const gsl::vec3& up)
{
    setPosition(pos);
    setVelocity(vel);
    setDirection(dir, up);
}

void SoundListener::setPosition(const gsl::vec3& pos)
{
    mPos = pos;

    ALfloat position[3];
    position[0] = mPos.x;
    position[1] = mPos.y;
    position[2] = mPos.z;

    alListenerfv(AL_POSITION, position);
}

void SoundListener::setVelocity(const gsl::vec3& vel)
{
    mVel = vel;

    ALfloat velocity[3];
    velocity[0] = mVel.x;
    velocity[1] = mVel.y;
    velocity[2] = mVel.z;

    alListenerfv(AL_VELOCITY, velocity);
}

void SoundListener::setDirection(const gsl::vec3& dir, const gsl::vec3& up)
{
    mDir = dir;
    mUp = up;

    ALfloat head[6];
    head[0] = mDir.x;
    head[1] = mDir.y;
    head[2] = mDir.z;
    head[3] = mUp.x;
    head[4] = mUp.y;
    head[5] = mUp.z;

    alListenerfv(AL_ORIENTATION, head);
}

void SoundListener::setMute(bool mute)
{
    alListenerf(AL_GAIN, mute ? 0.f : 1.f);
}

void SoundListener::update(const CameraComponent& camera, const TransformComponent& transform)
{
    setPosition(transform.position);

    auto view = camera.viewMatrix;
    ALfloat orientation[6];
    // TODO: Fix this. The calculations for forward vector is probably wrong.
    auto v = view.calcInverse();
    orientation[0] = v.getFloat(3);
    orientation[1] = v.getFloat(7);
    orientation[2] = v.getFloat(11);

    orientation[3] = view.getFloat(4);
    orientation[4] = view.getFloat(5);
    orientation[5] = view.getFloat(6);

    alListenerfv(AL_ORIENTATION, orientation);

    SoundManager::checkOpenALError();
}

