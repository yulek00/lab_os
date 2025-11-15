#pragma once

#include <exception>
#include <string>

namespace exceptions {
    class ThreadException: public std::exception {
    public:
        explicit ThreadException(const std::string& text): error_message_(text) {}
        const char* what() const noexcept override {
            return error_message_.c_str();
        }
    private:
        std::string error_message_;
    };
    
    class ArgumentException: public std::exception {
    public:
        explicit ArgumentException(const std::string& text): error_message_(text) {}
        const char* what() const noexcept override {
            return error_message_.c_str();
        }
    private:
        std::string error_message_;
    };
}