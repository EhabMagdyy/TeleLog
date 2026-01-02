#include "LogManager.hpp"

void LogManager::addLog(const LogMessage& log) {
    logs.push_back(log);
}