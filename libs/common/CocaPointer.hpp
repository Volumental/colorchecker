#pragma once

#include <memory>

namespace komb {

/**
 * @brief An owning pointer that has value semantics, i.e. it Clones On Copy and Assign.
 *
 * The pointer expects that the type T has defined a clone function.
 */
template<typename T>
class CocaPointer
{
public:
    CocaPointer()
    {}

    explicit CocaPointer(const T& x)
        : pointer(x.clone())
    {}

    CocaPointer(const CocaPointer<T>& x)
        : pointer(x.pointer->clone())
    {}

    CocaPointer<T>& operator=(CocaPointer<T> x)
    {
        std::swap(this->pointer, x.pointer);
        return *this;
    }

    T& operator*()
    {
        return pointer.operator*();
    }

    const T& operator*() const
    {
        return pointer.operator*();
    }

    T* operator->()
    {
        return pointer.operator->();
    }

    const T* operator->() const
    {
        return pointer.operator->();
    }

private:
    std::unique_ptr<T> pointer;
};

} // namespace komb
