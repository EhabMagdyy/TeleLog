#pragma once
#include "enums.hpp"
#include <string_view>

struct CpuPolicy {
    static constexpr TelemetrySrc_enum context = TelemetrySrc_enum::CPU;
    static constexpr std::string_view unit = "%";
    static constexpr float WARNING  = 75.0f;
    static constexpr float CRITICAL = 90.0f;

    static constexpr SeverityLvl_enum inferSeverity(float val) noexcept {
        return (val > CRITICAL) ? SeverityLvl_enum::CRITICAL
             : (val > WARNING)  ? SeverityLvl_enum::WARNING
             :                    SeverityLvl_enum::INFO;
    }
};

struct GpuPolicy {
    static constexpr TelemetrySrc_enum context = TelemetrySrc_enum::GPU;
    static constexpr std::string_view unit = "%";
    static constexpr float WARNING  = 70.0f;
    static constexpr float CRITICAL = 85.0f;

    static constexpr SeverityLvl_enum inferSeverity(float val) noexcept {
        return (val > CRITICAL) ? SeverityLvl_enum::CRITICAL
             : (val > WARNING)  ? SeverityLvl_enum::WARNING
             :                    SeverityLvl_enum::INFO;
    }
};

struct RamPolicy {
    static constexpr TelemetrySrc_enum context = TelemetrySrc_enum::RAM;
    static constexpr std::string_view unit = "%";
    static constexpr float WARNING  = 80.0f;
    static constexpr float CRITICAL = 95.0f;

    static constexpr SeverityLvl_enum inferSeverity(float val) noexcept {
        return (val > CRITICAL) ? SeverityLvl_enum::CRITICAL
             : (val > WARNING)  ? SeverityLvl_enum::WARNING
             :                    SeverityLvl_enum::INFO;
    }
};
