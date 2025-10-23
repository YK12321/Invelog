#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <map>

/**
 * @brief HTTP Response structure
 * 
 * Represents an HTTP response with status code, headers, and body.
 */
struct HTTPResponse {
    int statusCode;                               // 200, 201, 400, 404, 500, etc.
    std::map<std::string, std::string> headers;   // HTTP headers
    std::string body;                             // Response body (typically JSON)
    
    // Constructor with defaults
    HTTPResponse();
    HTTPResponse(int status, const std::string& body);
    
    // Helper methods
    void setHeader(const std::string& name, const std::string& value);
    void setContentType(const std::string& contentType);
    void enableCORS();
    
    // Factory methods for common responses
    static HTTPResponse ok(const std::string& body);
    static HTTPResponse created(const std::string& body);
    static HTTPResponse noContent();
    static HTTPResponse badRequest(const std::string& message);
    static HTTPResponse unauthorized(const std::string& message = "Unauthorized");
    static HTTPResponse notFound(const std::string& message = "Not found");
    static HTTPResponse internalError(const std::string& message = "Internal server error");
};

#endif // HTTP_RESPONSE_H
