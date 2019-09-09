#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include "componentdata.h"
#include <typeinfo>
#include <vector>
#include "resourcemanager.h"
#include <QObject>
#include <QDebug>

/** A component data manager class
 * Constructs entities and manages
 * component data for each entity.
 *
 * @author andesyv, Skau
 */
class EntityManager : public QObject
{
    Q_OBJECT
private:
    // ------------------------------ Member Variables ------------------------------
    // Component arrays. Remember to update componentCount if adding more.
    std::vector<TransformComponent> mTransforms;
    std::vector<MeshComponent> mMeshComponents;
    std::vector<EntityData> mEntityData;
    std::vector<CameraComponent> mCameraComponents;
    std::vector<InputComponent> mInputComponents;

    unsigned int idCounter{0};


signals:
    void updateUI(const std::vector<EntityData>& entityData);

public slots:
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
        if(auto comp = getComponent<InputComponent>(entity))
        {
            if(comp->valid)
            {
                outComponents.push_back(comp);
                addedAnyComponents = true;
            }
        }
        return addedAnyComponents;
    }

    // ------------------------- Member functions ---------------
public:
    EntityManager()
    {

    }

    std::vector<TransformComponent>& getTransforms() { return mTransforms; }
    std::vector<MeshComponent>& getMeshComponents() { return mMeshComponents; }
    std::vector<EntityData>& getEntityData() { return mEntityData; }
    std::vector<CameraComponent>& getCameraComponents() { return mCameraComponents; }
    std::vector<InputComponent>& getInputComponents() { return mInputComponents; }

    void createCube()
    {
        auto id = createEntity();
        addComponent<MeshComponent, TransformComponent>(id);
        auto render = getComponent<MeshComponent>(id);
        if(auto mesh = ResourceManager::instance()->getMesh("box2"))
        {
            render->meshData = *mesh;
            render->isVisible = true;
        }
    }

    void createMonkey()
    {
        auto id = createEntity();
        addComponent<MeshComponent, TransformComponent>(id);
        auto render = getComponent<MeshComponent>(id);
        if(auto mesh = ResourceManager::instance()->getMesh("monkey"))
        {
            render->meshData = *mesh;
            render->isVisible = true;
        }
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
        auto id = idCounter++;
        EntityData entityData;
        entityData.entityId = id;
        if(!name.size())
        {
            name = "Entity" + std::to_string(id);
        }
        entityData.name = name;
        mEntityData.push_back(entityData);
        updateUI(mEntityData);
        return id;
    }

    template<typename... componentTypes>
    std::tuple<componentTypes&...> addComponent(unsigned int entity)
    {
        return {addComponents<componentTypes>(entity)...};
    }

    void addComponent(unsigned int entity, ComponentType type)
    {
        switch (type)
        {
        case ComponentType::Mesh:
        {
            addComponent<MeshComponent>(entity);
            break;
        }
        case ComponentType::Transform:
        {
            addComponent<TransformComponent>(entity);
            break;
        }
        case ComponentType::Input:
        {
            addComponent<InputComponent>(entity);
            break;
        }
        default:
            break;
        }
    }

    template<class T,
             typename std::enable_if<(std::is_same<TransformComponent, T>::value)>::type* = nullptr>
    T* getComponent(unsigned int entity)
    {
        for (auto& comp : mTransforms)
            if (comp.valid && comp.entityId == entity)
                return &comp;

        return nullptr;
    }

    template<class T,
             typename std::enable_if<(std::is_same<MeshComponent, T>::value)>::type* = nullptr>
    T* getComponent(unsigned int entity)
    {
        for (auto& comp : mMeshComponents)
            if (comp.valid && comp.entityId == entity)
                return &comp;

        return nullptr;
    }

    template<class T,
             typename std::enable_if<(std::is_same<CameraComponent, T>::value)>::type* = nullptr>
    T* getComponent(unsigned int entity)
    {
        for (auto& comp : mCameraComponents)
            if (comp.valid && comp.entityId == entity)
                return &comp;

        return nullptr;
    }

    template<class T,
             typename std::enable_if<(std::is_same<InputComponent, T>::value)>::type* = nullptr>
    T* getComponent(unsigned int entity)
    {
        for (auto& comp : mInputComponents)
            if (comp.valid && comp.entityId == entity)
                return &comp;

        return nullptr;
    }

    template<class T,
             typename std::enable_if<(std::is_same<TransformComponent, T>::value)>::type* = nullptr>
    bool removeComponent(unsigned int entity)
    {
        for(auto& comp : mTransforms)
        {
            if(comp.entityId == entity)
            {
                if(comp.valid)
                {
                    comp = TransformComponent();
                    return true;
                }
            }
        }

        return false;
    }

    template<class T,
             typename std::enable_if<(std::is_same<MeshComponent, T>::value)>::type* = nullptr>
    bool removeComponent(unsigned int entity)
    {
        for(auto& comp : mMeshComponents)
        {
            if(comp.entityId == entity)
            {
                if(comp.valid)
                {
                    comp = MeshComponent();
                    return true;
                }
            }
        }

        return false;
    }

    template<class T,
             typename std::enable_if<(std::is_same<InputComponent, T>::value)>::type* = nullptr>
    bool removeComponent(unsigned int entity)
    {
        for(auto& comp : mInputComponents)
        {
            if(comp.entityId == entity)
            {
                if(comp.valid)
                {
                    comp = InputComponent();
                    return true;
                }
            }
        }

        return false;
    }

    void print() {
        std::cout << "transforms: ";
        for (auto comp : mTransforms)
            std::cout << "{id: " << comp.entityId << ", valid: " << comp.valid << "} ";
        std::cout << std::endl << "renders: ";
        for (auto comp : mMeshComponents)
            std::cout << "{id: " << comp.entityId << ", valid: " << comp.valid << "} ";
        std::cout << std::endl;
    }

