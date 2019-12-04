#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "componentdata.h"
#include "octree.h"
#include <utility>
#include <optional>

struct HitInfo
{
    unsigned int eID;
    unsigned int collidingEID;
    gsl::vec3 hitPoint;
    gsl::vec3 velocity;
    gsl::vec3 collidingNormal;

    // To be able to sort hitInfo's
    bool operator< (const HitInfo& rhs) const { return eID < rhs.eID; }
};

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
 * @brief Handles all physics / collisions that are needed.
 * @authors Skau, andesyv
 */
class PhysicsSystem
{
public:

    struct CollisionEntity
    {
        unsigned int eID;
        ColliderComponent::Bounds bounds;

        CollisionEntity(unsigned int id, const ColliderComponent::Bounds& b);
    };
    struct CubeNode
    {
        std::vector<CollisionEntity> entities;
        gsl::ivec3 from;
    };

    PhysicsSystem();
    static std::vector<HitInfo> UpdatePhysics(std::vector<TransformComponent>& transforms, std::vector<PhysicsComponent>& physics,
                              std::vector<ColliderComponent>& colliders, float deltaTime);



private:
    static std::vector<PhysicsSystem::CollisionEntity> updateBounds(std::vector<TransformComponent>& trans, std::vector<ColliderComponent>& colliders);
    static gsl::Octree<CubeNode> generateSceneTree(std::vector<TransformComponent>& trans, std::vector<ColliderComponent>& colliders);
    static void subdivideBranch(gsl::Octree<CubeNode>& branch);
    static std::vector<std::pair<gsl::ivec3, PhysicsSystem::CubeNode>> subdivide(const std::pair<gsl::ivec3, CubeNode> &node);
    static void updatePosVel(std::vector<TransformComponent>& transforms, std::vector<PhysicsComponent> &physics, float deltaTime);
    static std::optional<std::array<HitInfo, 2>> collisionCheck(std::tuple<const TransformComponent&, const ColliderComponent&, const gsl::vec3&> a,
                                                                std::tuple<const TransformComponent&, const ColliderComponent&, const gsl::vec3&> b);
    static void handleHitInfo(HitInfo info, TransformComponent *transform = nullptr, PhysicsComponent *physics = nullptr);
    static void fireHitEvent(HitInfo info);

    static TransformComponent* findInTransforms(std::vector<TransformComponent> &t, unsigned int eID);
    static ColliderComponent* findInColliders(std::vector<ColliderComponent> &t, unsigned int eID);

public:
    static gsl::vec3 ClosestPoint(const std::pair<gsl::vec3, gsl::vec3>& box, gsl::vec3 p);

    // --------- Collision checks --------------

    /**
     * @brief AABB vs AABB collision check using pairs of min and max coordinates
     * @param a - Axis Aligned Bounding Box as a pair of min and max coordinates
     * @param b - Axis Aligned Bounding Box as a pair of min and max coordinates
     * @return true if boxes overlap, false otherwise
     */
    static bool AABBAABB(const std::pair<gsl::vec3, gsl::vec3>& a, const std::pair<gsl::vec3, gsl::vec3>& b);
    /**
     * @brief AABB vs Sphere collision check using pairs of min and max coordinates and centre and radius.
     * @param a - Axis Aligned Bounding Box as a pair of min and max coordinates
     * @param b - Sphere as a pair of centre point and radius
     * @return true if sphere and box overlap, false otherwise
     */
    static bool AABBSphere(const std::pair<gsl::vec3, gsl::vec3>& a, const std::pair<gsl::vec3, float>& b);

    static bool SphereSphere(const std::pair<gsl::vec3, float>& a, const std::pair<gsl::vec3, float>& b);

    // ---------- Collision checks with hit info ---------------
    /**
     * @brief AABB vs AABB collision check using pairs of min and max coordinates, verbose edition
     * @param a - Axis Aligned Bounding Box as a pair of min and max coordinates
     * @param b - Axis Aligned Bounding Box as a pair of min and max coordinates
     * @param out - Outwards reference for more information about the hit
     * @return true if boxes overlap, false otherwise
     */
    static bool AABBAABB(const std::pair<gsl::vec3, gsl::vec3>& a, const std::pair<gsl::vec3, gsl::vec3>& b, std::array<HitInfo, 2>& out);
    /**
     * @brief AABB vs Sphere collision check using pairs of min and max coordinates and centre and radius, verbose edition
     * @param a - Axis Aligned Bounding Box as a pair of min and max coordinates
     * @param b - Sphere as a pair of centre point and radius
     * @param out - Outwards reference for more information about the hit
     * @return true if sphere and box overlap, false otherwise
     */
    static bool AABBSphere(const std::pair<gsl::vec3, gsl::vec3>& a, const std::pair<gsl::vec3, float>& b, std::array<HitInfo, 2>& out);

    static bool SphereSphere(const std::pair<gsl::vec3, float>& a, const std::pair<gsl::vec3, float>& b, std::array<HitInfo, 2>& out);
};

#endif // PHYSICSSYSTEM_H
