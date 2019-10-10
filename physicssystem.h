#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "componentdata.h"
#include "octree.h"

/** Physics system
 * The idea is to later move this system to
 * another thread while leaving rendering and inputs
 * on the main thread. This allowing physics calculations,
 * which are expensive, to have their own thread to work
 * on and not get in the way of the rest of the program.
 * As physics calculation often is the slowest and
 * the part that most heavily relies on the CPU,
 * we can by putting these calculations on a seperate thread
 * with its own clock have it run in the background on its
 * own pace. This means that rendering will be updated 2-3
 * times more often than physics.
 *
 * @brief Physic System
 * @authors Skau, andesyv
 */

class PhysicsSystem
{
public:
    struct HitInfo
    {
        gsl::vec3 hitPoint;
        gsl::vec3 velocity;
        gsl::vec3 normal;
    };

    PhysicsSystem();
    static void UpdatePhysics(std::vector<TransformComponent>& transforms, std::vector<PhysicsComponent>& physics, std::vector<ColliderComponent>& colliders, float deltaTime);

private:
    static Octree<std::vector<unsigned int>> generateSceneTree(std::vector<TransformComponent>& trans, std::vector<ColliderComponent>& colliders);
    static void updatePosVel(std::vector<TransformComponent>& transforms, std::vector<PhysicsComponent> &physics, float deltaTime);
    static void HandleCollisions(std::vector<TransformComponent>& transform, std::vector<ColliderComponent>& collider);
    static HitInfo getHitInfo(const TransformComponent& transform, const ColliderComponent& collider);
    static void handleHitInfo(HitInfo info);
};

#endif // PHYSICSSYSTEM_H
