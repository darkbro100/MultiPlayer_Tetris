//
// Created by pauli on 4/27/2023.
//

#ifndef MPTETRIS_CONCURRENTQUEUE_H
#define MPTETRIS_CONCURRENTQUEUE_H

#include <mutex>
#include <queue>
#include <condition_variable>

namespace Tetris {
    template <typename T>
    class ConcurrentQueue {
    public:
        void push(T t);
        T pop();
        bool empty();
        void clear();
        void wait();
    private:
        std::queue<T> queue;

        std::condition_variable cv;
        std::mutex cvMutex;

        std::mutex queueMutex;
    };
} // Tetris

#endif //MPTETRIS_CONCURRENTQUEUE_H
