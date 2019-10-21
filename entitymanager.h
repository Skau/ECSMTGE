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
    ADDCOMPONENTS(K) \


#define GETCOMPONENT(K) \
template<class T, \
    typename std::enable_if<(std::is_same<K, T>::value)>::type* = nullptr> \
T* getComponent(unsigned int entity) \
    { \
    for (auto& comp : CONCATENATE(m, K, s)) \
        if (comp.valid && comp.entityId == entity) \
            return &comp; \
        return nullptr; \
    } \

#define REMOVECOMPONENT(K) \
template<class T, \
         typename std::enable_if<(std::is_same<K, T>::value)>::type* = nullptr> \
bool removeComponent(unsigned int entity) \
{ \
    for(auto& comp : CONCATENATE(m, K, s)) \
    { \
        if(comp.entityId == entity) \
        { \
            if(comp.valid) \
            { \
                comp = K{}; \
                return true; \
            } \
        } \
    } \
    return false; \
} \

#define ADDCOMPONENTS(K) \
    template<class T, typename std::enable_if<(std::is_same<K, T>::value)>::type* = nullptr> \
    T& addComponents(unsigned int entity) \
    { \
        for (auto& comp : CONCATENATE(m, K, s)) \
        { \
            if (!comp.valid) \
            { \
                comp.valid = true; \
                comp.entityId = entity; \
                std::sort(CONCATENATE(m, K, s).begin(), CONCATENATE(m, K, s).end(),[](const K& t1, const K& t2) \
                { \
                    return t1.entityId < t2.entityId; \
                }); \
                return comp; \
            } \
        } \
        CONCATENATE(m, K, s).emplace_back(K{entity, true}); \
        auto &comp = CONCATENATE(m, K, s).back(); \
        std::sort(CONCATENATE(m, K, s).begin(), CONCATENATE(m, K, s).end(),[](const K& t1, const K& t2) \
        { \
            return t1.entityId < t2.entityId; \
        }); \
        return comp; \
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
    REGISTER(EntityInfo)

private:
    // ------------------------------ Member Variables -----------------------------
    unsigned int idCounter{0};

signals:
    void updateUI(const std::vector<EntityInfo>& entityData);
    void refreshWidgets();

    // ------------------------- Member functions ---------------
public:
    EntityManager()
    {

    }

    void clear()
    {
        for(auto info : mEntityInfos)
        {
            std::vector<Component*> components;
            getAllComponents(info.entityId, components);
            for(auto& comp : components)
            {
                comp->valid = false;
                comp->reset();
            }
        }

        idCounter = 0;
        mEntityInfos.clear();
//        mTransformComponents.clear();
//        mMeshComponents.clear();
//        mPhysicsComponents.clear();
//        mCameraComponents.clear();
//        mInputComponents.clear();
//        mSoundComponents.clear();
//        mDirectionalLightComponents.clear();
//        mSpotLightComponents.clear();
//        mPointLightComponents.clear();
//        mScriptComponents.clear();
//        mColliderComponents.cleear();
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

    unsigned createCube()
    {
        auto id = createEntity();
        addComponent<MeshComponent, TransformComponent>(id);
        auto render = getComponent<MeshComponent>(id);
        if(auto mesh = ResourceManager::instance()->getMesh("box2"))
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
        if(auto mesh = ResourceManager::instance()->getMesh("suzanne"))
        {
            render->meshData = *mesh;
            render->isVisible = true;
        }
        return id;
    }

    /**
     * @brief Updates the mesh on the given entity
     * @param Entity ID
     * @param Name of the mesh
     */
    void setMesh(unsigned int entity, const std::string& meshName)
    {
        auto render = getComponent<MeshComponent>(entity);
        if(render)
        {
            auto mesh = ResourceManager::instance()->getMesh(meshName);
            if(mesh)
            {
                render->meshData = *mesh;
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

    template<typename... componentTypes>
    std::tuple<componentTypes&...> addComponent(unsigned int entity)
    {
        return {addComponents<componentTypes>(entity)...};
    }

    template<typename... componentTypes>
    std::tuple<componentTypes*...> getComponents(unsigned int entity)
    {
        return {getComponent<componentTypes>(entity)...};
    }

    Component* addComponent(unsigned int entity, ComponentType type)
    {
        Component* returnComp;
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
        default:
            break;
        }

        return returnComp;
    }

    /**
     * @brief Adds all components for a given entity to the given vector. Returns true if any was found.
     * @param The entity ID.
     * @param The vector that will be filled with found components.
     * @return If any components were found and added to the vector.
     */
    bool getAllComponents(unsigned int entity, std::vector<Component*>& outComponents)
    {
        bool addedAnyComponents = false;
        if(auto comp = getComponent<TransformComponent>(entity))
        {
            if(comp->valid)
            {
                outComponents.push_back(comp);
                addedAnyComponents = true;
            }
        }
        if(auto comp = getComponent<MeshComponent>(entity))
        {
            if(comp->valid)
            {
                outComponents.push_back(comp);
                addedAnyComponents = true;
            }
        }
        if(auto comp = getComponent<PhysicsComponent>(entity))
        {
            if(comp->valid)
            {
                outComponents.push_back(comp);
                addedAnyComponents = true;
            }
        }
        if(auto comp = getComponent<InputComponent>(entity))
        {
            if(comp->valid)
            {
                outComponents.push_back(comp);
                addedAnyComponents = true;
            }
        }
        if(auto comp = getComponent<SoundComponent>(entity))
        {
            if(comp->valid)
            {
                outComponents.push_back(comp);
                addedAnyComponents = true;
            }
        }
        if(auto comp = getComponent<DirectionalLightComponent>(entity))
        {
            if(comp->valid)
            {
                outComponents.push_back(comp);
                addedAnyComponents = true;
            }
        }
        if(auto comp = getComponent<SpotLightComponent>(entity))
        {
            if(comp->valid)
            {
                outComponents.push_back(comp);
                addedAnyComponents = true;
            }
        }
        if(auto comp = getComponent<PointLightComponent>(entity))
        {
            if(comp->valid)
            {
                outComponents.push_back(comp);
                addedAnyComponents = true;
            }
        }
        if(auto comp = getComponent<ScriptComponent>(entity))
        {
            if(comp->valid)
            {
                outComponents.push_back(comp);
                addedAnyComponents = true;
            }
        }
        if(auto comp = getComponent<ColliderComponent>(entity))
        {
            if(comp->valid)
            {
                outComponents.push_back(comp);
                addedAnyComponents = true;
            }
        }
        return addedAnyComponents;
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

    // TODO: Find should use binary search
    template <typename iterator>
    static typename iterator::value_type* find(const iterator& begin, const iterator& end, unsigned int eID)
    {
        for (auto it{begin}; it != end; ++it)
            if (it->entityId == eID)
                return &(*it);

        return nullptr;
    }

    /** Breadth first search iterator for iterating through
     * a tree.
     * @brief Breadth first search iterator
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

    void addTransformPos(unsigned int eID, const gsl::vec3& pos);
    void addTransformRot(unsigned int eID, const gsl::quat& rot);
    void addTransformScale(unsigned int eID, const gsl::vec3& scale);
    void setTransformPos(unsigned int eID, const gsl::vec3& pos);
    void setTransformRot(unsigned int eID, const gsl::quat& rot);
    void setTransformScale(unsigned int eID, const gsl::vec3& scale);

    gsl::vec3 getTransformPos(unsigned int eID);
    gsl::quat getTransformRot(unsigned int eID);
    gsl::vec3 getTransformScale(unsigned int eID);

    // Updates all transformcomponents bounds
    void UpdateBounds();
    // static MeshData::Bounds CalculateBounds(const std::vector<Vertex> &vertices);
};

#endif // COMPONENTMANAGER_H
