#include "LogMessage.hpp"
#include <iostream>
#include <iomanip>
#include <ctime>

LogMessage::LogMessage(const std::string& app, const std::string& cntxt, const std::string& msg, LogType sev, std::chrono::system_clock::time_point timestamp)
                        : appName(app), context(cntxt), message(msg), severity(sev), timestamp(timestamp){
}

std::string LogMessage::getLogTypeString(LogType severity) const{
    switch(severity){
        case LogType::INFO:         return "INFO";
        case LogType::WARNING:      return "WARNING";
        case LogType::ERROR:        return "ERROR";
        default:                    return "UNKNOWN";
    }
}

std::ostream& operator<< (std::ostream& outStream, const LogMessage& log){
    std::time_t t = std::chrono::system_clock::to_time_t(log.timestamp);
    std::tm tm = *std::localtime(&t);

    outStream << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " "
              << "[" << log.getLogTypeString(const_cast<LogType&>(log.severity)) << "] "
              << log.appName  << " - " 
              << log.context << ": " << log.message << std::endl;
    return outStream;
}