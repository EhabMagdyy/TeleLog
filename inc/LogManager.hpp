#include "LogMessage.hpp"
#include <vector>

class LogManager{
    std::vector<LogMessage> logs;
public:
    LogManager() = default;
    ~LogManager() = default;
    void addLog(const LogMessage& log);
};
