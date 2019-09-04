#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include "componentdata.h"
#include <typeinfo>
#include <cassert>

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
        // No need to make the member functions private, as they are only
        // accessible from within ComponentManager
    public:
        // Data pointer to beginning of normal c-style array.
        T* mData = nullptr;
        static unsigned int mLength;

        // internalLength should always be the same as length
        // (only used when resizing)
        unsigned int mInternalLength;

        void resize(unsigned int newLength)
        {
            // Copy content to temp array
            T *temp = new T[mInternalLength];
            for (unsigned int i{0}; i < mInternalLength; ++i)
                temp[i] = mData[i];

            // Make new array
            delete[] mData;
            mData = new T[newLength];

            // Fill array again
            for (unsigned int i{0}; i < newLength; ++i) {
                // Default constructs objects outside of array
                mData[i] = (i < mInternalLength) ? temp[i] : T{};
            }

            // Cleanup
            delete[] temp;
            mInternalLength = newLength;
        }

    public:
        T* get() { return mData; }
        T* operator* () { return mData; }
        T& operator[] (unsigned int i) {return *(mData + i); }
        T& at (unsigned int i) { assert(i < mLength); return *(mData + i); }

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
        DataArrayIterator end() { return DataArrayIterator{*this, mLength}; }
    };



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

    void resizeArrays(unsigned int newSize);

public:
    ComponentManager();

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

#endif // COMPONENTMANAGER_H
