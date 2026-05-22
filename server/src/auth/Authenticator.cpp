#include "../include/auth/Authenticator.h"

Authenticator::Authenticator() : authRequired_(false), apiKey_("") {}

void Authenticator::setAuthRequired(bool required) {
    authRequired_ = required;
}

bool Authenticator::isAuthRequired() const {
    return authRequired_;
}

void Authenticator::setApiKey(const std::string& apiKey) {
    apiKey_ = apiKey;
}

bool Authenticator::authenticate(const HTTPRequest& request) const {
    if (!authRequired_) {
        return true;
    }
    
    // Try X-API-Key header
    std::string providedKey = extractApiKeyFromHeader(request);
    if (!providedKey.empty() && validateApiKey(providedKey)) {
        return true;
    }
    
    // Try Bearer token
    std::string token = extractBearerToken(request);
    if (!token.empty() && validateApiKey(token)) {
        return true;
    }
    
    return false;
}

HTTPResponse Authenticator::unauthorizedResponse() const {
    return HTTPResponse::unauthorized("Authentication required. Provide API key via X-API-Key header or Bearer token.");
}

std::string Authenticator::extractApiKeyFromHeader(const HTTPRequest& request) const {
    return request.getHeader("X-API-Key", "");
}

std::string Authenticator::extractBearerToken(const HTTPRequest& request) const {
    std::string authHeader = request.getHeader("Authorization", "");
    if (authHeader.substr(0, 7) == "Bearer ") {
        return authHeader.substr(7);
    }
    return "";
}

bool Authenticator::validateApiKey(const std::string& providedKey) const {
    return providedKey == apiKey_;
}
