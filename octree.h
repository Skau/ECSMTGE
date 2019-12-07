#ifndef OCTREE_H
#define OCTREE_H

#include "innpch.h"
#include <memory>

namespace gsl {
/** Template class for a custom oct-tree implementation
 * designed to be used by a scenegraph.
 * @brief Template class for a custom oct-tree implementation
 * designed to be used by a scenegraph.
 */
template <typename T>
class Octree
{
    friend class PhysicsSystem;
public:
    Octree(const T& value, gsl::ivec3 key = {0, 0, 0})
    : mKey{key}, mValue{value}
    {

    }

    gsl::ivec3 key() const { return mKey; }
    // Amount of nodes in branch, including this node
    unsigned int size() const { return mBranchSize; }

    // Searches for node with specified key. Returns a pair containing the node (if found) and the parent.
    Octree<T>* find(gsl::ivec3 pos)
    {
        if (pos == mKey)
            return this;
        // if pos < mKey then place it to the left of the current node. Right otherwise.
        gsl::ivec3 index{!(pos.x < mKey.x), !(pos.y < mKey.y), !(pos.z < mKey.z)};

        auto& nodePtr = mChild[index.x][index.y][index.z];
        if (nodePtr)
            return nodePtr->find(pos);
        else
            return nullptr;
    }

    Octree<T>* insert(ivec3 key, const T& value)
    {
        if (key == mKey)
            return nullptr;

        // if pos < mKey then place it to the left of the current node. Right otherwise.
        ivec3 index{!(key.x < mKey.x), !(key.y < mKey.y), !(key.z < mKey.z)};

        std::unique_ptr<Octree<T>>& nodePtr = mChild[index.x][index.y][index.z];
        if (nodePtr)
        {
            auto result = nodePtr->insert(key, value);
            if (result)
                ++mBranchSize;
            return result;
        }
        else
        {
            nodePtr = std::make_unique<Octree<T>>(value, key);
            return nodePtr.get();
        }
    }

    // returns a list of pointers to all nodes in preorder depth first order
    std::vector<Octree<T>*> preIt()
    {
        std::vector<Octree<T>*> list{};
        list.reserve(mBranchSize);

        preItAdd(list);

        return list;
    }

    std::vector<Octree<T>*> roots()
    {
        std::vector<Octree<T>*> list{};
        list.reserve(mBranchSize - 1);

        rootsAdd(list);

        return list;
    }

    void updateCount()
    {
        unsigned int newCount{1};
        for (int z{0}; z < 2; ++z)
            for (int y{0}; y < 2; ++y)
                for (int x{0}; x < 2; ++x)
                {
                    auto& node = mChild[x][y][z];
                    if (node)
                        newCount += node->size();
                }

        mBranchSize = newCount;
    }

    T mValue;

protected:
    std::unique_ptr<Octree<T>>& findPtr(gsl::ivec3 pos)
    {
        // if pos < mKey then place it to the left of the current node. Right otherwise.
        gsl::ivec3 index{!(pos.x < mKey.x), !(pos.y < mKey.y), !(pos.z < mKey.z)};

        auto& nodePtr = mChild[index.x][index.y][index.z];
        if (nodePtr)
            if (pos == nodePtr->mKey)
                return nodePtr;
            else
                return nodePtr->findPtr(pos);
        else
            return nodePtr;
    }

    void preItAdd(std::vector<Octree<T>*>& t)
    {
        // First everyone to the left, and then everyone to the right.
        for (int z{0}; z < 2; ++z)
        {
            for (int y{0}; y < 2; ++y)
            {
                for (int x{0}; x < 2; ++x)
                {
                    auto& node = mChild[x][y][z];
                    if (node)
                        node->preItAdd(t);
                }
            }

            if (z == 0)
                t.push_back(this);
        }
    }

    void rootsAdd(std::vector<Octree<T>*>& t)
    {
        bool wentDeeper = false;
        // First everyone to the left, and then everyone to the right.
        for (int z{0}; z < 2; ++z)
        {
            for (int y{0}; y < 2; ++y)
            {
                for (int x{0}; x < 2; ++x)
                {
                    auto& node = mChild[x][y][z];
                    if (node)
                    {
                        wentDeeper = true;
                        node->preItAdd(t);
                    }
                }
            }
        }

        if (!wentDeeper)
            t.push_back(this);
    }

    // Left and right of x, y and z
    gsl::ivec3 mKey{};
    std::unique_ptr<Octree<T>> mChild[2][2][2];
    unsigned int mBranchSize{1};
};

}

#endif
