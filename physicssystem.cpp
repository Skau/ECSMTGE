#include "physicssystem.h"
#include <chrono>
#include "entitymanager.h"

PhysicsSystem::PhysicsSystem()
{

}

PhysicsSystem::CollisionEntity::CollisionEntity(unsigned int id, const ColliderComponent::Bounds &b)
    : eID{id}, bounds{b}
{

}

void PhysicsSystem::UpdatePhysics(std::vector<TransformComponent> &transforms, std::vector<PhysicsComponent> &physics,
                                  std::vector<ColliderComponent> &colliders, float deltaTime)
{
    // 1. Update positions and velocities
    updatePosVel(transforms, physics, deltaTime);

    // 2. Calculate bounds
    // auto sceneTree = generateSceneTree(transforms, colliders);
    auto bounds = updateBounds(transforms, colliders);

    std::vector<HitInfo> hitInfos;
    // 3. Collision detection
    for (unsigned int i{0}; i < bounds.size(); ++i)
    {
        for (unsigned int j{i + 1}; j < bounds.size(); ++j)
        {
            if (AABBAABB(bounds.at(i).bounds.minMax(), bounds.at(j).bounds.minMax()))
            {
                auto ieID{bounds.at(i).eID}, jeID{bounds.at(j).eID};

                auto collisions = collisionCheck(
                {*gsl::find(transforms.begin(), transforms.end(), ieID), *gsl::find(colliders.begin(), colliders.end(), ieID)},
                {*gsl::find(transforms.begin(), transforms.end(), jeID), *gsl::find(colliders.begin(), colliders.end(), jeID)});

                if (collisions)
                {
                    hitInfos.push_back(collisions->at(0));
                    hitInfos.push_back(collisions->at(1));
                }
            }
        }
    }

    // 2. Collision detection
//    for (const auto &item : transforms)
//    {
//        hitInfos.push_back(getHitInfo(item));
//    }
//    HandleCollisions(transforms, colliders);

    // 3. Handle collisions
    for (const auto &item : hitInfos)
    {
        handleHitInfo(item);
    }

    // 4. Recursive update
}

std::vector<PhysicsSystem::CollisionEntity> PhysicsSystem::updateBounds(std::vector<TransformComponent> &trans, std::vector<ColliderComponent> &colliders)
{
    auto transIt = trans.begin();
    auto collIt = colliders.begin();

    bool collEnd{collIt == colliders.end()}; // For short circuit evaluation
    std::vector<PhysicsSystem::CollisionEntity> boundsList;
    boundsList.reserve(colliders.size());

    while (static_cast<bool>("we didn't start the fire"))
    {
        if (collIt == colliders.end())
            break;

        if (!collIt->valid)
        {
            ++collIt;
            continue;
        }

        if (!transIt->valid || transIt->entityId < collIt->entityId)
        {
            ++transIt;

            continue;
        }

        if (!collEnd)
        {
            if (!collIt->valid || collIt->entityId < transIt->entityId)
            {
                ++collIt;
                if (collIt == colliders.end())
                    collEnd = true;
                continue;
            }

            if (collIt->entityId == transIt->entityId)
            {
                if (transIt->colliderBoundsOutdated)
                {
                    // Calculate bounds
                    switch (collIt->collisionType)
                    {
                        float radius;

                        case ColliderComponent::SPHERE:
                            collIt->bounds.centre = gsl::vec3{0.f, 0.f, 0.f};
                            radius = std::get<float>(collIt->extents);
                            collIt->bounds.extents = gsl::vec3{radius * transIt->scale.x, radius * transIt->scale.y, radius * transIt->scale.z};
                            break;
                        case ColliderComponent::AABB:
                            gsl::vec3 min{};
                            gsl::vec3 max{};
                            auto mMatrix = gsl::mat4::modelMatrix(gsl::vec3{}, transIt->rotation, transIt->scale);
                            std::array<gsl::vec3, 2> points{(mMatrix * collIt->bounds.minMax().first).toVector3D(),
                                                            (mMatrix * collIt->bounds.minMax().second).toVector3D()};
                            for (const auto &p : points)
                            {
                                min.x = (p.x < min.x) ? p.x : min.x;
                                min.y = (p.y < min.y) ? p.y : min.y;
                                min.z = (p.z < min.z) ? p.z : min.z;

                                max.x = (max.x < p.x) ? p.x : max.x;
                                max.y = (max.y < p.y) ? p.y : max.y;
                                max.z = (max.z < p.z) ? p.z : max.z;
                            }
                            collIt->bounds.centre = (max - min) * 0.5f + min;
                            collIt->bounds.extents = gsl::abs(max - collIt->bounds.centre);
                            break;
                    }
                }

                auto relativeBounds = collIt->bounds;
                relativeBounds.centre += transIt->position;
                boundsList.emplace_back(collIt->entityId, relativeBounds);
            }
        }

        transIt->colliderBoundsOutdated = false;
        ++collIt;
        ++transIt;
    }

    return boundsList;
}

