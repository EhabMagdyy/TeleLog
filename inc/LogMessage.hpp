#include <string>
#include <chrono>

enum class LogType{
    INFO,
    WARNING,
    ERROR
};

class LogMessage{
    std::string appName;
    std::string context;
    std::chrono::time_point<std::chrono::system_clock> timestamp;
    std::string message;
    LogType severity;
public:
    LogMessage(const std::string& app, const std::string& ctx, const std::string& msg, LogType sev);
    ~LogMessage() = default;
};