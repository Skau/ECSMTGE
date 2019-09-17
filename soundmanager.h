#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <string>
#include "GSL/vector3d.h"

#ifdef _WIN32
#include <al.h>
#include <alc.h>
#elif __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif

class SoundSource;
class SoundComponent;
class TransformComponent;
class PhysicsComponent;

/**
 * @brief Holds the context and current sound device
 */
class SoundManager
{
public:
    SoundManager();
    ~SoundManager();

   static bool checkOpenALError();

   static void createSource(SoundComponent* comp, const std::string& wav);
   static void play(unsigned source);
   static void pause(unsigned source);
   static void stop(unsigned source);
   static void changeGain(unsigned source, float value);
   static void changePitch(unsigned source, float value);
   static void setLooping(unsigned source, bool value);
   static void setMute(unsigned source, bool value, float gain = 0.f);
   static void setPosition(unsigned source, gsl::vec3 position = gsl::vec3{});
   static void setVelocity(unsigned source, gsl::vec3 velocity = gsl::vec3{});
   static void cleanupSource(unsigned source);

   static void UpdatePositions(std::vector<TransformComponent> &transforms, std::vector<SoundComponent> &sounds);
   static void UpdateVelocities(std::vector<PhysicsComponent> &physics, std::vector<SoundComponent> &sounds);

public:
    ALCdevice* mDevice;
    ALCcontext* mContext;
};

#endif // SOUNDMANAGER_H
