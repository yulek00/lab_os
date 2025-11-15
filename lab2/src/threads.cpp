#include "threads.h"
#include <iostream>  
#include <utility>
#include <pthread.h>

namespace thread { 
        struct threadInfo {
            pthread_t thread = 0; 
        };

        Thread::Thread(threadFunc func) : func(func), is_joined(true) {
            pimpl = new threadInfo();
        }

        Thread::Thread(Thread&& other) noexcept 
        : func(other.func), pimpl(other.pimpl), is_joined(other.is_joined) {
            other.pimpl = nullptr;
            other.is_joined = true; 
        }

        Thread& Thread::operator=(Thread&& other) noexcept {
            if (pimpl != nullptr && !is_joined) {
                pthread_detach(pimpl->thread);
            }
            delete pimpl;
            
            Thread temp = std::move(other);
            std::swap(func, temp.func);
            std::swap(pimpl, temp.pimpl);
            std::swap(is_joined, temp.is_joined);
            
            return *this;
        }
        
        void Thread::Run(void* threadData) {
            if (pimpl == nullptr) {
                throw exceptions::ThreadException("Thread is not initialized.");
            }

            if (!is_joined) { 
                throw exceptions::ThreadException("Thread is already running.");
            }

            int result = pthread_create(
                &(pimpl->thread), 
                nullptr,      
                func,         
                threadData    
            );

            if (result != 0) {
                pimpl->thread = 0;
                throw exceptions::ThreadException("Failed to create thread");
            }
            is_joined = false;
        }

        void Thread::Join() {
            if (pimpl == nullptr || is_joined) {
                return;
            }

            int result = pthread_join(pimpl->thread, nullptr); 

            if (result != 0) {
                throw exceptions::ThreadException("Failed to join thread");
            }
            
            pimpl->thread = 0; 
            is_joined = true;
        }

        Thread::~Thread() noexcept {
            if (pimpl != nullptr) {
                if (!is_joined) {
                    std::cerr << "Warning: Thread resource leaked (not joined). Detaching..." << std::endl;
                    pthread_detach(pimpl->thread); 
                }
                delete pimpl; 
            }
        }
}