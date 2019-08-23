#ifndef WORLD_H
#define WORLD_H

#include "btBulletDynamicsCommon.h"

class World
{
public:
    static World* instance()
    {
        static World instance;
        return &instance;
    }

    void AddCollisionShape(btCollisionShape* collisionShape)
    {
        if(collisionShape)
            mCollisionShapes.push_back(collisionShape);
    }

    btDiscreteDynamicsWorld* getDynamicsWorld() { return mDynamicsWorld; }

    void step(float DeltaTime);

private:
    World();
    ~World();

    btDiscreteDynamicsWorld* mDynamicsWorld{nullptr};

    btAlignedObjectArray<btCollisionShape*> mCollisionShapes;
};

#endif // WORLD_H
