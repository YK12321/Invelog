#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <map>

/**
 * @brief HTTP Request structure
 * 
 * Represents an incoming HTTP request with method, path, headers, and body.
 */
struct HTTPRequest {
    std::string method;                           // GET, POST, PUT, DELETE, etc.
    std::string path;                             // /api/items, /api/containers, etc.
    std::map<std::string, std::string> headers;   // HTTP headers
    std::map<std::string, std::string> queryParams; // Query parameters
    std::string body;                             // Request body (typically JSON)
    
    // Helper methods
    bool hasHeader(const std::string& name) const;
    std::string getHeader(const std::string& name, const std::string& defaultValue = "") const;
    bool hasQueryParam(const std::string& name) const;
    std::string getQueryParam(const std::string& name, const std::string& defaultValue = "") const;
};

#endif // HTTP_REQUEST_H
