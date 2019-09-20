#ifndef SKYBOX_H
#define SKYBOX_H

#include "componentdata.h"
#include <QOpenGLFunctions_4_1_Core>

class Skybox : public QOpenGLFunctions_4_1_Core
{
public:
    Skybox();

    DirectionalLightComponent* mSun;
};

#endif // SKYBOX_H
