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
    std::vector<Transform> mTransforms;
    std::vector<Render> mRenders;
    std::vector<EntityData> mEntityData;
    std::vector<Camera> mCameras;

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
        if(auto comp = getComponent<Transform>(entity))
        {
            if(comp->valid)
            {
                outComponents.push_back(comp);
                addedAnyComponents = true;
            }
        }
        if(auto comp = getComponent<Render>(entity))
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

    std::vector<Transform>& getTransforms() { return mTransforms; }
    std::vector<Render> getRenders() { return mRenders; }
    std::vector<EntityData> getEntityData() { return mEntityData; }
    std::vector<Camera>& getCameras() { return mCameras; }

    void createCube()
    {
        auto id = createEntity();
        addComponent<Render, Transform>(id);
        auto render = getComponent<Render>(id);
        if(auto mesh = ResourceManager::instance()->getMesh("box2"))
        {
            render->meshData = *mesh;
            render->isVisible = true;
        }
    }

    void createMonkey()
    {
        auto id = createEntity();
        addComponent<Render, Transform>(id);
        auto render = getComponent<Render>(id);
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
        auto render = getComponent<Render>(entity);
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
        case ComponentType::Render:
        {
            addComponent<Render>(entity);
            break;
        }
        case ComponentType::Transform:
        {
            addComponent<Transform>(entity);
            break;
        }
        default:
            break;
        }
    }

    template<class T,
             typename std::enable_if<(std::is_same<Transform, T>::value)>::type* = nullptr>
    T* getComponent(unsigned int entity)
    {
        for (auto& comp : mTransforms)
            if (comp.valid && comp.entityId == entity)
                return &comp;

        return nullptr;
    }

    template<class T,
             typename std::enable_if<(std::is_same<Render, T>::value)>::type* = nullptr>
    T* getComponent(unsigned int entity)
    {
        for (auto& comp : mRenders)
            if (comp.valid && comp.entityId == entity)
                return &comp;

        return nullptr;
    }

    template<class T,
             typename std::enable_if<(std::is_same<Camera, T>::value)>::type* = nullptr>
    T* getComponent(unsigned int entity)
    {
        for (auto& comp : mCameras)
            if (comp.valid && comp.entityId == entity)
                return &comp;

        return nullptr;
    }

    template<class T,
             typename std::enable_if<(std::is_same<Transform, T>::value)>::type* = nullptr>
    bool removeComponent(unsigned int entity)
    {
        for(auto& comp : mTransforms)
        {
            if(comp.entityId == entity)
            {
                if(comp.valid)
                {
                    comp = Transform();
                    return true;
                }
            }
        }

        return false;
    }

    template<class T,
             typename std::enable_if<(std::is_same<Render, T>::value)>::type* = nullptr>
    bool removeComponent(unsigned int entity)
    {
        for(auto& comp : mRenders)
        {
            if(comp.entityId == entity)
            {
                if(comp.valid)
                {
                    comp = Render();
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
        for (auto comp : mRenders)
            std::cout << "{id: " << comp.entityId << ", valid: " << comp.valid << "} ";
        std::cout << std::endl;
    }

private:
    template<class T, typename std::enable_if<(std::is_same<Transform, T>::value)>::type* = nullptr>
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
                    std::sort(mTransforms.begin(), mTransforms.end(),[](const Transform& t1, const Transform& t2)
                    {
                        return t1.entityId < t2.entityId;
                    });
                }
                return comp;
            }
        }

        mTransforms.emplace_back(Transform{entity, true});
        auto &comp = mTransforms.back();
        std::sort(mTransforms.begin(), mTransforms.end(),[](const Transform& t1, const Transform& t2)
        {
            return t1.entityId < t2.entityId;
        });
        return comp;
    }

    template<class T, typename std::enable_if<(std::is_same<Render, T>::value)>::type* = nullptr>
    T& addComponents(unsigned int entity)
    {
        for (auto& comp : mRenders)
        {
            if (!comp.valid)
            {
                comp.valid = true;
                if (comp.entityId != entity)
                {
                    comp.entityId = entity;
                    std::sort(mRenders.begin(), mRenders.end(),[](const Render& t1, const Render& t2)
                    {
                        return t1.entityId < t2.entityId;
                    });
                }
                return comp;
            }
        }

        mRenders.emplace_back(Render{entity, true});
        auto &comp = mRenders.back();
        std::sort(mRenders.begin(), mRenders.end(),[](const Render& t1, const Render& t2)
        {
            return t1.entityId < t2.entityId;
        });
        return comp;
    }

    template<class T, typename std::enable_if<(std::is_same<Camera, T>::value)>::type* = nullptr>
    T& addComponents(unsigned int entity)
    {
        for (auto& comp : mCameras)
        {
            if (!comp.valid)
            {
                comp.valid = true;
                if (comp.entityId != entity)
                {
                    comp.entityId = entity;
                    //sortComponents(mCameras);
                    std::sort(mCameras.begin(), mCameras.end(),[](const Camera& t1, const Camera& t2)
                    {
                        return t1.entityId < t2.entityId;
                    });
                }
                return comp;
            }
        }

        mCameras.emplace_back(Camera{entity, true});
        auto &comp = mCameras.back();
        std::sort(mCameras.begin(), mCameras.end(),[](const Camera& t1, const Camera& t2)
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
