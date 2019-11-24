#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include "componentdata.h"
#include <typeinfo>
#include <vector>
#include "resourcemanager.h"
#include <QObject>
#include <QDebug>
#include <queue>


/** Precompiler directives
 * We had to instance some template functions,
 * so then we templated the instanced functions
 * with a precompiler directive.
 * These defines replace some codes with their
 * respective instanced typename K.
 * This removed a lot of bloat and copy pasting of code.
 */


/** Registers the component in the manager
 * 1. Constructs the vector container for the components of type K.
 * 2. Creates a getter of the vector used in the corresponding systems that needs them.
 * 3. Creates a getter for a given entity.
 * 4. Creates a remove component function for a given entity.
 * 5. Creates an add component function for a given entity.
 */
#define REGISTER(K) \
    private: \
    std::vector<K> CONCATENATE(m, K, s); \
    public: \
    std::vector<K>& CONCATENATE(get, K, s)() { return CONCATENATE(m, K, s); } \
    GETCOMPONENT(K) \
    REMOVECOMPONENT(K) \
    private: \
    ADDCOMPONENT(K) \


#define GETCOMPONENT(K) \
template<class T, \
    typename std::enable_if<(std::is_same<K, T>::value)>::type* = nullptr> \
T* getComponent(unsigned int entity) { return find(CONCATENATE(m, K, s).begin(), CONCATENATE(m, K, s).end(), entity); } \

#define REMOVECOMPONENT(K) \
template<class T, \
         typename std::enable_if<(std::is_same<K, T>::value)>::type* = nullptr> \
bool removeComponent(unsigned int entity) \
{ \
    auto it = std::lower_bound(CONCATENATE(m, K, s).begin(), CONCATENATE(m, K, s).end(), entity, [](const K& a, const unsigned int& b){ \
        return a.entityId < b; \
    }); \
    if (it != CONCATENATE(m, K, s).end() && it->entityId == entity) \
    { \
        if (it->valid) \
        { \
            it->valid = false; \
            return true; \
        } \
    } \
    return false; \
} \

#define ADDCOMPONENT(K) \
    template<class T, typename std::enable_if<(std::is_same<K, T>::value)>::type* = nullptr> \
    T& addComponents(unsigned int entity) \
    { \
        for (auto it{CONCATENATE(m, K, s).begin()}; it != CONCATENATE(m, K, s).end(); ++it) \
        { \
            if (!it->valid) \
            { \
                it->~K(); \
                new (&(*it)) K{entity, true}; \
                std::sort(it, CONCATENATE(m, K, s).end(),[](const K& t1, const K& t2) \
                { \
                    return t1.entityId < t2.entityId; \
                }); \
                if (it->entityId == entity) \
                    return *it; \
                else \
                { \
                    return *std::lower_bound(it, CONCATENATE(m, K, s).end(), entity, [](const K& a, const unsigned int& b){ \
                        return a.entityId < b; \
                    }); \
                } \
            } \
        } \
        CONCATENATE(m, K, s).emplace_back(entity, true); \
        auto &comp = CONCATENATE(m, K, s).back(); \
        std::sort(CONCATENATE(m, K, s).begin(), CONCATENATE(m, K, s).end(),[](const K& t1, const K& t2) \
        { \
            return t1.entityId < t2.entityId; \
        }); \
        if (comp.entityId == entity) \
            return comp; \
        else \
            return *std::lower_bound(CONCATENATE(m, K, s).begin(), CONCATENATE(m, K, s).end(), entity, [](const K& a, const unsigned int& b){ \
                return a.entityId < b; \
            }); \
    } \

#define CONCATENATE( x, y, z) x##y##z


/** A component data manager class
 * Constructs entities and manages
 * component data for each entity.
 *
 * @author andesyv, Skau
 */
class EntityManager : public QObject
{
    Q_OBJECT

