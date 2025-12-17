#pragma once
#include <stdexcept>
#include <string>

namespace exceptions {
    class ThreadException : public std::runtime_error {
    public:
        ThreadException(const std::string& message) : std::runtime_error(message) {}
    };

    class PipeException : public std::runtime_error {
    public:
        PipeException(const std::string& message) : std::runtime_error(message) {}
    };
    
}