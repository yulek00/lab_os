#pragma once
#include "GameMessages.h"
#include <string>
#include <vector>
#include <pthread.h>

namespace game_server {

    struct Player {
        pid_t pid;
    };

    struct GameSession {
        std::string name;
        std::string secret_number;
        int max_players;
        std::vector<Player> players;
        bool is_started = false;
        bool is_finished = false;
        size_t current_turn_index = 0;
    };

    class GameServer {
    private:
        std::vector<GameSession> games;
        pthread_mutex_t games_mutex;

        GameSession* findGameByName(const std::string& name);
        GameSession* findGameByPlayerPid(pid_t pid);
        GameSession* findOpenGame(int max_players);

    public:
        GameServer();
        ~GameServer();
        void handleRequest(GamePacket& req);
        void broadcast(GameSession& game, GamePacket& pkt);
        void run();
    };
}