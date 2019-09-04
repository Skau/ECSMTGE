#include "componentmanager.h"

template <typename T>
unsigned int ComponentManager::DataArray<T>::mLength = 0;

ComponentManager::ComponentManager()
{

}

void ComponentManager::resizeArrays(unsigned int newSize) {
    mTransforms.resize(newSize);
    mRenders.resize(newSize);

    mTransforms.mLength = newSize;
    arrayLength = newSize;
}
