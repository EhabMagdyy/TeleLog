#pragma once
#include "LogMessage.hpp"
#include "enums.hpp"
#include <optional>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>

template <typename Policy>

class LogFormatter{
public:
    // takes a value received from a telemetry source (float formatted as a string) and infers the severity,
    // fills in the timestamp, and constructs a full log message.
    static std::optional<LogMessage> formatDataToLogMsg(const std::string& raw){
        float val = std::stof(raw);

        SeverityLvl_enum severity = Policy::inferSeverity(val);

        std::string msg = msgDescription(val);
        std::string timestamp = currentTimeStamp();

        return LogMessage{
            "SysMonitor",
            telemetryContextToString(Policy::context),
            msg,
            severity,
            std::chrono::system_clock::now()
        };
    }
    // generates a message description/payload describing the received reading
    static std::string msgDescription(float val){
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << val << Policy::unit;
        return oss.str();
    }
    // generates the timestamp at message construction.
    static std::string currentTimeStamp(){
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm{};
        localtime_r(&t, &tm);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

private:
    static std::string telemetryContextToString(TelemetrySrc_enum src){
        auto name = magic_enum::enum_name(src);  // "CPU", "GPU", "RAM"
        if(name.empty()){
            return "Unknown";
        }
        return std::string(name) + " Usage";     // "CPU Usage" / "GPU Usage" / "RAM Usage"
    }
};
