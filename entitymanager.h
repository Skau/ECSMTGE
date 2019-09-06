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
            createCube();
            break;
        }
        case 1:
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
            outComponents.push_back(comp);
            addedAnyComponents = true;
        }
        if(auto comp = getComponent<Render>(entity))
        {
            outComponents.push_back(comp);
            addedAnyComponents = true;
        }
        return addedAnyComponents;
    }

    // ------------------------- Member functions ---------------
public:
    EntityManager()
    {

    }

    std::vector<Transform> getTransforms() { return mTransforms; }
    std::vector<Render> getRenders() { return mRenders; }
    std::vector<EntityData> getEntityData() { return mEntityData; }
    std::vector<Camera> getCameras() { return mCameras; }

    void createCube()
    {
        auto id = createEntity();
        addComponent<Render, Transform>(id);
        auto render = getComponent<Render>(id);
        if(auto mesh = ResourceManager::instance()->getMesh("box"))
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
    void addComponent(unsigned int entity)
    {
        std::tuple<componentTypes...> l = {addComponents<componentTypes>(entity)...};
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
    template<class T,
             typename std::enable_if<(std::is_same<Transform, T>::value)>::type* = nullptr>
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

    template<class T,
             typename std::enable_if<(std::is_same<Render, T>::value)>::type* = nullptr>
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

    template<class T,
             typename std::enable_if<(std::is_same<Camera, T>::value)>::type* = nullptr>
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
};

#endif // COMPONENTMANAGER_H
