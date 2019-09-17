#include "soundsource.h"
#include "wavfilehandler.h"
#include <sstream>
#include <iostream>

SoundSource::SoundSource() :
    mSource(0),
    mBuffer(0)
{

}

SoundSource::~SoundSource()
{
    alGetError();
    alDeleteBuffers(1, &mBuffer);
}