    REGISTER(TransformComponent)
    REGISTER(MeshComponent)
    REGISTER(PhysicsComponent)
    REGISTER(CameraComponent)
    REGISTER(InputComponent)
    REGISTER(SoundComponent)
    REGISTER(DirectionalLightComponent)
    REGISTER(SpotLightComponent)
    REGISTER(PointLightComponent)
    REGISTER(ScriptComponent)
    REGISTER(ColliderComponent)
    REGISTER(ParticleComponent)
    REGISTER(EntityInfo)

private:
    // ------------------------------ Member Variables -----------------------------
    std::vector<unsigned> entitiesToDestroy;
    unsigned int idCounter{0};

    void removeEntity(unsigned entity)
    {
        for(auto& comp : getAllComponents(entity))
        {
            comp->valid = false;
            comp->reset();
        }
        for(unsigned i = 0; i < mEntityInfos.size(); ++i)
        {
            if(mEntityInfos[i].entityId == entity)
            {
                mEntityInfos.erase(mEntityInfos.begin() + i);
                break;
            }
        }
    }

signals:
    void updateUI(const std::vector<EntityInfo>& entityData);

    // ------------------------- Member functions ---------------
public:
    EntityManager()
    {

    }

    /** Resets all components to be "empty" while keeping memory allocation.
     * @brief clear the entitymanager
     */
    void clear()
    {
        for(auto info : mEntityInfos)
        {
            for(auto& comp : getAllComponents(info.entityId))
            {
                comp->valid = false;
                comp->reset();
            }
        }

        idCounter = 0;
        mEntityInfos.clear();
        updateUI({});
    }

    void createObject(int index)
    {
        switch (index)
        {
        case 0:
        {
            createEntity();
            break;
        }
        case 1:
        {
            createCube();
            break;
        }
        case 2:
        {
            createMonkey();
            break;
        }
        }
    }


    unsigned int createEntity(std::string name = "")
    {
        auto id = ++idCounter;
        EntityInfo entityInfo;
        entityInfo.entityId = id;
        if(!name.size())
        {
            name = "Entity" + std::to_string(id);
        }
        entityInfo.name = name;
        mEntityInfos.push_back(entityInfo);
        updateUI(mEntityInfos);
        return id;
    }

    unsigned createCube()
    {
        auto id = createEntity();
        addComponent<MeshComponent, TransformComponent>(id);
        auto render = getComponent<MeshComponent>(id);
        if(auto mesh = ResourceManager::instance().getMesh("box2"))
        {
            render->meshData = *mesh;
            render->isVisible = true;
        }
        return id;
    }

    unsigned createMonkey()
    {
        auto id = createEntity();
        addComponent<MeshComponent, TransformComponent>(id);
        auto render = getComponent<MeshComponent>(id);
        if(auto mesh = ResourceManager::instance().getMesh("suzanne"))
        {
            render->meshData = *mesh;
            render->isVisible = true;
        }
        return id;
    }

    void removeEntityLater(unsigned entity)
    {
        entitiesToDestroy.push_back(entity);
    }

    void removeEntitiesMarked()
    {
        if(entitiesToDestroy.empty())
            return;

        for(auto& entity : entitiesToDestroy)
        {
            removeEntity(entity);
        }
        entitiesToDestroy.clear();
    }

    /** Runs a function on a entity component based on components in template.
     * Uses variadic template packing and std::function to take in a varying number
     * of parameters, attempts to get the components specified and sends them as
     * parameters to the function.
     * @param componentTypes - the different component types to get
     * @param eID - entityID
     * @param f - std::function wrapper object of function with equal parameter count
     * as the count of componentTypes.
     */
    template <typename... componentTypes>
    void transform(unsigned int eID, const std::function<void(componentTypes*...)>& f)
    {
        auto comps = getComponents<componentTypes...>(eID);
        std::apply(f, comps);
    }

    /** Runs a function on a entity component based on components in template.
     * Similar to other version, but takes a lambda instead of a std::function,
     * but lambda type needs to be sent in as first template parameter for
     * compiler to understand. (use decltype on lambda to get type)
     * @param FT - Lambda Type
     * @param componentTypes - the different component types to get
     * @param eID - entityID
     * @param f - lambda for function with equal parameter count
     */
    template <typename FT, typename... componentTypes>
    void transform(unsigned int eID, FT f)
    {
        auto comps = getComponents<componentTypes...>(eID);
        std::apply(f, comps);
    }

