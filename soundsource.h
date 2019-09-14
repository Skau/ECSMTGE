#ifndef SOUNDSOURCE_H
#define SOUNDSOURCE_H

#ifdef _WIN32
#include <al.h>
#include <alc.h>
#elif __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif

#include <string>
#include "innpch.h"

/// One sound source.
/**
    Contains one source and one buffer.
    Intended to play a single clip.
    Does not support streaming audio (yet).
**/
class SoundSource
{
public:
    /// Constructor.
    /**
        Takes two arguments, initialises some variables.
        \param The name of the sound source. Not used.
        \param A boolean to check if sound should loop.
    **/
    SoundSource(std::string name, bool loop = false, float gain = 1.0);
    ~SoundSource();

    /// Plays the sound source from the current position.
    void play();
    /// Pauses the sound source at the current position.
    void pause();
    /// Stops the sound source and rewinds to start.
    void stop();

    void setPosition(gsl::vec3 newPos);             ///< Sets source position from Vector3.
    gsl::vec3 getPosition() {return mPosition;}     ///< Returns source position as Vector3.
    void setVelocity(gsl::vec3 newVel);             ///< Sets source velocity from Vector3.
    gsl::vec3 getVelocity() {return mVelocity;}     ///< Returns source velocity as Vector3.

    std::string mName;          ///< The name of the sound source (Not used).
    ALuint mSource;             ///< The sound source.
    ALuint mBuffer;             ///< The data buffer.

private:
    gsl::vec3 mPosition;    ///< Vector containing source position.
    gsl::vec3 mVelocity;    ///< Vector containing source velocity.
};

#endif
