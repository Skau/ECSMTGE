#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include "componentdata.h"
#include <typeinfo>
#include <vector>
#include <QDebug>

/** A component data manager class
 * Constructs entities and manages
 * component data for each entity.
 *
 * @author andesyv, Skau
 */
class EntityManager
{
private:
    // ------------------------------ Member Variables ------------------------------
    // Component arrays. Remember to update componentCount if adding more.
    std::vector<Transform> mTransforms;
    std::vector<Render> mRenders;

    unsigned int idCounter{0};




    // ------------------------- Member functions ---------------
public:
    EntityManager()
    {

    }

    std::vector<Transform> getTransforms() { return mTransforms; }
    std::vector<Render> getRenders() { return mRenders; }

    unsigned int createEntity() { return idCounter++; }

    template<typename... componentTypes>
    void addComponent(unsigned int entity)
    {
        std::tuple<componentTypes...> l = {addComponents<componentTypes>(entity)...};
    }

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

        // TODO: Make constructor
        mTransforms.emplace_back(Transform{});
        mTransforms.back().entityId = entity;
        mTransforms.back().valid = true;
        return mTransforms.back();
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

        // TODO: Make constructor
        mRenders.emplace_back(Render{});
        mRenders.back().entityId = entity;
        mRenders.back().valid = true;
        return mRenders.back();
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

};

#endif // COMPONENTMANAGER_H