    /**
     * @brief Variadic template function that adds all components types given to an entity
     */
    template<typename... componentTypes>
    std::tuple<componentTypes&...> addComponent(unsigned int entity)
    {
        return {addComponents<componentTypes>(entity)...};
    }

    /**
     * @brief Variadic template function that returns all component types given for an entity.
     * @return Returns a tuple containing the components. If the entity does not have the respective component
     * the variable will be null.
     *
     */
    template<typename... componentTypes>
    std::tuple<componentTypes*...> getComponents(unsigned int entity)
    {
        return {getComponent<componentTypes>(entity)...};
    }

    /**
     * @brief Adds a component to a given entity using the ComponentType enum.
     */
    Component* addComponent(unsigned int entity, ComponentType type)
    {
        Component* returnComp{nullptr};
        switch (type)
        {
        case ComponentType::Mesh:
        {
            auto [component] = addComponent<MeshComponent>(entity);
            returnComp = &component;
            break;
        }
        case ComponentType::Transform:
        {
            auto [component] = addComponent<TransformComponent>(entity);
            returnComp = &component;
            break;
        }
        case ComponentType::Physics:
        {
            auto [component] = addComponent<PhysicsComponent>(entity);
            returnComp = &component;
            break;
        }
        case ComponentType::Camera:
        {
            auto [component] = addComponent<CameraComponent>(entity);
            returnComp = &component;
            break;
        }
        case ComponentType::Input:
        {
            auto [component] = addComponent<InputComponent>(entity);
            returnComp = &component;
            break;
        }
        case ComponentType::Sound:
        {
            auto [component] = addComponent<SoundComponent>(entity);
            returnComp = &component;
            break;
        }
        case ComponentType::LightSpot:
        {
            auto [component] = addComponent<SpotLightComponent>(entity);
            returnComp = &component;
            break;
        }
        case ComponentType::LightDirectional:
        {
            auto [component] = addComponent<DirectionalLightComponent>(entity);
            returnComp = &component;
            break;
        }
        case ComponentType::LightPoint:
        {
            auto [component] = addComponent<PointLightComponent>(entity);
            returnComp = &component;
            break;
        }
        case ComponentType::Script:
        {
            auto [component] = addComponent<ScriptComponent>(entity);
            returnComp = &component;
            break;
        }
        case ComponentType::Collider:
        {
            auto [component] = addComponent<ColliderComponent>(entity);
            returnComp = &component;
            break;
        }
        case ComponentType::Particle:
        {
            auto [component] = addComponent<ParticleComponent>(entity);
            returnComp = &component;
            break;
        }
        default:
            break;
        }

        return returnComp;
    }

    /**
     * @brief Gets a component to a given entity using the ComponentType enum.
     */
    Component* getComponent(unsigned int entity, ComponentType type)
    {
        Component* returnComp{nullptr};
        switch (type)
        {
        case ComponentType::Mesh:
        {
            returnComp = getComponent<MeshComponent>(entity);
            break;
        }
        case ComponentType::Transform:
        {
            returnComp = getComponent<TransformComponent>(entity);
            break;
        }
        case ComponentType::Physics:
        {
            returnComp = getComponent<PhysicsComponent>(entity);
            break;
        }
        case ComponentType::Camera:
        {
            returnComp = getComponent<CameraComponent>(entity);
            break;
        }
        case ComponentType::Input:
        {
            returnComp = getComponent<InputComponent>(entity);
            break;
        }
        case ComponentType::Sound:
        {
            returnComp = getComponent<SoundComponent>(entity);
            break;
        }
        case ComponentType::LightSpot:
        {
            returnComp = getComponent<SpotLightComponent>(entity);
            break;
        }
        case ComponentType::LightDirectional:
        {
            returnComp = getComponent<DirectionalLightComponent>(entity);
            break;
        }
        case ComponentType::LightPoint:
        {
            returnComp = getComponent<PointLightComponent>(entity);
            break;
        }
        case ComponentType::Script:
        {
            returnComp = getComponent<ScriptComponent>(entity);
            break;
        }
        case ComponentType::Collider:
        {
            returnComp = getComponent<ColliderComponent>(entity);
            break;
        }
        case ComponentType::Particle:
        {
            returnComp = getComponent<ParticleComponent>(entity);
            break;
        }
        default:
            break;
        }

        return returnComp;
    }

