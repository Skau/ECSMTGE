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

   /**
    * @brief Updates the source in a given sound component and wav given.
    */
   void createSource(SoundComponent* comp, const std::string& wav);

   /**
    * @brief Starts playing the given source.
    */
   void play(unsigned source);

   /**
    * @brief Iterates through all sound components and runs play() on all components with autplay enabled. Called once when the game is started.
    */
   void playOnStartup(const std::vector<SoundComponent> &comps);

   /**
    * @brief Pauses the given source.
    */
   void pause(unsigned source);

   /**
    * @brief Stops the given source.
    */
   void stop(unsigned source);

   /**
    * @brief Stops all sounds given.
    */
   void stop(std::vector<SoundComponent> comps);

   /**
    * @brief Updates the gain on the given source with the given value.
    */
   void changeGain(unsigned source, float value);

   /**
    * @brief Updates the pitch on the given source with the given value.
    */
   void changePitch(unsigned source, float value);

   /**
    * @brief Updates the loop on the given source with the given value.
    */
   void setLooping(unsigned source, bool value);

   /**
    * @brief Sets mute on the given source with the given value. If the value is false, aka unmuted, the optional gain will be used.
    */
   void setMute(unsigned source, bool value, float gain = 0.f);

   /**
    * @brief Updates the position on the given source with the given value.
    */
   void setPosition(unsigned source, gsl::vec3 position = gsl::vec3{});

   /**
    * @brief Updates the velocity on the given source with the given value.
    */
   void setVelocity(unsigned source, gsl::vec3 velocity = gsl::vec3{});

   /**
    * @brief Cleanup function. Stops the source if playing and deletes it.
    */
   void cleanupSource(unsigned source);

   /**
    * @brief Updates the positions on all sources given the transform components.
    */
   void UpdatePositions(std::vector<TransformComponent> &transforms, std::vector<SoundComponent> &sounds);

   /**
    * @brief Updates the velocities on all sources given the physics component.
    */
   void UpdateVelocities(std::vector<PhysicsComponent> &physics, std::vector<SoundComponent> &sounds);

public:
    ALCdevice* mDevice;
    ALCcontext* mContext;
};

#endif // SOUNDMANAGER_H