gsl::Octree<PhysicsSystem::CubeNode> PhysicsSystem::generateSceneTree(std::vector<TransformComponent> &trans, std::vector<ColliderComponent> &colliders)
{
    // std::chrono::high_resolution_clock::time_point t = std::chrono::high_resolution_clock::now();

    /* The tree defines squares reaching from the last square until
     * the coordinate at the specified key.
     * Example: tree(1, 0, 1) will be the square between (1, 0, 1) and (0, 0, 0),
     * unless theres a coordinate at (0.5, 0, 0.5), in which case the square
     * is half the size.
     *
     * Note: Tree should always be dividible by 2 since we subdivide into a power of 2.
     */
    gsl::Octree<CubeNode> tree{{}};

    // Fill tree with all entities
    tree.mValue.entities.reserve(colliders.size());
    for (auto entIt = colliders.begin(); entIt != colliders.end(); ++entIt)
        tree.mValue.entities.emplace_back(entIt->entityId, entIt->bounds);



    // Create first 8 nodes:
    gsl::ivec3 range{128, 128, 128};

    for (int z = -1; z < 2; z += 2)
        for (int y = -1; y < 2; y += 2)
            for (int x = -1; x < 2; x += 2)
            {
                gsl::ivec3 to{range.x * x, range.y * y, range.z * z};
                CubeNode node
                {
                    {},
                    {0, 0, 0}
                };
                node.entities.reserve(colliders.size());

                for (auto it = colliders.begin(); it != colliders.end(); ++it)
                    if (AABBAABB(it->bounds.minMax(), {to, {0, 0, 0}}))
                        node.entities.emplace_back(it->entityId, it->bounds);

                // A little optimization
                node.entities.shrink_to_fit();

                tree.insert(to, node);
                // tree(to.x, to.y, to.z) = node;
            }

    // Subdivide tree
    subdivideBranch(tree);


    // Update count
    tree.updateCount();

//    long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t).count();
//    qDebug() << "tree generation took " << duration << " ms";

//    auto lst = tree.preIt();
//    for (auto it = lst.begin(); it != lst.end(); ++it)
//    {
//        qDebug() << "Node at key: (" << (*it)->key().x << ", " << (*it)->key().y << ", " << (*it)->key().z << ") with from: (" << (*it)->mValue.from.x
//                 << ", " << (*it)->mValue.from.y << ", " << (*it)->mValue.from.z << ")";
//    }



    // qDebug() << "scenetree nodes: " << tree.nodes();
    return tree;
}

void PhysicsSystem::subdivideBranch(gsl::Octree<PhysicsSystem::CubeNode> &branch)
{
    // Make a iterative list of all root nodes (nodes without children)
    // Basically just to make sure we only run this algorithm on the ends.
    auto roots = branch.roots();

    /* Note: Because of this step, which is for optimization, the tree will
     * skip nodes that have children in order to get to the roots and will
     * only update the roots in the tree. Which means that the higher nodes
     * will then end up with the wrong count for subnodes. Making iterative
     * traversal slower (preIt() does som preallocation to save reallocation
     * time).
     * Therefore, until a better solution is found (and there definetively is),
     * it would be faster to make 1 node and then use this function to construct
     * the tree.
     */

    // Iterate through all roots
    for (auto it = roots.begin(); it != roots.end(); ++it)
    {
        // If the node has more than 2 entities in it, subdivide the node
        gsl::Octree<CubeNode>& node = **it;
        if (2 < node.mValue.entities.size())
        {
            auto subNodes = subdivide({node.key(), node.mValue});
            for (auto subNode = subNodes.begin(); subNode != subNodes.end(); ++subNode)
            {
                // We don't need to do collision testing in nodes with 1 or 0 entities. Discard.
                if (1 < subNode->second.entities.size())
                {
                    auto result = node.insert(subNode->first, subNode->second);
                    // If the node got inserted into the root, subdivide on that node again
                    // (incase there are more than 2 in that node)
                    if (result)
                        subdivideBranch(*result);
                }
            }
        }
    }
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

std::optional<std::array<PhysicsSystem::HitInfo, 2>> PhysicsSystem::collisionCheck(std::pair<const TransformComponent &, const ColliderComponent &> a,
                                                                    std::pair<const TransformComponent &, const ColliderComponent &> b)
{
    return std::nullopt;
}

PhysicsSystem::HitInfo PhysicsSystem::getHitInfo(const TransformComponent &transform, const ColliderComponent &collider)
{
    return HitInfo{};
}

void PhysicsSystem::handleHitInfo(PhysicsSystem::HitInfo info)
{

}

TransformComponent *PhysicsSystem::findInTransforms(std::vector<TransformComponent> &t, unsigned int eID)
{
    for (auto it{t.begin()}; it != t.end(); ++it)
    {
        if (it->entityId == eID)
            return &(*it);
    }
    return nullptr;
}

ColliderComponent *PhysicsSystem::findInColliders(std::vector<ColliderComponent> &t, unsigned int eID)
{
    for (auto it{t.begin()}; it != t.end(); ++it)
    {
        if (it->entityId == eID)
            return &(*it);
    }
    return nullptr;
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
