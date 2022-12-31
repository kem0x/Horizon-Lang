export module Runtime.Thread;

import <thread>;
import Types.Core;
import Runtime.RuntimeValue;

export
{
    struct RuntimeThread : RuntimeValue
    {
        std::thread* Thread;

        template <class Func, class... Args>
        [[nodiscard]] explicit RuntimeThread(Func&& Fn, Args&&... Arg)
            : RuntimeValue(RuntimeValueType::Thread)
            , Thread(new std::thread(std::forward<Func>(Fn), std::forward<Args>(Arg)...))
        {
        }

        ~RuntimeThread()
        {
            if (Thread != nullptr)
            {
                Thread->join();
                delete Thread;
            }
        }
    };
}