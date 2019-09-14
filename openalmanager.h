#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#ifdef _WIN32
#include <al.h>
#include <alc.h>
#elif __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif

/**
 * @brief Holds the context and current sound device
 */
class OpenALManager
{
public:
    OpenALManager();
    ~OpenALManager();

   static bool checkOpenALError(ALenum e);

public:

    ALCdevice* mDevice;
    ALCcontext* mContext;
};

#endif // SOUNDMANAGER_H
