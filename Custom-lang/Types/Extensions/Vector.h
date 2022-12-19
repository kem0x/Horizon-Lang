#pragma once
#include <vector>

namespace VectorExtensions
{
    template <typename T>
    inline auto Shift(Vector<T>& Vector) -> T
    {
        auto Value = Vector[0];
        Vector.erase(Vector.begin());
        return Value;
    }

    template <typename T>
    inline auto Pop(Vector<T>& Vector) -> T
    {
        auto Value = Vector.back();
        Vector.pop_back();
        return Value;
    }
}