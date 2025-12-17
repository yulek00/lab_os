#pragma once
#include <string>


class NumberLogic {
public:
    static std::string generateSecret();
    
    static void calculate(const std::string& secret, const std::string& guess, int& bulls, int& cows);
    
    static bool isValidGuess(const char* guess);
};