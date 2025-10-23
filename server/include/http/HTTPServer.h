#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include "RouteHandler.h"

/**
 * @brief HTTP Server
 * 
 * Manages HTTP server lifecycle, route registration, and request routing.
 * Wraps the underlying HTTP library (cpp-httplib).
 */
class HTTPServer {
public:
    HTTPServer(int port = 8080);
    ~HTTPServer();
    
    // Server lifecycle
    bool start();
    void stop();
    bool isRunning() const;
    
    // Configuration
    void setPort(int port);
    int getPort() const;
    
    // Route registration
    void addRoute(const std::string& method, const std::string& path, RouteHandler handler);
    void removeRoute(const std::string& method, const std::string& path);
    
    // Direct request handling (for testing without starting server)
    HTTPResponse handleRequest(const HTTPRequest& request);
    
private:
    int port_;
    bool running_;
    std::mutex mutex_;
    std::thread serverThread_;
    
    // Route table: method -> path -> handler
    std::map<std::string, std::map<std::string, RouteHandler>> routes_;
    
    // Internal HTTP library instance (implementation detail)
    class HTTPServerImpl;
    std::unique_ptr<HTTPServerImpl> impl_;
    
    // Helper methods
    RouteHandler findHandler(const std::string& method, const std::string& path);
    std::string extractPathSegment(const std::string& path, int segmentIndex);
};

#endif // HTTP_SERVER_H
