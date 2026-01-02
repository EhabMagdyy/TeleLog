#include "ConsoleSinkImpl.hpp"

void ConsoleSink::write(const LogMessage& log){
    std::cout << log;
}