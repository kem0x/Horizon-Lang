export module Extensions.Vector;

import Types.Core;

export
{
    namespace VectorExtensions
    {
        template <typename T>
        inline T Shift(Vector<T>& Vector)
        {
            auto Value = Vector[0];
            Vector.erase(Vector.begin());
            return Value;
        }

        template <typename T>
        inline T Pop(Vector<T>& Vector)
        {
            auto Value = Vector.back();
            Vector.pop_back();
            return Value;
        }
    }
}
