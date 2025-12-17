#pragma once
#include "GameMessages.h"
#include "threads.h"
#include <string>
#include <unistd.h>

namespace game_client {

    class GameClient {
    private:
        pid_t myPid;
        std::string clientPipePath;
        thread::Thread reader_thread;
        bool running = true;
        
        bool is_joined = false;
        bool is_playing = false;
        std::string currentGameName;

        void handleServerResponse(const GamePacket& pkt);
        void displayMenu();
        void handleInput();

    public:
        GameClient();
        ~GameClient();
        static void* receive_loop_wrapper(void* context);
        void run();
        void sendPacket(GamePacket& pkt);
    };
}