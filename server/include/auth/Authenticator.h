#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <string>
#include "../http/HTTPRequest.h"
#include "../http/HTTPResponse.h"

/**
 * @brief API Authentication Manager
 * 
 * Handles authentication for API requests using API keys or tokens.
 */
class Authenticator {
public:
    Authenticator();
    ~Authenticator() = default;
    
    // Configuration
    void setAuthRequired(bool required);
    bool isAuthRequired() const;
    void setApiKey(const std::string& apiKey);
    
    // Authentication
    bool authenticate(const HTTPRequest& request) const;
    HTTPResponse unauthorizedResponse() const;
    
private:
    bool authRequired_;
    std::string apiKey_;
    
    // Helper methods
    std::string extractApiKeyFromHeader(const HTTPRequest& request) const;
    std::string extractBearerToken(const HTTPRequest& request) const;
    bool validateApiKey(const std::string& providedKey) const;
};

#endif // AUTHENTICATOR_H