private:
    template<class T, typename std::enable_if<(std::is_same<TransformComponent, T>::value)>::type* = nullptr>
    T& addComponents(unsigned int entity)
    {
        for (auto& comp : mTransforms)
        {
            if (!comp.valid)
            {
                comp.valid = true;
                if (comp.entityId != entity)
                {
                    comp.entityId = entity;
                    std::sort(mTransforms.begin(), mTransforms.end(),[](const TransformComponent& t1, const TransformComponent& t2)
                    {
                        return t1.entityId < t2.entityId;
                    });
                }
                return comp;
            }
        }

        mTransforms.emplace_back(TransformComponent{entity, true});
        auto &comp = mTransforms.back();
        std::sort(mTransforms.begin(), mTransforms.end(),[](const TransformComponent& t1, const TransformComponent& t2)
        {
            return t1.entityId < t2.entityId;
        });
        return comp;
    }

    template<class T, typename std::enable_if<(std::is_same<MeshComponent, T>::value)>::type* = nullptr>
    T& addComponents(unsigned int entity)
    {
        for (auto& comp : mMeshComponents)
        {
            if (!comp.valid)
            {
                comp.valid = true;
                if (comp.entityId != entity)
                {
                    comp.entityId = entity;
                    std::sort(mMeshComponents.begin(), mMeshComponents.end(),[](const MeshComponent& t1, const MeshComponent& t2)
                    {
                        return t1.entityId < t2.entityId;
                    });
                }
                return comp;
            }
        }

        mMeshComponents.emplace_back(MeshComponent{entity, true});
        auto &comp = mMeshComponents.back();
        std::sort(mMeshComponents.begin(), mMeshComponents.end(),[](const MeshComponent& t1, const MeshComponent& t2)
        {
            return t1.entityId < t2.entityId;
        });
        return comp;
    }

    template<class T, typename std::enable_if<(std::is_same<CameraComponent, T>::value)>::type* = nullptr>
    T& addComponents(unsigned int entity)
    {
        for (auto& comp : mCameraComponents)
        {
            if (!comp.valid)
            {
                comp.valid = true;
                if (comp.entityId != entity)
                {
                    comp.entityId = entity;
                    std::sort(mCameraComponents.begin(), mCameraComponents.end(),[](const CameraComponent& t1, const CameraComponent& t2)
                    {
                        return t1.entityId < t2.entityId;
                    });
                }
                return comp;
            }
        }

        mCameraComponents.emplace_back(CameraComponent{entity, true});
        auto &comp = mCameraComponents.back();
        std::sort(mCameraComponents.begin(), mCameraComponents.end(),[](const CameraComponent& t1, const CameraComponent& t2)
        {
            return t1.entityId < t2.entityId;
        });
        return comp;
    }

    template<class T, typename std::enable_if<(std::is_same<InputComponent, T>::value)>::type* = nullptr>
    T& addComponents(unsigned int entity)
    {
        for (auto& comp : mInputComponents)
        {
            if (!comp.valid)
            {
                comp.valid = true;
                if (comp.entityId != entity)
                {
                    comp.entityId = entity;
                    std::sort(mInputComponents.begin(), mInputComponents.end(),[](const InputComponent& t1, const InputComponent& t2)
                    {
                        return t1.entityId < t2.entityId;
                    });
                }
                return comp;
            }
        }

        mInputComponents.emplace_back(InputComponent{entity, true});
        auto &comp = mInputComponents.back();
        std::sort(mInputComponents.begin(), mInputComponents.end(),[](const InputComponent& t1, const InputComponent& t2)
        {
            return t1.entityId < t2.entityId;
        });
        return comp;
    }

    template <class T>
    void sortComponents(std::vector<T>& vector)
    {
        std::sort(vector.begin(), vector.end(), [](Component t1, Component t2)
        {
            return t1.entityId < t2.entityId;
        });
    }
};

#endif // COMPONENTMANAGER_H
