#ifndef VISUALOBJECT_H
#define VISUALOBJECT_H

#include <QOpenGLFunctions_4_1_Core>
#include <vector>
#include "vertex.h"
#include "matrix4x4.h"
#include "material.h"
#include "shader.h"

#include "LinearMath/btTransform.h"
#include "LinearMath/btVector3.h"

class btCollisionShape;
class btRigidBody;

class RenderWindow;

class VisualObject : public QOpenGLFunctions_4_1_Core {
public:
    VisualObject();
    virtual ~VisualObject();
    virtual void init();
    virtual void draw()=0;

    gsl::Matrix4x4 mMatrix;

    void setShader(Shader *shader);

    std::string mName;

    RenderWindow *mRenderWindow; //Just to be able to call checkForGLerrors()

    Material mMaterial;

    void update();
    void initializeRigidBody(float mass);
    btTransform mTransform;
    btCollisionShape* mCollision;
    btRigidBody* mRigidBody;
    btVector3 mLocalInertia{0, 0, 0};
    float mMass;


protected:
    std::vector<Vertex> mVertices;   //This is usually not needed after object is made
    std::vector<GLuint> mIndices;    //This is usually not needed after object is made

    GLuint mVAO{0};
    GLuint mVBO{0};
    GLuint mEAB{0}; //holds the indices (Element Array Buffer - EAB)

};
#endif // VISUALOBJECT_H

