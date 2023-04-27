//
// Created by pauli on 4/27/2023.
//

#include "ConcurrentQueue.h"

namespace Tetris {

    template<typename T>
    void ConcurrentQueue<T>::push(T t) {
        // Lock the mutex, push the item, unlock the mutex, notify the condition variable
        std::unique_lock<std::mutex> lock(queueMutex);
        queue.push(t);
        lock.unlock();
        cv.notify_one();
    }

    template<typename T>
    T ConcurrentQueue<T>::pop() {
        std::unique_lock<std::mutex> lock(queueMutex);
        T t = queue.front();
        queue.pop();
        lock.unlock();

        return t;
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

} // Tetris