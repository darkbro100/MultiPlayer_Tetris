// this is required otherwise asio tries to include
// other boost libraries which you won't have installed
// or you could add a -DASIO_STANDALONE flag right next to -std=c++11 in CMakeLists.txt

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#define ASIO_STANDALONE

#include "TetrisApp.h"
#include "./net/NetworkMessage.h"
#include "./net/NetworkClient.h"
#include "./net/NetworkServer.h"

int main(int argc, char *argv[]) {
    asio::io_context context;

    const std::string & host = "127.0.0.1";
    const uint16_t port = 25000;

    // Resolve hostname/ip-address into tangiable physical address
    asio::ip::tcp::resolver resolver(context);
    asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

    Tetris::NetworkClient client(context, asio::ip::tcp::socket(context));
    client.connect(endpoints);

    while(true) {

    }
    return 0;
}
//
//int main(int argc, char *argv[]) {
//    Tetris::NetworkServer server(25000);
//    server.start();
//
//    while(true) {
//        server.update();
//    }
//
////    Tetris::App app;
////    app.run();
//    return 0;
//}