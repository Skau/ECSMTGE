#include "soundmanager.h"

#include <QDebug>

#include "resourcemanager.h"
#include "componentdata.h"

SoundManager::SoundManager()
{
    qDebug() << "Intializing OpenAL!";
    mDevice = alcOpenDevice(nullptr);
    if (mDevice)
    {
        mContext = alcCreateContext(mDevice, nullptr);
        alcMakeContextCurrent(mContext);
    }

    checkOpenALError();

    if (!mDevice)
        qDebug() << "Device not made!";
    else
        qDebug() << "Intialization complete!";

    checkOpenALError();
}

SoundManager::~SoundManager()
{
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(mContext);
    alcCloseDevice(mDevice);
}

bool SoundManager::checkOpenALError()
{
    auto e = alGetError();
    switch (e)
    {
    case AL_NO_ERROR:
        break;
    case AL_INVALID_NAME:
        qDebug() << "Invalid name!";
        return false;
    case AL_INVALID_ENUM:
        qDebug() << "Invalid enum!";
        return false;
    case AL_INVALID_VALUE:
        qDebug() << "Invalid value!";
        return false;
    case AL_INVALID_OPERATION:
        qDebug() << "Invalid operation!";
        return false;
    case AL_OUT_OF_MEMORY:
        qDebug() << "Out of memory!";
        return false;
    default: break;
    }
    return true;
}

void SoundManager::createSource(SoundComponent* comp, const std::string& wav)
{
    if(!comp || !wav.size()) return;

    checkOpenALError();

    if(auto waveData = ResourceManager::instance()->getWav(wav))
    {
        ALuint source;
        alGenSources(1, &source);

        ALuint buffer;

        auto sourceBuffer = ResourceManager::instance()->getSourceBuffer(wav);

        if(sourceBuffer > -1)
        {
            buffer = static_cast<ALuint>(sourceBuffer);
        }
        else
        {
            ALuint frequency{};
            ALenum format{};

            frequency = waveData->sampleRate;

            switch (waveData->bitsPerSample)
            {
            case 8:
                switch (waveData->channels)
                {
                case 1:
                    format = AL_FORMAT_MONO8;
                    break;
                case 2:
                    format = AL_FORMAT_STEREO8;
                    break;
                default: break;
                }
                break;
            case 16:
                switch (waveData->channels)
                {
                case 1:
                    format = AL_FORMAT_MONO16;
                    break;
                case 2:
                    format = AL_FORMAT_STEREO16;
                    break;
                default: break;
                }
                break;
            default: break;
            }

            if (waveData->buffer == nullptr)
            {
                qDebug() << "Error: No wave data!";
            }

            alGenBuffers(1, &buffer);
            alBufferData(buffer, format, waveData->buffer, static_cast<ALsizei>(waveData->dataSize), static_cast<ALsizei>(frequency));
        }


        alSourcei(source, AL_BUFFER, static_cast<ALint>(buffer));

        checkOpenALError();

        comp->mSource = static_cast<int>(source);
        comp->name = wav;

        setMute(source, comp->isMuted, comp->gain);
        changePitch(source, comp->pitch);
        setLooping(source, comp->isLooping);
        setPosition(source);
        setVelocity(source);
    }
}

void SoundManager::play(unsigned source)
{
    alSourcePlay(source);
    SoundManager::checkOpenALError();
}

void SoundManager::pause(unsigned source)
{
    alSourcePause(source);
    checkOpenALError();
}

void SoundManager::stop(unsigned source)
{
    alSourceStop(source);
    checkOpenALError();
}

void SoundManager::changeGain(unsigned source, float value)
{
    alSourcef(source, AL_GAIN, value);
    checkOpenALError();
}

void SoundManager::changePitch(unsigned source, float value)
{
    alSourcef(source, AL_PITCH, value);
    checkOpenALError();
}

void SoundManager::setLooping(unsigned source, bool value)
{
    alSourcei(source, AL_LOOPING, value);
    checkOpenALError();
}

void SoundManager::setMute(unsigned source, bool value, float gain)
{
    changeGain(source, (!value) ? gain : 0);
}

void SoundManager::setPosition(unsigned source, gsl::vec3 position)
{
    ALfloat temp[3] = {position.x, position.y, position.z};
    alSourcefv(source, AL_POSITION, temp);
    checkOpenALError();
}

void SoundManager::setVelocity(unsigned source, gsl::vec3 velocity)
{
    ALfloat temp[3] = {velocity.x, velocity.y, velocity.z};
    alSourcefv(source, AL_VELOCITY, temp);
    checkOpenALError();
}

void SoundManager::cleanupSource(unsigned source)
{
    alSourcei(source, AL_BUFFER, 0);
    alDeleteSources(1, &source);
    checkOpenALError();
}

void SoundManager::UpdatePositions(std::vector<TransformComponent>& transforms, std::vector<SoundComponent>& sounds)
{
    auto transIt = transforms.begin();
    auto soundIt = sounds.begin();

    bool transformShortest = transforms.size() < sounds.size();
    bool _{true};
    // cause normal while (true) loops are so outdated
    for ( ;_; )
    {
        if (transformShortest)
        {
            if (transIt == transforms.end())
                break;
        }
        else
        {
            if (soundIt == sounds.end())
                break;
        }

        // Increment lowest index
        if (!transIt->valid || transIt->entityId < soundIt->entityId)
        {
            ++transIt;
        }
        else if (!soundIt->valid || soundIt->entityId < transIt->entityId)
        {
            ++soundIt;
        }
        else
        {

            if(soundIt->mSource > -1)
            {
                setPosition(static_cast<unsigned>(soundIt->mSource), transIt->position);
            }

            // Increment all
            ++transIt;
            ++soundIt;
        }
    }
}

void SoundManager::UpdateVelocities(std::vector<PhysicsComponent>& physics, std::vector<SoundComponent>& sounds)
{
    auto physicsIt = physics.begin();
    auto soundIt = sounds.begin();

    bool transformShortest = physics.size() < sounds.size();
    bool _{true};
    // cause normal while (true) loops are so outdated
    for ( ;_; )
    {
        if (transformShortest)
        {
            if (physicsIt == physics.end())
                break;
        }
        else
        {
            if (soundIt == sounds.end())
                break;
        }

        // Increment lowest index
        if (!physicsIt->valid || physicsIt->entityId < soundIt->entityId)
        {
            ++physicsIt;
        }
        else if (!soundIt->valid || soundIt->entityId < physicsIt->entityId)
        {
            ++soundIt;
        }
        else
        {
            if(soundIt->mSource > -1)
            {
                setVelocity(static_cast<unsigned>(soundIt->mSource), physicsIt->velocity);
            }

            // Increment all
            ++physicsIt;
            ++soundIt;
        }
    }
}
