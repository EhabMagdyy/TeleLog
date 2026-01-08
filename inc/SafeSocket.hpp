#pragma once

class SafeSocket{
    int socketfd{-1};
public:
    explicit SafeSocket(int sockerFd);
    ~SafeSocket();
    // Delete Copy Constructor & Copy Assignment
    SafeSocket(const SafeSocket&) = delete;
    SafeSocket& operator=(const SafeSocket&) = delete;
    SafeSocket(SafeSocket&& other) noexcept;
    SafeSocket& operator=(SafeSocket&& other) noexcept;

    int getSocketFD() const;
};