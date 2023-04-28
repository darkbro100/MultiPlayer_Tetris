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
        void push(const T & t);
        T pop();
        const T & front();
        bool empty();
        void clear();
        void wait();
    private:
        std::queue<T> queue{};

        std::condition_variable cv;
        std::mutex cvMutex;

        std::mutex queueMutex;
    };

    template<typename T>
    void ConcurrentQueue<T>::push(const T & t) {
        // Lock the mutex, push the item, unlock the mutex, notify the condition variable
        std::unique_lock<std::mutex> lock(queueMutex);
        queue.emplace(std::move(t));
        lock.unlock();

        std::unique_lock<std::mutex> cvLock(cvMutex);
        cv.notify_one();
    }

    template<typename T>
    T ConcurrentQueue<T>::pop() {
        std::unique_lock<std::mutex> lock(queueMutex);
        auto t = std::move(queue.front());
        queue.pop();
        lock.unlock();

        return std::move(t);
    }

    template<typename T>
    void ConcurrentQueue<T>::wait() {
        while (empty()) {
            std::unique_lock<std::mutex> lock(cvMutex);
            cv.wait(lock);
        }
    }

    template<typename T>
    void ConcurrentQueue<T>::clear() {
        std::unique_lock<std::mutex> lock(queueMutex);
        while (!queue.empty()) {
            queue.pop();
        }
    }

    template<typename T>
    bool ConcurrentQueue<T>::empty() {
        std::unique_lock<std::mutex> lock(queueMutex);
        return queue.empty();
    }

    template<typename T>
    const T & ConcurrentQueue<T>::front() {
        std::unique_lock<std::mutex> lock(queueMutex);
        return queue.front();
    }
} // Tetris

#endif //MPTETRIS_CONCURRENTQUEUE_H
