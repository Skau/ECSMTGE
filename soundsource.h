#ifndef SOUNDSOURCE_H
#define SOUNDSOURCE_H

#ifdef _WIN32
#include <al.h>
#include <alc.h>
#elif __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif

class SoundSource
{
public:
    SoundSource();
    ~SoundSource();

    ALuint mSource;
    ALuint mBuffer;
};

#endif