    /**
     * @brief Returns all components for a given entity.
     */
    std::vector<Component*> getAllComponents(unsigned int entity)
    {
        std::vector<Component*> components;
        if(auto comp = getComponent<TransformComponent>(entity))
        {
            if(comp->valid)
            {
                components.push_back(comp);
            }
        }
        if(auto comp = getComponent<MeshComponent>(entity))
        {
            if(comp->valid)
            {
                components.push_back(comp);
            }
        }
        if(auto comp = getComponent<PhysicsComponent>(entity))
        {
            if(comp->valid)
            {
                components.push_back(comp);
            }
        }
        if(auto comp = getComponent<CameraComponent>(entity))
        {
            if(comp->valid)
            {
                components.push_back(comp);
            }
        }
        if(auto comp = getComponent<InputComponent>(entity))
        {
            if(comp->valid)
            {
                components.push_back(comp);
            }
        }
        if(auto comp = getComponent<SoundComponent>(entity))
        {
            if(comp->valid)
            {
                components.push_back(comp);
            }
        }
        if(auto comp = getComponent<DirectionalLightComponent>(entity))
        {
            if(comp->valid)
            {
                components.push_back(comp);
            }
        }
        if(auto comp = getComponent<SpotLightComponent>(entity))
        {
            if(comp->valid)
            {
                components.push_back(comp);
            }
        }
        if(auto comp = getComponent<PointLightComponent>(entity))
        {
            if(comp->valid)
            {
                components.push_back(comp);
            }
        }
        if(auto comp = getComponent<ScriptComponent>(entity))
        {
            if(comp->valid)
            {
                components.push_back(comp);
            }
        }
        if(auto comp = getComponent<ColliderComponent>(entity))
        {
            if(comp->valid)
            {
                components.push_back(comp);
            }
        }
        return components;
    }

public:
    // ---------------------------- Helper functions ---------------------------
    void print() {
        std::cout << "transforms: ";
        for (auto comp : mTransformComponents)
            std::cout << "{id: " << comp.entityId << ", valid: " << comp.valid << "} ";
        std::cout << std::endl << "renders: ";
        for (auto comp : mMeshComponents)
            std::cout << "{id: " << comp.entityId << ", valid: " << comp.valid << "} ";
        std::cout << std::endl;
    }

    /** Finds a component for a given entity.
     * Searches from begin to end in a certain component array to find
     * a component with the specified entityID and returns it if found
     * and nullptr otherwise.
     * @param begin - startpos to search from. Inclusive.
     * @param end - endpos to search to. Exclusive.
     * @param eID - entityID for component to find.
     * @return ptr to component if found or nullptr otherwise
     */
    template <typename iterator>
    static typename iterator::value_type* find(const iterator& begin, const iterator& end, unsigned int eID)
    {
        auto result = std::lower_bound(begin, end, eID, [](const typename iterator::value_type& a, const unsigned int& b){
            return a.entityId < b || !a.valid;
        });
        return (result != end && result->entityId == eID) ? &(*result) : nullptr;
    }

    /** Implementation of binary search to use in other functions.
     * This function should be more of a guideline in how to use
     * binary search and usage should just copy this function.
     */
    template <typename iterator, typename comp = std::less<typename iterator::value_type>>
    static iterator binarySearch(const iterator& begin, const iterator& end, const typename iterator::value_type& value, comp compare = comp())
    {
        auto result = std::lower_bound(begin, end, value, compare);
        // If lower_bound found something and it's not lower or higher, then it found it. Return position.
        return (result != end && !(compare(*result, value) || compare(value, *result))) ? result : end;
    }

