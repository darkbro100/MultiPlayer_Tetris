//
// Created by pauli on 4/25/2023.
//

#ifndef MPTETRIS_NETWORKCLIENT_H
#define MPTETRIS_NETWORKCLIENT_H

#include "NetworkMessage.h"
#include "ConcurrentQueue.h"
#include "asio.hpp"

namespace Tetris {

    enum class ClientType {
        CLIENT,
        SERVER
    };

    class NetworkClient : public std::enable_shared_from_this<NetworkClient> {
    public:
        /**
         * States that represent the Tetris client's connection status
         */
        const static uint8_t CONNECTED = 1;
        const static uint8_t DISCONNECTED = 2;
        const static uint8_t CONNECTING = 3;
        const static uint8_t WAITING = 0;

        /**
         * @brief Construct a new Network Client object
         * @param asioContext ASIO's IO Context
         * @param _socket Socket to use for the connection
         * @param _incoming Reference to incoming messages
         */
        NetworkClient(asio::io_context& asioContext, asio::ip::tcp::socket _socket, ConcurrentQueue<OwnedNetworkMessage> & _incoming) : ioContext(asioContext), socket(std::move(_socket)), incoming(_incoming) {};

        /**
         * Destructor for handling ASIO related tasks
         */
        ~NetworkClient();

        /**
         * @brief Connect to a server. This will be used exclusively by the client.
         * @param endpoint Reference to an ASIO Endpoint, which essentially just translates to the host we're connecting to
         */
        void connect(const asio::ip::basic_resolver<asio::ip::tcp, asio::any_io_executor>::results_type &endpoint);

        /**
         * @brief Connect to a server. This will be used exclusively by the server.
         * @param id ID to be passed to the client
         */
        void connect(uint32_t id);

        /**
         * Sends a message through ASIO to our server socket.
         * @param message Reference to the message we are sending
         */
        void send(const NetworkMessage & message);

        /**
         * Callback function for handling when the client has received a message.
         * @param handler Handler to call
         */
        void onMessageReceive(std::function<void(NetworkMessage&)> handler);

        /**
         * Callback function for handling when this specific client has connected to the server.
         * @param handler Handler to call
         */
        void onConnect(const std::function<void(bool)> & handler);

        /**
         * Setter for the client's ID
         * @param id ID to set
         */
        void setId(uint32_t id);

        /**
         * Getter for the client's ID
         * @return ID of the client
         */
        uint32_t getId() const;

        /**
         * Returns whether or not the client's socket is still connected to the server.
         * @return True if connected, false otherwise
         */
        bool isSocketOpen();
    private:
        /**
         * Begins reading for a message, will look for a header message first before deducing what the size of the message needs to be.
         */
        void beginRead();

        /**
         * Once we know the size of the incoming message, we can begin reading the actual message.
         */
        void finishRead();

        /**
         * Once a message has been fully read, we can add it to the incoming queue.
         */
        void addToIncomingQueue();

        /**
         * Begins sending a message to the server. Similarily to our read routine, it will do the same except for writing out to our server socket
         */
        void beginMessageSend();

        /**
         * Once we know the size of the message we are sending, we can begin writing the actual message.
         */
        void finishMessageSend();

        /**
         * Queues relating to incoming/outgoing messages
         */
        ConcurrentQueue<OwnedNetworkMessage> & incoming;
        ConcurrentQueue<NetworkMessage> outgoing{};

        /**
         * ASIO Related vars
         */
        asio::io_context & ioContext;
        asio::ip::tcp::socket socket;

        /**
         * Buffer for reading/writing messages
         */
        NetworkMessage modifyingMessage;

        /**
         * Thread for handling ASIO tasks
         */
        std::thread clientThread;

        // Function for handling messages
        std::function<void(bool)> connectionHandler = [](bool) { };
        std::function<void(NetworkMessage &)> messageHandler = [](NetworkMessage&) {};

        /**
         * ID of the client & the type of this client
         */
        uint32_t clientId = 0;
        ClientType clientType;
    };

} // Tetris

#endif //MPTETRIS_NETWORKCLIENT_H
