#include "GameClient.h"
#include "GameMessages.h" 
#include "exceptions.h"
#include "PosixPipe.h" 
#include <iostream> 
#include <unistd.h> 
#include <sstream>
#include <cstring>
#include <limits> 
#include <cstdlib> 

namespace game_client {

    GameClient::GameClient() 
        : myPid(getpid()), 
          clientPipePath(GameMessages::getClientPipePath(myPid)), 
          reader_thread(GameClient::receive_loop_wrapper) 
    {
        POSIXPipe pipe(clientPipePath);
        pipe.unlinkFile();
        if (!pipe.create()) {
            throw exceptions::PipeException("Failed to create client FIFO.");
        }
        reader_thread.Run(this); 
    }

    GameClient::~GameClient() {
        running = false;
        
        if (is_joined) {
            GamePacket pkt;
            memset(&pkt, 0, sizeof(pkt));
            pkt.op_code = OP_EXIT;
            pkt.sender_pid = myPid;
            sendPacket(pkt);
        }

        reader_thread.Join();

        POSIXPipe pipe(clientPipePath);
        pipe.unlinkFile();
        std::cout << "Client shut down." << std::endl;
    }

    void GameClient::sendPacket(GamePacket& pkt) {
        pkt.sender_pid = myPid; 
        GameMessages::sendPacket(pkt, SERVER_FIFO);
    }

    void* GameClient::receive_loop_wrapper(void* context) {
        GameClient* client = static_cast<GameClient*>(context);
        
        POSIXPipe myPipe(client->clientPipePath);
        if (!myPipe.openRead()) {
            std::cerr << "Reader: Failed to open FIFO." << std::endl;
            return nullptr;
        }

        GamePacket pkt;
        while (client->running) {
            if (myPipe.receive(&pkt, sizeof(GamePacket))) { 
                client->handleServerResponse(pkt);
            }
        }
        myPipe.closePipe();
        return nullptr;
    }

    void GameClient::handleServerResponse(const GamePacket& pkt) {
        std::cout << "\n--- SERVER MESSAGE ---\n";
        
        if (pkt.op_code == OP_MSG_TEXT) {
            std::cout << "[INFO] " << pkt.text_payload << std::endl;
            
            if (strstr(pkt.text_payload, "Game Started!") != nullptr) {
                is_playing = true;
                is_joined = true;
            }
            
            if (strstr(pkt.text_payload, "has disconnected.") != nullptr) {
                is_playing = false;
                is_joined = false;
                displayMenu();
                handleInput();
            } else if (strstr(pkt.text_payload, "It is your turn now.") != nullptr) {
                displayMenu();
                handleInput();
            }
        } else if (pkt.op_code == OP_ERROR) {
             std::cout << "[ERROR] " << pkt.text_payload << std::endl;
             if (is_playing) {
                displayMenu();
                handleInput();
             }
        } else if (pkt.op_code == OP_GUESS_RESULT) {
            std::cout << "[RESULT] " << pkt.guess << ": " << pkt.bulls << "B, " << pkt.cows << "C." << std::endl;
        } else if (pkt.op_code == OP_GAME_WIN) {
            std::cout << "*** VICTORY ***\n" << pkt.text_payload << "\n***************\n";
            is_playing = false;
            is_joined = false;
            displayMenu();
            handleInput();
        }
        std::cout << "----------------------\n";
    }

    void GameClient::displayMenu() {
        std::cout << "\n--- Menu ---\n";
        if (is_playing) {
            std::cout << "1. Guess\n2. Exit\n";
        } else if (is_joined) {
            std::cout << "Waiting (2. Exit)\n";
        } else {
            std::cout << "1. Create\n2. Join\n3. Find (Auto)\n4. Exit\n";
        }
        std::cout << "> ";
    }

    void GameClient::handleInput() {
        int choice;
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return; 
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

        if (!is_joined && choice == 4) {
            running = false;
            exit(0); 
            return;
        }

        if (is_joined && choice == 2) {
            GamePacket exit_pkt;
            memset(&exit_pkt, 0, sizeof(exit_pkt));
            exit_pkt.op_code = OP_EXIT;
            sendPacket(exit_pkt);
            
            is_playing = false;
            is_joined = false;
            
            displayMenu();
            handleInput();
            return;
        }

        GamePacket pkt;
        memset(&pkt, 0, sizeof(pkt));
        
        if (is_playing) {
            if (choice == 1) {
                std::cout << "Guess (4 digits): ";
                
                std::string guess_input;
                if (std::getline(std::cin, guess_input)) {
                    strncpy(pkt.guess, guess_input.c_str(), sizeof(pkt.guess) - 1);
                    pkt.op_code = OP_GUESS;
                    sendPacket(pkt);
                }
            }
        } else if (!is_joined) {
            if (choice == 1) {
                std::cout << "Name: "; 
                std::cin >> pkt.game_name;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
                
                std::cout << "Players: "; 
                if (!(std::cin >> pkt.players_count)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

                pkt.op_code = OP_CREATE_GAME;
                currentGameName = pkt.game_name;
                sendPacket(pkt);
                is_joined = true;
                
            } else if (choice == 2) {
                std::cout << "Name: "; 
                std::cin >> pkt.game_name;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

                pkt.op_code = OP_JOIN_BY_NAME;
                currentGameName = pkt.game_name;
                sendPacket(pkt);
                is_joined = true;
            } else if (choice == 3) {
                pkt.op_code = OP_FIND_GAME;
                pkt.players_count = 0;
                currentGameName = "Auto";
                sendPacket(pkt);
                is_joined = true;
            }
        }
    }

    void GameClient::run() {
        std::cout << "PID: " << myPid << std::endl;
        
        displayMenu();
        handleInput();

        while (running) {
            usleep(100000); 
        }
    }
}