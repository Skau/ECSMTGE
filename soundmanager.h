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
private:
     friend class App;
     SoundManager();
     static SoundManager* mSoundManagerInstance;

public:
    ~SoundManager();

   static SoundManager& get();

   bool checkOpenALError();

   void createSource(SoundComponent* comp, const std::string& wav);
   void play(unsigned source);
   void playOnStartup(const std::vector<SoundComponent> &comps);
   void pause(unsigned source);
   void stop(unsigned source);
   void stop(std::vector<SoundComponent> comps);
   void changeGain(unsigned source, float value);
   void changePitch(unsigned source, float value);
   void setLooping(unsigned source, bool value);
   void setMute(unsigned source, bool value, float gain = 0.f);
   void setPosition(unsigned source, gsl::vec3 position = gsl::vec3{});
   void setVelocity(unsigned source, gsl::vec3 velocity = gsl::vec3{});
   void cleanupSource(unsigned source);

   void UpdatePositions(std::vector<TransformComponent> &transforms, std::vector<SoundComponent> &sounds);
   void UpdateVelocities(std::vector<PhysicsComponent> &physics, std::vector<SoundComponent> &sounds);

public:
    ALCdevice* mDevice;
    ALCcontext* mContext;
};

#endif // SOUNDMANAGER_H
