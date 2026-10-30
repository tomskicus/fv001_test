#ifndef __EXCEPTION_THREAD_SAFE_EXCEPTION_H__
#define __EXCEPTION_THREAD_SAFE_EXCEPTION_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <exception>
#include <mutex>

namespace exception
{
    class ThreadSafeExceptionHolder
    {
        private:

            std::unique_ptr<std::mutex> mtx;
            std::exception_ptr ex;
        
        public:

            inline ThreadSafeExceptionHolder(): mtx(std::make_unique<std::mutex>()),
                                                ex(nullptr){}

            inline auto on_exception(std::exception_ptr ex)
            {
                std::lock_guard<std::mutex> lck_grd(*this->mtx);
                
                this->ex = ex;
            }

            inline void throw_ex()
            {
                std::lock_guard<std::mutex> lck_grd(*this->mtx);
                
                if (this->ex != nullptr)
                {
                    std::rethrow_exception(this->ex);
                }
            }
    };
}

#endif