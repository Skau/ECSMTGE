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

    /**
     * @brief Updates the position of the sound listener with the given position.
     */
    void setPosition(const gsl::vec3& pos);

    /**
     * @brief Updates the velocity of the sound listener with the given velocity.
     */
    void setVelocity(const gsl::vec3& vel);

    /**
     * @brief Updates the direction of the sound listener with the given direction.
     */
    void setDirection(const gsl::vec3& dir, const gsl::vec3& up);

    /**
     * @brief Sets mute to be the given value.
     */
    void setMute(bool mute);

    /**
     * @brief Updates the listener based on the given camera and transform component.
     */
    void update(const CameraComponent& camera, const TransformComponent& transform);

private:
    gsl::vec3 mPos;
    gsl::vec3 mVel;
    gsl::vec3 mDir;
    gsl::vec3 mUp;
};

#endif // SOUNDLISTENER_H
