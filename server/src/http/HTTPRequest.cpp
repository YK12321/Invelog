#include "../include/http/HTTPRequest.h"

bool HTTPRequest::hasHeader(const std::string& name) const {
    return headers.find(name) != headers.end();
}

std::string HTTPRequest::getHeader(const std::string& name, const std::string& defaultValue) const {
    auto it = headers.find(name);
    return (it != headers.end()) ? it->second : defaultValue;
}

bool HTTPRequest::hasQueryParam(const std::string& name) const {
    return queryParams.find(name) != queryParams.end();
}

std::string HTTPRequest::getQueryParam(const std::string& name, const std::string& defaultValue) const {
    auto it = queryParams.find(name);
    return (it != queryParams.end()) ? it->second : defaultValue;
}
