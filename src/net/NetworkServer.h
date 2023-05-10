//
// Created by pauli on 4/27/2023.
//
#ifndef MPTETRIS_NETWORKSERVER_H
#define MPTETRIS_NETWORKSERVER_H

#include "asio.hpp"
#include "NetworkMessage.h"
#include "ConcurrentQueue.h"
#include <thread>

namespace Tetris {

    class NetworkServer {
    public:
        /**
         * Creates a new NetworkServer instance. This will not start the server, you must call start() to start the server.
         * @param port Port to listen on
         */
        explicit NetworkServer(uint16_t port) : acceptor(ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {};

        /**
         * Destructor will handle destroying the ASIO related stuff
         */
        ~NetworkServer();

        /**
         * Sends a message to all clients
         * @param message Message to send
         * @param id ID of client to exclude from sending to
         */
        void sendMessageToAll(const NetworkMessage & message, uint32_t id = 0);

        /**
         * Sends a message to a specific client
         * @param message Message to send
         * @param client ID of the client to send to
         */
        void sendMessageTo(const NetworkMessage & message, std::shared_ptr<NetworkClient> & client);

        /**
         * Starts the NetworkServer
         */
        void start();

        /**
         * Updates the server and processes incoming messages.
         * @param block Whether or not we should block the current thread. By default, blocking behavior is disabled. However, in our Tetris server we end up blocking so we don't have to deal with race conditions.
         */
        void update(bool block = false);

        /**
         * Assign a std::function handler to be called when a message is received.
         * @param handler Handler to call
         */
        void onMessageReceive(std::function<void(std::shared_ptr<NetworkClient>, NetworkMessage &)> handler);

        /**
         * Assign a std::function handler to be called when a client pre-connects (before the client is added to the client list, hence why the lambda returns a boolean)
         * @param handler Handler to call
         */
        void onClientPreConnect(std::function<bool(std::shared_ptr<NetworkClient>)> handler);

        /**
         * Assign a std::function handler to be called when a client connects (fully connected, they've been accepted into the server)
         * @param handler Handler to call
         */
        void onClientConnect(std::function<void(std::shared_ptr<NetworkClient>)> handler);

        /**
         * Assign a std::function handler to be called when a client disconnects
         * @param handler Handler to call
         */
        void onClientDisconnect(std::function<void(std::shared_ptr<NetworkClient>&)> handler);
    private:
        /**
         * Stops the NetworkServer, used in the destructor
         */
        void stop();

        /**
         * Begins the routine to start listening for an incoming client
         */
        void listenForClient();

        /**
         * Queue for incoming messages
         */
        ConcurrentQueue<OwnedNetworkMessage> incoming;

        /**
         * ASIO related stuff
         */
        asio::io_context ioContext;
        asio::ip::tcp::acceptor acceptor;

        /**
         * Thread for running the server
         */
        std::thread serverThread;

        /**
         * Vector containing all the connected clients
         */
        std::vector<std::shared_ptr<NetworkClient>> clients;

        // Function for handling client related activities
        std::function<bool(std::shared_ptr<NetworkClient>)> preConnectHandler;
        std::function<void(std::shared_ptr<NetworkClient>)> connectHandler;
        std::function<void(std::shared_ptr<NetworkClient>&)> disconnectHandler = [](std::shared_ptr<NetworkClient>) { };

        // Function for handling messages
        std::function<void(std::shared_ptr<NetworkClient>, NetworkMessage &)> messageHandler;

        // Global ID counter used to represent each client
        uint32_t clientIdCounter = 10000;
    };
}

#endif //MPTETRIS_NETWORKSERVER_H
