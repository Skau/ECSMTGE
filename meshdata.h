#ifndef MESHDATA_H
#define MESHDATA_H

#include "gltypes.h"

struct MeshData
{
    unsigned int VAO{};
    unsigned int indicesCount{};
    GLenum renderType{};
};

#endif // MESHDATA_H
