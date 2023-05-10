//
// Created by pauli on 4/27/2023.
//

#ifndef MPTETRIS_CONCURRENTQUEUE_H
#define MPTETRIS_CONCURRENTQUEUE_H

#include <mutex>
#include <queue>
#include <condition_variable>

namespace Tetris {
    /**
     * A thread-safe queue implementation
     * @tparam T TypeName to infer for the Queue
     */
    template <typename T>
    class ConcurrentQueue {
    public:
        /**
         * Pushes a new item into the queue
         * @param t Item to push
         */
        void push(const T & t);

        /**
         * Pops an item from the queue
         * @return Item popped
         */
        T pop();

        /**
         * Returns the front of the queue
         * @return Reference to the front of the queue
         */
        const T & front();

        /**
         * Returns whether the queue is empty
         * @return If queue is empty, true, false otherwise
         */
        bool empty();

        /**
         * Clears the queue
         */
        void clear();

        /**
         * Waits until the queue is not empty. Held by a conditional variables. Although in our server implementation we are full blocking it is nice to have as well if we want to multithread our game object.
         */
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