    /** Breadth first search iterator for iterating through entity children
     * in a tree-like fashion.
     * @brief Parent-child breadth first search iterator
     */
    class TreeIterator
    {
    private:
        struct ChildInfo
        {
            unsigned int eID;
            int parentID{-1};
        } currentChild;
        EntityManager *EM;
        TransformComponent* mCurrent;
        std::queue<ChildInfo> mNext{};
        std::queue<ChildInfo> mNextNext{};
        unsigned int mLevel{0};

        void findChilds()
        {
            for (auto eID{mCurrent->children.begin()}; eID != mCurrent->children.end(); ++eID)
            {
                mNextNext.push({*eID, static_cast<int>(mCurrent->entityId)});
            }
        }

    public:
        TreeIterator(EntityManager* entityManager, unsigned int startId)
            : currentChild{startId}, EM{entityManager}, mCurrent{EM->getComponent<TransformComponent>(startId)}
        {
            if (mCurrent != nullptr)
                findChilds();
        }
        TreeIterator(EntityManager* entityManager, TransformComponent* current)
            : EM{entityManager}, mCurrent{current}
        {

        }

        TreeIterator& operator++()
        {
            if (mNext.empty() && !mNextNext.empty())
            {
                mNext.swap(mNextNext);
                ++mLevel;
            }

            if (mNext.empty())
            {
                mCurrent = nullptr;
            }
            else
            {
                while (!mNext.empty())
                {
                    currentChild = mNext.front();
                    mCurrent = EM->getComponent<TransformComponent>(currentChild.eID);
                    mNext.pop();

                    if (mCurrent != nullptr)
                    {
                        findChilds();
                        break;
                    }
                }
            }
            return *this;
        }
        bool operator!= (const TreeIterator& it) { return mCurrent != it.mCurrent; }
        TransformComponent& operator* () { return *mCurrent; }
        TransformComponent* operator-> () { return mCurrent; }
        unsigned int getLevel() const { return mLevel; }
        int getParentId() const { return currentChild.parentID; }
        TransformComponent* getParent()
        {
            return (currentChild.parentID != -1)
                ? EM->getComponent<TransformComponent>(static_cast<unsigned int>(currentChild.parentID))
                : nullptr;
        }
    };

    // Returns a begin iterator pointing to eID's transform component
    TreeIterator treeBegin(unsigned int eID) { return TreeIterator{this, eID}; }
    // Returns an end iterator pointing to nothing
    TreeIterator treeEnd() { return TreeIterator{this, nullptr}; }



    /** Helper functions to add and set position rotation and scale for transforms.
     * These functions works just like the components themselves, but does take
     * children of entities into account, which component transforms does not.
     * @brief transform setters
     */
    /// Add transform position with inheritance
    void addTransformPos(unsigned int eID, const gsl::vec3& pos);
    /// Add transform rotation with inheritance
    void addTransformRot(unsigned int eID, const gsl::quat& rot);
    /// Add transform scale with inheritance
    void addTransformScale(unsigned int eID, const gsl::vec3& scale);
    /// Set transform position with inheritance
    void setTransformPos(unsigned int eID, const gsl::vec3& pos);
    /// Set transform rotation with inheritance
    void setTransformRot(unsigned int eID, const gsl::quat& rot);
    /// Set transform scale with inheritance
    void setTransformScale(unsigned int eID, const gsl::vec3& scale);

    gsl::vec3 getTransformPos(unsigned int eID);
    gsl::quat getTransformRot(unsigned int eID);
    gsl::vec3 getTransformScale(unsigned int eID);

    // Updates all transformcomponents bounds
    void UpdateBounds();
    // static MeshData::Bounds CalculateBounds(const std::vector<Vertex> &vertices);
};

#endif // COMPONENTMANAGER_H
