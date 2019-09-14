#include "soundsource.h"
#include "wavfilehandler.h"
#include <sstream>
#include <iostream>

SoundSource::SoundSource(std::string name, bool loop, float gain) :
    mName(name),
    mSource(0),
    mBuffer(0),
    mPosition(0.0f, 0.0f, 0.0f),
    mVelocity(0.0f, 0.0f, 0.0f)
{
    alGetError();
    alGenBuffers(1, &mBuffer);
    alGenSources(1, &mSource);
    alSourcef(mSource, AL_PITCH, 1.0f);
    alSourcef(mSource, AL_GAIN, gain);

    ALfloat temp[3] = {mPosition.x, mPosition.y, mPosition.z};
    alSourcefv(mSource, AL_POSITION, temp);
    ALfloat temp2[3] = {mVelocity.x, mVelocity.y, mVelocity.z};
    alSourcefv(mSource, AL_VELOCITY, temp2);

    alSourcei(mSource, AL_LOOPING, loop);
}

SoundSource::~SoundSource()
{
    std::cout << "Destroying SoundSource " + mName;
    stop();
    alGetError();
    alSourcei(mSource, AL_BUFFER, 0);
    alDeleteSources(1, &mSource);
    alDeleteBuffers(1, &mBuffer);
}

void SoundSource::play()
{
    alSourcePlay(mSource);
}
void SoundSource::pause()
{
    alSourcePause(mSource);
}
void SoundSource::stop()
{
    alSourceStop(mSource);
}

void SoundSource::setPosition(gsl::vec3 newPos)
{
    mPosition = newPos;
    ALfloat temp[3] = {mPosition.x, mPosition.y, mPosition.z};
    alSourcefv(mSource, AL_POSITION, temp);
}
void SoundSource::setVelocity(gsl::vec3 newVel)
{
    mVelocity = newVel;
    ALfloat temp[3] = {mVelocity.x, mVelocity.y, mVelocity.z};
    alSourcefv(mSource, AL_VELOCITY, temp);
}
