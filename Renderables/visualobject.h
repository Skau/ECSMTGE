#ifndef VISUALOBJECT_H
#define VISUALOBJECT_H

#include <QOpenGLFunctions_4_1_Core>
#include <vector>
#include <memory>
#include "vertex.h"
#include "matrix4x4.h"
#include "material.h"
#include "Shaders/shader.h"

class Renderer;

class VisualObject : public QOpenGLFunctions_4_1_Core {
public:
    VisualObject();
    virtual ~VisualObject();
    virtual void init();
    virtual void draw()=0;

    gsl::Matrix4x4 mMatrix;

    void setShader(std::shared_ptr<Shader> shader);

    std::string mName;

    Renderer *mRenderWindow; //Just to be able to call checkForGLerrors()

    MaterialClass mMaterial;

protected:
    std::vector<Vertex> mVertices;   //This is usually not needed after object is made
    std::vector<GLuint> mIndices;    //This is usually not needed after object is made

    GLuint mVAO{0};
    GLuint mVBO{0};
    GLuint mEAB{0}; //holds the indices (Element Array Buffer - EAB)

};
#endif // VISUALOBJECT_H

