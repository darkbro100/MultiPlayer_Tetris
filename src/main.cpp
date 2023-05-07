// this is required otherwise asio tries to include
// other boost libraries which you won't have installed
// or you could add a -DASIO_STANDALONE flag right next to -std=c++11 in CMakeLists.txt

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#define ASIO_STANDALONE

#include "TetrisApp.h"

int main(int argc, char *argv[]) {
    Tetris::App app;
    app.run();
    return 0;
}