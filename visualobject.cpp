#include "innpch.h"
#include "visualobject.h"
#include "shader.h"

#include "world.h"

VisualObject::VisualObject()
{
    mTransform.setIdentity();
}

VisualObject::~VisualObject()
{
   glDeleteVertexArrays( 1, &mVAO );
   glDeleteBuffers( 1, &mVBO );
}

void VisualObject::init()
{
}

void VisualObject::setShader(Shader *shader)
{
    mMaterial.mShader = shader;
}

void VisualObject::update()
{
   mMatrix.setToIdentity();
   auto transform = mRigidBody->getWorldTransform();
   mMatrix.translate(transform.getOrigin().getX(), transform.getOrigin().getY(), transform.getOrigin().getZ());
}

void VisualObject::initializeRigidBody(float mass)
{
    mMass = mass;
    mCollision = new btBoxShape(btVector3(1,1,1));
    if(mMass != 0.f)
    {
        mCollision->calculateLocalInertia(mMass, mLocalInertia);
    }
    //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(mTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mMass, myMotionState, mCollision, mLocalInertia);
    mRigidBody = new btRigidBody(rbInfo);

    World::instance()->getDynamicsWorld()->addRigidBody(mRigidBody);
}
