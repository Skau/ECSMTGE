#include "physicssystem.h"

PhysicsSystem::PhysicsSystem()
{

}

void PhysicsSystem::UpdatePhysics(std::vector<TransformComponent> &transforms, std::vector<PhysicsComponent> &physics,
                                  std::vector<ColliderComponent> &colliders, float deltaTime)
{
    // 1. Update positions and velocities
    updatePosVel(transforms, physics, deltaTime);

    std::vector<HitInfo> hitInfos;
    // 2. Collision detection
//    for (const auto &item : transforms)
//    {
//        hitInfos.push_back(getHitInfo(item));
//    }
    HandleCollisions(transforms, colliders);

    // 3. Handle collisions
    for (const auto &item : hitInfos)
    {
        handleHitInfo(item);
    }

    // 4. Recursive update
}

void PhysicsSystem::updatePosVel(std::vector<TransformComponent> &transforms, std::vector<PhysicsComponent> &physics, float deltaTime)
{
    auto transIt = transforms.begin();
    auto physIt = physics.begin();

    bool physEnd{physIt == physics.end()}; // For short circuit evaluation
    bool _{true};

    // cause normal while (true) loops are so outdated
    for ( ;_; )
    {
        if (transIt == transforms.end())
            break;

        if (!transIt->valid)
        {
            ++transIt;
            continue;
        }

        if (!physEnd && transIt->entityId == physIt->entityId) {
            if (transIt->entityId > physIt->entityId || !physIt->valid)
            {
                ++physIt;
                if (physIt == physics.end())
                    physEnd = true;
                continue;
            }

            // Apply acceleration to velocity and then velocity to position
            physIt->velocity += physIt->acceleration * deltaTime;
            transIt->position += physIt->velocity * deltaTime;
            transIt->updated = true;

            ++physIt;
        }
        else
        {
            // Update transforms without physics

            // Question: Would a transform ever be updated if there
            // are no velocity, forces og collisions applied?
            // - and would then this else clause be needed?
        }

        ++transIt;
    }
}

void PhysicsSystem::HandleCollisions(std::vector<TransformComponent> &transform, std::vector<ColliderComponent> &collider)
{

}

PhysicsSystem::HitInfo PhysicsSystem::getHitInfo(const TransformComponent &transform, const ColliderComponent &collider)
{
    return HitInfo{};
}

void PhysicsSystem::handleHitInfo(PhysicsSystem::HitInfo info)
{

}
