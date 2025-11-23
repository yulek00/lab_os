#pragma once

#include "exceptions.h"

namespace thread {
    using threadFunc = void* (*)(void*);
    struct threadInfo;

    class Thread {
        private: 
            threadFunc func;
            threadInfo* pimpl;
            bool is_joined = true;

        public:

            explicit Thread(threadFunc func); 
            Thread(const Thread&) = delete;
            Thread& operator=(const Thread&) = delete;
            Thread(Thread&& other) noexcept;
            Thread& operator=(Thread&& other) noexcept;    
            void Run(void* threadData);
            void Join();
            ~Thread() noexcept;

    };

}