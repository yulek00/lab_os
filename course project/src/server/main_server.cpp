#include "GameServer.h"
#include <iostream>

int main() {
    game_server::GameServer server;
    try {
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}