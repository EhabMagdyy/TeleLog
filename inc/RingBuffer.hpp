#pragma once
#include <vector>
#include <optional>
#include <cstddef>
#include <utility>
#include <mutex>

template <typename T>
class RingBuffer{
private:
    std::vector<std::optional<T>> buffer;   // passed in LogManager
    std::size_t head = 0;       // where the next element will be pushed
    std::size_t tail = 0;       // where the next element will be popped
    std::size_t capacity = 0;   // max number of elements the buffer can hold
    bool full = false;
    std::mutex mtx;
public:
    RingBuffer(size_t capacity) : buffer(capacity), capacity(capacity) {}

    // No Copy Semantics
    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;

    // Move Semantics
    RingBuffer(RingBuffer&&) = default;
    RingBuffer& operator=(RingBuffer&&) = default;

    bool tryPush(const T& element){
        // locking buffer for thread safety
        std::lock_guard<std::mutex> lock(mtx);
        if(full) {
            return false;
        }
        // Place the element at the head & move the head forward
        buffer[head] = std::move(element);
        head = (head + 1) % capacity;   // circular ++

        if(head == tail) {
            full = true;
        }
        return true;
    }

    std::optional<T> tryPop(){
        std::lock_guard<std::mutex> lock(mtx);
        if(head == tail && !full) {
            return std::nullopt;    // buffer is empty
        }
        // pop the element at the tail & move the tail forward
        std::optional<T> item = std::move(buffer[tail]);
        buffer[tail].reset();
        tail = (tail + 1) % capacity;
        full = false;

        return item;
    }
};