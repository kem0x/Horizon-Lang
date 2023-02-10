export module Runtime.Thread;

import <thread>;
import Types.Core;
import Runtime.RuntimeValue;

export
{
    //@note: this is not perfect, as when the main thread exits threads will desync,
    //im not sure if this has critical effects so i will leave this be until there's a reason to fix it.
    struct RuntimeThread : RuntimeValue
    {
        std::thread* Thread;

        template <class Func, class... Args>
        [[nodiscard]] explicit RuntimeThread(Func&& Fn, Args&&... Arg)
            : RuntimeValue(RuntimeValueType::Thread, "Thread")
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