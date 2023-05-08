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
        const static uint8_t CONNECTED = 1;
        const static uint8_t DISCONNECTED = 2;
        const static uint8_t CONNECTING = 3;
        const static uint8_t WAITING = 0;

        NetworkClient(asio::io_context& asioContext, asio::ip::tcp::socket _socket, ConcurrentQueue<OwnedNetworkMessage> & _incoming) : ioContext(asioContext), socket(std::move(_socket)), incoming(_incoming) {};
        ~NetworkClient();
        void connect(const asio::ip::tcp::resolver::results_type & endpoint);
        void connect(uint32_t id);

        void send(const NetworkMessage & message);
        void onMessageReceive(std::function<void(NetworkMessage&)> handler);
        void onConnect(const std::function<void(bool)> & handler);

        void setId(uint32_t id);
        uint32_t getId() const;
        bool isSocketOpen();
    private:
        void beginRead();
        void finishRead();

        void addToIncomingQueue();
        void beginMessageSend();
        void finishMessageSend();

        ConcurrentQueue<OwnedNetworkMessage> & incoming;
        ConcurrentQueue<NetworkMessage> outgoing{};

        asio::io_context & ioContext;
        asio::ip::tcp::socket socket;

        NetworkMessage modifyingMessage;

        std::thread clientThread;

        // Function for handling messages
        std::function<void(bool)> connectionHandler = [](bool) { };
        std::function<void(NetworkMessage &)> messageHandler = [](NetworkMessage&) {};

        uint32_t clientId = 0;
        ClientType clientType;
    };

} // Tetris

#endif //MPTETRIS_NETWORKCLIENT_H
