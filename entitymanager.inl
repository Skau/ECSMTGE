#include <cassert>

#include "entitymanager.h"

// -.---------------------------- DataArray -------------------------------------
template<class T>
EntityManager::DataArray<T>::DataArray()
{

}

template<class T>
T *EntityManager::DataArray<T>::get()
{
    return mData;
}

template<class T>
T *EntityManager::DataArray<T>::operator*()
{
    return mData;
}

template<class T>
T &EntityManager::DataArray<T>::operator[](unsigned int i)
{
    return *(mData + i);
}

template<class T>
T &EntityManager::DataArray<T>::at(unsigned int i)
{
    assert(i < mLength);
    return *(mData + i);
}

template<class T>
typename EntityManager::DataArray<T>::DataArrayIterator EntityManager::DataArray<T>::begin() { return DataArrayIterator{*this, 0}; }

template<class T>
typename EntityManager::DataArray<T>::DataArrayIterator EntityManager::DataArray<T>::end() { return DataArrayIterator{*this, mLength}; }

template<class T>
void EntityManager::DataArray<T>::resize(unsigned int newLength)
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

    mLength = newLength;

    // Cleanup
    delete[] temp;
    mInternalLength = newLength;
}

// ------------------------------------ DataArray::DataArrayIterator --------------------------------------------
template<class T>
EntityManager::DataArray<T>::DataArrayIterator::DataArrayIterator(DataArray<T> &_ref, unsigned int _index)
    : ref{_ref}, index{_index}
{

}

template<class T>
typename EntityManager::DataArray<T>::DataArrayIterator &EntityManager::DataArray<T>::DataArrayIterator::operator++()
{
    ++index;
    return *this;
}

template<class T>
bool EntityManager::DataArray<T>::DataArrayIterator::operator!=(const DataArrayIterator &it)
{
    return index != it.index;
}

template<class T>
T &EntityManager::DataArray<T>::DataArrayIterator::operator*()
{
    return *(ref.get() + index);
}
