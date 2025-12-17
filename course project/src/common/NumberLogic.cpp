#include "NumberLogic.h"
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <cstring>
#include <cctype>
#include <stdexcept>

std::string NumberLogic::generateSecret() {
    std::string digits = "0123456789";
    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::shuffle(digits.begin(), digits.end(), rng);
    return digits.substr(0, 4);
}

void NumberLogic::calculate(const std::string& secret, const std::string& guess, int& bulls, int& cows) {
    bulls = 0;
    cows = 0;
    if (secret.length() != 4 || guess.length() != 4) {
        throw std::invalid_argument("Secret and guess must be 4 digits long.");
    }

    for (int i = 0; i < 4; ++i) {
        if (guess[i] == secret[i]) {
            bulls++;
        } else if (secret.find(guess[i]) != std::string::npos) {
            cows++;
        }
    }
}

bool NumberLogic::isValidGuess(const char* guess) {
    if (strlen(guess) != 4) return false;
    for (int i = 0; i < 4; ++i) {
        if (!isdigit(guess[i])) return false;
        for (int j = i + 1; j < 4; ++j) {
            if (guess[i] == guess[j]) return false;
        }
    }
    return true;
}