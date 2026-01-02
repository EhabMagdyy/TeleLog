#include "LogMessage.hpp"

LogMessage::LogMessage(const std::string& app, const std::string& ctx, const std::string& msg, LogType sev)
                        : appName(app), context(ctx), message(msg), severity(sev){
    timestamp = std::chrono::system_clock::now();
}