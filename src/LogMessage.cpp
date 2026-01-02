#include "LogMessage.hpp"
#include <iostream>
#include <iomanip>
#include <ctime>

LogMessage::LogMessage(const std::string& app, const std::string& cntxt, const std::string& msg, LogType sev)
                        : appName(app), context(cntxt), message(msg), severity(sev){
    timestamp = std::chrono::system_clock::now();
}

std::string LogMessage::getLogTypeString(LogType& severity) const{
    switch(severity){
        case LogType::INFO:         return "INFO";
        case LogType::WARNING:      return "WARNING";
        case LogType::ERROR:        return "ERROR";
        default:                    return "UNKNOWN";
    }
}

std::ostream& operator<< (std::ostream& outStream, const LogMessage& log){
    // Convert timestamp to time_t
    std::time_t t = std::chrono::system_clock::to_time_t(log.timestamp);

    // Convert to local time
    std::tm tm = *std::localtime(&t);

    // Print timestamp in YYYY-MM-DD HH:MM:SS format
    outStream << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " "
              << "[" << log.getLogTypeString(const_cast<LogType&>(log.severity)) << "] "
              << log.appName  << " - " 
              << log.context << " : " << log.message << std::endl;
    return outStream;
}