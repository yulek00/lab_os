#include "GameMessages.h"
#include <cstdio>
#include <cstring>
#include <iostream>

std::string GameMessages::getClientPipePath(pid_t pid) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), CLIENT_FIFO_FMT, pid);
    return std::string(buffer);
}

void GameMessages::sendPacket(const GamePacket& pkt, const char* fifo_path) {
    POSIXPipe pipe(fifo_path);
    
    if (pipe.openWrite()) {
        pipe.send(&pkt, sizeof(GamePacket));
        pipe.closePipe(); 
    }
}

void GameMessages::sendError(pid_t recipient_pid, const char* message) {
    GamePacket pkt;
    memset(&pkt, 0, sizeof(pkt)); 
    pkt.op_code = OP_ERROR; 
    pkt.sender_pid = 0;   
    strncpy(pkt.text_payload, message, sizeof(pkt.text_payload) - 1);
    sendPacket(pkt, getClientPipePath(recipient_pid).c_str());
}

void GameMessages::sendText(pid_t recipient_pid, const char* message) {
    GamePacket pkt;
    memset(&pkt, 0, sizeof(pkt));
    pkt.op_code = OP_MSG_TEXT;
    pkt.sender_pid = 0;
    strncpy(pkt.text_payload, message, sizeof(pkt.text_payload) - 1);
    sendPacket(pkt, getClientPipePath(recipient_pid).c_str());
}

void GameMessages::sendGuessResult(pid_t recipient_pid, const char* guess, int bulls, int cows) {
    GamePacket pkt;
    memset(&pkt, 0, sizeof(pkt));
    pkt.op_code = OP_GUESS_RESULT;
    pkt.sender_pid = 0;
    pkt.bulls = bulls;
    pkt.cows = cows;
    strncpy(pkt.guess, guess, sizeof(pkt.guess) - 1);
    sendPacket(pkt, getClientPipePath(recipient_pid).c_str());
}