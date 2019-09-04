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

class EntityManager
{
private:
    // ------------------------------- Internal Data Types --------------------------
    /**
     * A data wrapper class for objects of type T
     * that uses normal C-style arrays to store
     * data and with some more functionality.
     * @brief dataWrapper class for components
     */
    template <class T>
    class DataArray
    {
        // No need to make the member functions private, as they are only
        // accessible from within ComponentManager
    public:
        // Data pointer to beginning of normal c-style array.
        T* mData = nullptr;
        static unsigned int mLength;

        // internalLength should always be the same as length
        // (only used when resizing)
        unsigned int mInternalLength;

    public:
        DataArray();

        T* get();
        T* operator* ();
        T& operator[] (unsigned int i);
        T& at (unsigned int i);

        class DataArrayIterator
        {
        private:
            unsigned int index;
            DataArray<T> &ref;

        public:
            DataArrayIterator(DataArray<T> &_ref, unsigned int _index);
            DataArrayIterator& operator++();
            bool operator!= (const DataArrayIterator& it);
            T& operator* ();
        };

        DataArrayIterator begin();
        DataArrayIterator end();

        void resize(unsigned int newLength);
    };



    /**
     * A wrapper class for a horizontal iterator
     * To be used by the loopHorizontal function so
     * that you can use a range-based for loop to loop
     * through each row of the components arrays.
     *
     * @brief HorizontalIterator wrapper
     */
    class HorizontalIteratorWrapper {
    private:
        EntityManager* compManRef;
        unsigned int index;

    public:
        HorizontalIteratorWrapper(EntityManager* _compManRef, unsigned int _index);

        class HorizontalIterator
        {
        private:
            EntityManager* compManRef;
            unsigned int index;
            unsigned int hIndex;

        public:
            HorizontalIterator(EntityManager* _compManRef, unsigned int _index, unsigned int _hIndex);
            HorizontalIterator& operator++();
            bool operator!= (const HorizontalIterator& it);
            // Kind of a "look up table"
            Component* operator*();
        };

        HorizontalIterator begin();
        HorizontalIterator end();
    };






private:
    // ------------------------------ Member Variables ------------------------------
    // Amount of component arrays.
    // Compile time static as arrays also are compile time statics.
    static constexpr unsigned int componentCount{2};

    // Component arrays. Remember to update componentCount if adding more.
    // Transform *mTransforms{nullptr};
    // Render *mRenders{nullptr};
    DataArray<Transform> mTransforms;
    DataArray<Render> mRenders;

    // Length of all component arrays.
    unsigned int arrayLength{0};







    // ------------------------- Member functions ---------------
private:
    HorizontalIteratorWrapper loopHorizontal(unsigned int index);
    void resizeArrays(unsigned int newSize);



public:

    EntityManager();

    unsigned int createEntity();

    template <typename T>
    void addComponent(unsigned int entity)
    {
        long int internalIndex{-1}, emptyRow{-1};
        for (unsigned int i{0}; i < arrayLength && internalIndex != -1; ++i)
        {
            bool rowIsEmpty{true};
            for (auto comp : loopHorizontal(i)) {
                if (comp->valid) {
                    if (comp->entityId == entity) {
                        // This component is part of same entity
                        internalIndex = i;
                        // break;
                    }

                    rowIsEmpty = false;
                }
            }
            if (rowIsEmpty)
                emptyRow = i;
        }

        // If internalIndex isn't -1, we have found other components
        // that belong to the same entity
        if (-1 < internalIndex)
        {
            if (typeid (T) == typeid(Transform)) {
                if (mTransforms[internalIndex].valid) {
                    // Adding component on already excisting component
                    /* In the scenario that we try to add a component to an
                     * already excisting component for the same entity, we
                     * should just return out.
                     * All components on a row (should) belongs to the same entity,
                     * so we won't need to check that the component is
                     * of the same entity aswell.
                     */
                    return;
                } else {
                    // Object isn't used, you are free to place a component there
                    mTransforms[internalIndex] = T{};
                }
            } else if (typeid (T) == typeid(Render)) {
                if (mRenders[internalIndex].valid) {
                    // Adding component on already excisting component
                    /* In the scenario that we try to add a component to an
                     * already excisting component for the same entity, we
                     * should just return out.
                     * All components on a row (should) belongs to the same entity,
                     * so we won't need to check that the component is
                     * of the same entity aswell.
                     */
                    return;
                } else {
                    // Object isn't used, you are free to place a component there
                    mRenders[internalIndex] = T{};
                }
            }
        }
        // If internalIndex is -1, we didn't find any other components
        // that belongs to entity. A.k.a., this is a new entity.
        else
        {
            // If no empty rows were found, add a new row and add components to
            // that row
            if (emptyRow < 0)
                resizeArrays(arrayLength + 1);

            if (typeid(T) == typeid(Transform)) {
                mTransforms.at((emptyRow < 0) ? arrayLength : emptyRow) = T{};
            } else if (typeid(T) == typeid(Render)) {
                mRenders.at((emptyRow < 0) ? arrayLength : emptyRow) = T{};
            }
        }
    }
};



// Inline include of entitymanager.inl
#include "entitymanager.inl"

#endif // COMPONENTMANAGER_H



