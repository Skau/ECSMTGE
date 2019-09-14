#include "openalmanager.h"

#include <QDebug>

OpenALManager::OpenALManager()
{
    qDebug() << "Intializing OpenAL!";
    mDevice = alcOpenDevice(nullptr);
    if (mDevice)
    {
        mContext = alcCreateContext(mDevice, nullptr);
        alcMakeContextCurrent(mContext);
    }

    checkOpenALError(alGetError());

    if (!mDevice)
        qDebug() << "Device not made!";
    else
        qDebug() << "Intialization complete!";
}

OpenALManager::~OpenALManager()
{
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(mContext);
    alcCloseDevice(mDevice);
}

bool OpenALManager::checkOpenALError(ALenum e)
{
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
