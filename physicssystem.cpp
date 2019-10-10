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

Octree<PhysicsSystem::CubeNode> PhysicsSystem::generateSceneTree(std::vector<TransformComponent> &trans, std::vector<ColliderComponent> &colliders)
{


    Octree<CubeNode> tree{static_cast<int>(2 * trans.size())};

    /* The tree defines squares reaching from the last square until
     * the coordinate at the specified key.
     * Example: tree(1, 0, 1) will be the square between (1, 0, 1) and (0, 0, 0),
     * unless theres a coordinate at (0.5, 0, 0.5), in which case the square
     * is half the size.
     */

    // typedef std::pair<gsl::ivec3, std::vector<CollisionEntity>> Node;

}

std::vector<std::pair<gsl::ivec3, PhysicsSystem::CubeNode> > PhysicsSystem::subdivide(const std::pair<gsl::ivec3, CubeNode> &node)
{
    std::vector<std::pair<gsl::ivec3, PhysicsSystem::CubeNode>> children{};
    children.reserve(8);

    gsl::ivec3 range{node.first - node.second.from};
    if ((range / 2).x < 1)
    {
        // Range is too low to be divided integer style. Logic error.
        throw std::logic_error{"Range is not subdividable!"};
    }

    // Iterate through all possible nodes:
    for (int z = 0; z < 2; ++z)
        for (int y = 0; y < 2; ++y)
            for (int x = 0; x < 2; ++x)
            {
                // Make a new node with new boundaries
                auto d = range / gsl::ivec3{2, 2, 2};
                gsl::ivec3 to{node.second.from + d * gsl::ivec3{1 + x, 1 + y, 1 + z}};
                std::pair<gsl::ivec3, PhysicsSystem::CubeNode> subNode
                {
                    to,
                    PhysicsSystem::CubeNode
                    {
                        {},
                        node.second.from + d * gsl::ivec3{x, y, z}
                    }
                };

                // Optimization
                subNode.second.entities.reserve(node.second.entities.size());

                // Put all entities that fit inside this node into the node
                for (auto it = node.second.entities.begin(); it != node.second.entities.end(); ++it)
                    if (AABBAABB(it->bounds.minMax(), {to, subNode.second.from}))
                        subNode.second.entities.push_back(*it);

                children.push_back(subNode);
            }
    return children;
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

bool PhysicsSystem::AABBAABB(const std::pair<gsl::vec3, gsl::vec3> &a, const std::pair<gsl::vec3, gsl::vec3> &b)
{
    // min = first, max = second
    // return A.min <= B.max && a.max >= b.min
    return
        (a.first.x <= b.second.x && a.second.x >= b.first.x) &&
        (a.first.y <= b.second.y && a.second.y >= b.first.y) &&
        (a.first.z <= b.second.z && a.second.z >= b.first.z);

}

bool PhysicsSystem::AABBSphere(const std::pair<gsl::vec3, gsl::vec3> &a, const std::pair<gsl::vec3, float> &b)
{
    return false;
}
