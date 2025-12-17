#pragma once
#include <sys/types.h>
#include "PosixPipe.h"

#define SERVER_FIFO "/tmp/bc_srv_fifo"
#define CLIENT_FIFO_FMT "/tmp/bc_cli_%d"

enum OpCode {
    OP_CREATE_GAME = 1,
    OP_JOIN_BY_NAME,
    OP_FIND_GAME,
    OP_GUESS,
    OP_EXIT,
    OP_MSG_TEXT,
    OP_GAME_START,
    OP_GUESS_RESULT,
    OP_GAME_WIN,
    OP_ERROR
};

struct GamePacket {
    int op_code;
    pid_t sender_pid;
    char game_name[64];
    int players_count;
    char guess[5];
    char text_payload[256];
    int bulls;
    int cows;
};

class GameMessages {
public:
    static void sendPacket(const GamePacket& pkt, const char* fifo_path);
    static void sendError(pid_t recipient_pid, const char* message);
    static void sendText(pid_t recipient_pid, const char* message);
    static void sendGuessResult(pid_t recipient_pid, const char* guess, int bulls, int cows);
    static std::string getClientPipePath(pid_t pid);
};