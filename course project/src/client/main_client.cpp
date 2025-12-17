#include "GameClient.h"
#include <iostream>

int main() {
    try {
        game_client::GameClient client;
        client.run();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}