#ifndef SOUNDLISTENER_H
#define SOUNDLISTENER_H

#include "innpch.h"
#include "componentdata.h"

/** The ears of the player / camera.
 * Global listener and also sound mixer for all sounds.
 * @brief The ears of the player / camera.
 */
class SoundListener
{
public:
    SoundListener(const gsl::vec3& pos = gsl::vec3(0),
                  const gsl::vec3& vel = gsl::vec3(0),
                  const gsl::vec3& dir = gsl::vec3(0.f, 0.f, -1.f),
                  const gsl::vec3& up = gsl::vec3(0.f, 1.f, 0.f));

    void setPosition(const gsl::vec3& pos);
    void setVelocity(const gsl::vec3& vel);
    void setDirection(const gsl::vec3& dir, const gsl::vec3& up);
    void setMute(bool mute);

    void update(const CameraComponent& camera, const TransformComponent& transform);

private:
    gsl::vec3 mPos;
    gsl::vec3 mVel;
    gsl::vec3 mDir;
    gsl::vec3 mUp;
};

#endif // SOUNDLISTENER_H
