#ifndef __PARALLEL_DEVICE_LAMBDA_AS_TASK_H__
#define __PARALLEL_DEVICE_LAMBDA_AS_TASK_H__

#include <stdint.h>
#include <stdlib.h>
#include <interface/parallel_device/parallel_task_interface.h>

namespace parallel_device
{
    template <class Lambda>
    class LambdaTask: public virtual ParallelTaskInterface
    {
        private:

            Lambda lambda;
        
        public:

            inline LambdaTask(Lambda lambda): lambda(std::move(lambda)){}

            inline void execute() noexcept
            {
                static_assert(noexcept(lambda()));

                lambda();
            }
    };

    template <class Lambda>
    inline auto lambda_as_task(Lambda lambda) -> std::unique_ptr<ParallelTaskInterface>
    {
        return std::make_unique<LambdaTask<Lambda>>(std::move(lambda));
    }
}

#endif