#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include "componentdata.h"
#include <typeinfo>

/** A component data manager class
 * Constructs entities and manages
 * component data for each entity.
 *
 * @author andesyv
 */

class ComponentManager
{
private:
    // Data info for each array type
    template <class T>
    class DataArray
    {
    private:
        T* data = nullptr;
        static unsigned int length;

    public:
        T* get() { return data; }

        // template <class G>
        class DataArrayIterator
        {
        private:
            unsigned int index;
            DataArray<T> &ref;

        public:
            DataArrayIterator(DataArray<T> &_ref, unsigned int _index)
                : ref{_ref}, index{_index} {}
            DataArrayIterator& operator++() { ++index; return *this; }
            bool operator!= (const DataArrayIterator& it) { return index != it.index; }
            T& operator* () { return *(ref.get() + index); }
        };

        DataArrayIterator begin() { return DataArrayIterator{*this, 0}; }
        DataArrayIterator end() { return DataArrayIterator{*this, length}; }
    };



    // Amount of component arrays.
    // Compile time static as arrays also are compile time statics.
    static constexpr unsigned int componentCount{2};

    // Component arrays. Remember to update componentCount if adding more.
    Transform *mTransforms{nullptr};
    Render *mRenders{nullptr};
    // DataArray<Transform> mTransforms;
    // DataArray<Render> mRenders;

    // Length of all component arrays.
    unsigned int arrayLength{0};




    class HorizontalIteratorWrapper {
    private:
        ComponentManager* compManRef;
        unsigned int index;

    public:
        HorizontalIteratorWrapper(ComponentManager* _compManRef, unsigned int _index)
            : compManRef{_compManRef}, index{_index} {}

        class HorizontalIterator
        {
        private:
            ComponentManager* compManRef;
            unsigned int index;
            unsigned int hIndex;

        public:
            HorizontalIterator(ComponentManager* _compManRef, unsigned int _index, unsigned int _hIndex)
                : compManRef{_compManRef}, index{_index}, hIndex{_hIndex} {}
            HorizontalIterator& operator++()
            {
                ++hIndex;
                return *this;
            }
            bool operator!= (const HorizontalIterator& it)
            {
                return !(index == it.index && hIndex == it.hIndex);
            }
            // Kind of a "look up table"
            Component* operator*() {
                switch (hIndex) {
                    case 0:
                        return &compManRef->mTransforms[index];
                    case 1:
                        return &compManRef->mRenders[index];
                    default:
                        return nullptr;
                }
            }
        };

        HorizontalIterator begin() {
            return HorizontalIterator{compManRef, index, 0};
        }

        HorizontalIterator end() {
            return HorizontalIterator{compManRef, index, ComponentManager::componentCount};
        }
    };




    HorizontalIteratorWrapper loopHorizontal(unsigned int index) {
        return HorizontalIteratorWrapper{this, index};
    }

    void resizeArray(unsigned int newSize) {
        Transform *temp = new Transform[arrayLength];
        for (unsigned int i{0}; i < arrayLength; ++i)
            temp[i] = mTransforms[i];

        delete[] mTransforms;
        mTransforms = new Transform[newSize];

        for (unsigned int i{0}; i < arrayLength; ++i)
            temp[i] = mTransforms[i];

        arrayLength = newSize;
    }
public:
    ComponentManager();

    unsigned int createEntity();
    template <typename T>
    void addComponent(unsigned int entity)
    {
        long int internalIndex{-1};
        for (unsigned int i{0}; i < arrayLength && internalIndex != -1; ++i)
        {
            for (auto comp : loopHorizontal(i)) {
                if (comp->valid && comp->entityId == entity) {
                    // This component is part of same entity
                    internalIndex = i;
                    break;
                }
            }
        }

        // If internalIndex isn't -1, we have found other components
        // that belong to the same entity
        if (-1 < internalIndex)
        {

        }
        // If internalIndex is -1, we didn't find any other components
        // that belongs to entity. A.k.a., this is a new entity.
        else
        {
            if (typeid(T) == typeid(Transform)) {

            } else if (typeid(T) == typeid(Render)) {

            }
        }
    }
};

#endif // COMPONENTMANAGER_H
