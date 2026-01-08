#include "FileTelemetrySourceImpl.hpp"

bool FileTelemetrySourceImpl::openSource(){

    return true;
}

bool FileTelemetrySourceImpl::readSource(std::string& out){
    out = "Test";
    return true;
}