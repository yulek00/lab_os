#include "GameServer.h"
#include "PosixPipe.h"
#include "NumberLogic.h"
#include "GameMessages.h" 
#include "exceptions.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <sstream>

namespace game_server {

GameSession* GameServer::findGameByName(const std::string& name) {
    for (auto& g : games) {
        if (g.name == name) return &g;
    }
    return nullptr;
}

GameSession* GameServer::findGameByPlayerPid(pid_t pid) {
    for (auto& g : games) {
        for (const auto& p : g.players) {
            if (p.pid == pid) return &g;
        }
    }
    return nullptr;
}

GameSession* GameServer::findOpenGame(int max_players) {
    for (auto& g : games) {
        if (!g.is_started && !g.is_finished && (int)g.players.size() < g.max_players) {
            if (max_players == 0 || g.max_players == max_players) {
                return &g;
            }
        }
    }
    return nullptr;
}

GameServer::GameServer() {
    pthread_mutex_init(&games_mutex, nullptr);
}

GameServer::~GameServer() {
    pthread_mutex_destroy(&games_mutex);
}

void GameServer::handleRequest(GamePacket& req) {
    pthread_mutex_lock(&games_mutex);

    GameSession* myGame = findGameByPlayerPid(req.sender_pid);

    if (req.op_code == OP_CREATE_GAME) {
        std::string name(req.game_name);
        if (findGameByName(name)) {
            GameMessages::sendError(req.sender_pid, "Game name already exists.");
        } else if (req.players_count < 1) {
            GameMessages::sendError(req.sender_pid, "Players count must be at least 1.");
        } else if (myGame) {
            GameMessages::sendError(req.sender_pid, "You are already in a game.");
        } else {
            GameSession newG;
            newG.name = name;
            newG.max_players = req.players_count;
            newG.secret_number = NumberLogic::generateSecret();
            newG.players.push_back({req.sender_pid});
            
            if (newG.max_players == 1) {
                newG.is_started = true;
                GameMessages::sendText(req.sender_pid, "Single-player game started! It is your turn now. Guess 4 unique digits.");
            } else {
                GameMessages::sendText(req.sender_pid, "Game created. Waiting for players...");
            }
            games.push_back(std::move(newG));
        }
    }
    else if (req.op_code == OP_JOIN_BY_NAME || req.op_code == OP_FIND_GAME) {
        GameSession* target = nullptr;
        if (req.op_code == OP_JOIN_BY_NAME) {
            target = findGameByName(std::string(req.game_name));
        } else { 
            target = findOpenGame(req.players_count); 
        }

        if (myGame) {
            GameMessages::sendError(req.sender_pid, "You are already in a game.");
        } else if (!target || target->is_started || target->is_finished || (int)target->players.size() == target->max_players) {
            GameMessages::sendError(req.sender_pid, "No suitable games found or game is full/started.");
        } else {
            target->players.push_back({req.sender_pid});

            if ((int)target->players.size() == target->max_players) {
                target->is_started = true;
                
                pid_t first_player_pid = target->players[0].pid;
                
                for (const auto& p : target->players) {
                    if (p.pid == first_player_pid) {
                        GameMessages::sendText(p.pid, "Game Started! It is your turn now. Make a guess!");
                    } else {
                        char msg[256];
                        snprintf(msg, sizeof(msg), "Game Started! Waiting for player %d to make the first move...", first_player_pid);
                        GameMessages::sendText(p.pid, msg);
                    }
                }
            } else {
                char msg[256];
                snprintf(msg, sizeof(msg), "Joined game '%s'. Waiting for players (%zu/%d)...", 
                        target->name.c_str(), target->players.size(), target->max_players);
                GameMessages::sendText(req.sender_pid, msg); 
            }
        }
    }
    else if (req.op_code == OP_GUESS) {
        if (!myGame || !myGame->is_started || myGame->is_finished) {
            GameMessages::sendError(req.sender_pid, "You are not in a running game.");
        } else if (myGame->players.empty() || myGame->players[myGame->current_turn_index].pid != req.sender_pid) {
            GameMessages::sendError(req.sender_pid, "It is not your turn yet.");
        } else if(!NumberLogic::isValidGuess(req.guess)) {
            GameMessages::sendError(req.sender_pid, "Invalid format. Use 4 unique digits.");
        } else {
            int b, c;
            NumberLogic::calculate(myGame->secret_number, req.guess, b, c);
            
            GameMessages::sendGuessResult(req.sender_pid, req.guess, b, c);
            
            if (b == 4) {
                myGame->is_finished = true;
                GamePacket winPkt;
                winPkt.op_code = OP_GAME_WIN;
                std::stringstream ss;
                ss << "Player " << req.sender_pid << " WINS the game by guessing " << req.guess << "!";
                strncpy(winPkt.text_payload, ss.str().c_str(), sizeof(winPkt.text_payload) - 1);
                
                for (const auto& p : myGame->players) {
                    GameMessages::sendPacket(winPkt, GameMessages::getClientPipePath(p.pid).c_str());
                }
                
                for (auto it = games.begin(); it != games.end(); ++it) {
                    if (it->name == myGame->name) {
                        games.erase(it);
                        break;
                    }
                }
            } else {
                myGame->current_turn_index = (myGame->current_turn_index + 1) % myGame->players.size();
                pid_t next_pid = myGame->players[myGame->current_turn_index].pid;
                
                GameMessages::sendText(next_pid, "It is your turn now. Make a guess!");
                
                for (const auto& p : myGame->players) {
                    if (p.pid != req.sender_pid && p.pid != next_pid) {
                        char msg[256];
                        snprintf(msg, sizeof(msg), "Player %d guessed %s: %dB, %dC. Waiting for player %d...",
                                req.sender_pid, req.guess, b, c, next_pid);
                        GameMessages::sendText(p.pid, msg);
                    }
                }
            }
        }
    }
    else if (req.op_code == OP_EXIT) {
        if (myGame) {
            for (auto it = myGame->players.begin(); it != myGame->players.end(); ) {
                if (it->pid == req.sender_pid) {
                    it = myGame->players.erase(it);
                    break;
                } else {
                    ++it;
                }
            }
            
            if (myGame->players.empty()) {
                for (auto it = games.begin(); it != games.end(); ++it) {
                    if (it->name == myGame->name) {
                        games.erase(it);
                        break;
                    }
                }
            } else if (myGame->is_started && !myGame->is_finished) {
                myGame->is_finished = true;
                char msg[256];
                snprintf(msg, sizeof(msg), "Game '%s' ended. Player %d has disconnected.", 
                        myGame->name.c_str(), req.sender_pid);
                
                for (const auto& p : myGame->players) {
                    GameMessages::sendText(p.pid, msg);
                }
                for (auto it = games.begin(); it != games.end(); ++it) {
                    if (it->name == myGame->name) {
                        games.erase(it);
                        break;
                    }
                }
            }
        }
    }
    else {
        GameMessages::sendError(req.sender_pid, "Unknown operation code.");
    }

    pthread_mutex_unlock(&games_mutex); 
}

void GameServer::run() {
    POSIXPipe srvPipe(SERVER_FIFO);
    srvPipe.unlinkFile(); 
    
    if (!srvPipe.create() || !srvPipe.openRead()) {
        throw exceptions::PipeException("Error starting server pipe.");
    }
    std::cout << "Server Started on " << SERVER_FIFO << std::endl;
    std::cout << "To stop server: press Ctrl+C" << std::endl;

    GamePacket req;
    while (srvPipe.receive(&req, sizeof(GamePacket))) {
        handleRequest(req); 
    }
    
    srvPipe.unlinkFile();
    std::cout << "Server stopped." << std::endl;
}
}