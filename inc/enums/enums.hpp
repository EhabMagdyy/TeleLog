#pragma once
#include <magic_enum.hpp>

enum class LogSinkType_enum{
    CONSOLE,
    FILE,
    SOCKET
};

enum class SeverityLvl_enum{
    CRITICAL,
    WARNING,
    INFO
};

enum class TelemetrySrc_enum{
    CPU,
    GPU,
    RAM
};
