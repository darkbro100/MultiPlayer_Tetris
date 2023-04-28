//
// Created by pauli on 4/25/2023.
//

#include "NetworkClient.h"

namespace Tetris {
    void NetworkClient::connect(
            const asio::ip::basic_resolver<asio::ip::tcp, asio::any_io_executor>::results_type &endpoint) {

        asio::async_connect(socket, endpoint, [this](std::error_code ec, const asio::ip::tcp::endpoint &endpoints) {
            if (!ec) {
                std::cout << "Connected to " << endpoints << std::endl;
            } else {
                std::cout << "Failed to connect to " << endpoints << std::endl;
            }
        });

    }

    void NetworkClient::read() {
        asio::async_read(socket, asio::buffer(&modifyingMessage.messageSize, sizeof(modifyingMessage.messageSize)),
                         [this](std::error_code ec, std::size_t length) {
                             if (!ec) {
                                 std::cout << "Read " << length << " bytes" << std::endl;
                             } else {
                                 std::cout << "Failed to read " << length << " bytes" << std::endl;
                             }
                         });
    }

    void NetworkClient::send(const NetworkMessage &message) {
        if(message.messageSize <= 0)
            return;

        asio::post(ioContext, [this, message]() {
            bool isWriting = !outgoing.empty();
            outgoing.push(message);

            if (!isWriting) {
                beginMessageSend();
            }
        });
    }

    void NetworkClient::beginMessageSend() {
        asio::async_write(socket, asio::buffer(&outgoing.front().messageSize, sizeof(outgoing.front().messageSize)),
                          [this](std::error_code ec, std::size_t length) {
                              if (!ec) {
                                  std::cout << "Wrote " << length << " bytes" << std::endl;
                                  finishMessageSend();
                              } else {
                                  std::cout << "Failed to write " << length << " bytes" << std::endl;
                              }
                          });
    }

    void NetworkClient::finishMessageSend() {
        asio::async_write(socket, asio::buffer(outgoing.front().data.data(), outgoing.front().messageSize), [this](std::error_code ec, std::size_t length) {
            if(!ec) {
                std::cout << "Wrote " << length << " bytes" << std::endl;
                outgoing.pop();

                if(!outgoing.empty()) {
                    beginMessageSend();
                }
            } else {
                std::cout << "Failed to write " << length << " bytes" << std::endl;
            }
        });
    }
} // Tetris