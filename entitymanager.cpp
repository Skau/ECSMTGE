#include "entitymanager.h"

template <typename T>
unsigned int EntityManager::DataArray<T>::mLength = 0;

EntityManager::EntityManager()
{

}

EntityManager::HorizontalIteratorWrapper EntityManager::loopHorizontal(unsigned int index)
{
    return HorizontalIteratorWrapper{this, index};
}

void EntityManager::resizeArrays(unsigned int newSize)
{
    mTransforms.resize(newSize);
    mRenders.resize(newSize);

    mTransforms.mLength = newSize;
    arrayLength = newSize;
}





// ------------------ HorizonalIterarorWrapper --------------------------
EntityManager::HorizontalIteratorWrapper::HorizontalIteratorWrapper(EntityManager *_compManRef, unsigned int _index)
    : compManRef{_compManRef}, index{_index}
{

}

EntityManager::HorizontalIteratorWrapper::HorizontalIterator EntityManager::HorizontalIteratorWrapper::begin()
{
    return HorizontalIterator{compManRef, index, 0};
}

EntityManager::HorizontalIteratorWrapper::HorizontalIterator EntityManager::HorizontalIteratorWrapper::end()
{
    return HorizontalIterator{compManRef, index, EntityManager::componentCount};
}





// ----------------- HorizontalIterator ------------------------------
EntityManager::HorizontalIteratorWrapper::HorizontalIterator::HorizontalIterator(EntityManager *_compManRef, unsigned int _index, unsigned int _hIndex)
    : compManRef{_compManRef}, index{_index}, hIndex{_hIndex}
{

}

EntityManager::HorizontalIteratorWrapper::HorizontalIterator &EntityManager::HorizontalIteratorWrapper::HorizontalIterator::operator++()
{
    ++hIndex;
    return *this;
}

bool EntityManager::HorizontalIteratorWrapper::HorizontalIterator::operator!=(const EntityManager::HorizontalIteratorWrapper::HorizontalIterator &it)
{
    return !(index == it.index && hIndex == it.hIndex);
}

Component *EntityManager::HorizontalIteratorWrapper::HorizontalIterator::operator*()
{
    switch (hIndex) {
    case 0:
        return &compManRef->mTransforms[index];
    case 1:
        return &compManRef->mRenders[index];
    default:
        return nullptr;
    }
}
