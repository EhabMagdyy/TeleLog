#include "LogMessage.hpp"
#include <iostream>
#include <iomanip>
#include <ctime>
#include "enums.hpp"

LogMessage::LogMessage(const std::string& app, const std::string& cntxt, const std::string& msg, SeverityLvl_enum sev, std::chrono::system_clock::time_point timestamp)
                        : appName(app), context(cntxt), message(msg), severity(sev), timestamp(timestamp){
}

std::string LogMessage::getLogTypeString(SeverityLvl_enum severity) const{
    auto name = magic_enum::enum_name(severity);  // "CRITICAL", "WARNING", "INFO"
    if(name.empty()){
        return "UNKNOWN";
    }
    return std::string(name);  // "CRITICAL", "WARNING", "INFO"
}

std::ostream& operator<< (std::ostream& outStream, const LogMessage& log){
    std::time_t t = std::chrono::system_clock::to_time_t(log.timestamp);
    std::tm tm = *std::localtime(&t);

    outStream << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " "
              << "[" << log.getLogTypeString(log.severity) << "] "
              << log.appName  << " - " 
              << log.context << ": " << log.message << std::endl;
    return outStream;
}