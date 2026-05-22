#include "http/HTTPServer.h"
#include "http/HTTPRequest.h"
#include "http/HTTPResponse.h"
#include <httplib.h>
#include <iostream>
#include <algorithm>
#include <vector>

// Internal implementation using cpp-httplib
class HTTPServer::HTTPServerImpl {
public:
    std::unique_ptr<httplib::Server> server;
    
    HTTPServerImpl() : server(std::make_unique<httplib::Server>()) {}
};

HTTPServer::HTTPServer(int port)
    : port_(port)
    , running_(false)
    , impl_(std::make_unique<HTTPServerImpl>()) {
}

HTTPServer::~HTTPServer() {
    stop();
}

bool HTTPServer::start() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (running_) {
        return false;
    }
    
    std::cout << "Starting HTTP server on port " << port_ << "..." << std::endl;
    
    // Start server in a separate thread
    serverThread_ = std::thread([this]() {
        running_ = true;
        if (!impl_->server->listen("0.0.0.0", port_)) {
            std::cerr << "Failed to start server on port " << port_ << std::endl;
            std::lock_guard<std::mutex> lock(mutex_);
            running_ = false;
        }
    });
    
    // Give server a moment to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    return running_;
}

void HTTPServer::stop() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (running_) {
        std::cout << "Stopping HTTP server..." << std::endl;
        impl_->server->stop();
        if (serverThread_.joinable()) {
            serverThread_.join();
        }
        running_ = false;
    }
}

bool HTTPServer::isRunning() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return running_;
}

void HTTPServer::setPort(int port) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!running_) {
        port_ = port;
    }
}

int HTTPServer::getPort() const {
    return port_;
}

// Helper function to check if pattern matches path
static bool pathMatches(const std::string& pattern, const std::string& path) {
    std::vector<std::string> patternSegments;
    std::vector<std::string> pathSegments;
    
    // Split pattern by '/'
    size_t start = 0;
    while (start < pattern.length()) {
        while (start < pattern.length() && pattern[start] == '/') start++;
        if (start >= pattern.length()) break;
        
        size_t end = pattern.find('/', start);
        if (end == std::string::npos) end = pattern.length();
        
        patternSegments.push_back(pattern.substr(start, end - start));
        start = end;
    }
    
    // Split path by '/'
    start = 0;
    while (start < path.length()) {
        while (start < path.length() && path[start] == '/') start++;
        if (start >= path.length()) break;
        
        size_t end = path.find('/', start);
        if (end == std::string::npos) end = path.length();
        
        pathSegments.push_back(path.substr(start, end - start));
        start = end;
    }
    
    if (patternSegments.size() != pathSegments.size()) {
        return false;
    }
    
    for (size_t i = 0; i < patternSegments.size(); i++) {
        if (patternSegments[i][0] == ':') {
            continue;
        }
        if (patternSegments[i] != pathSegments[i]) {
            return false;
        }
    }
    
    return true;
}

void HTTPServer::addRoute(const std::string& method, const std::string& path, RouteHandler handler) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    routes_[method][path] = handler;
    
    auto wrapHandler = [handler](const httplib::Request& req, httplib::Response& res) {
        // Log incoming request
        std::cout << "[" << req.method << "] " << req.path;
        if (!req.params.empty()) {
            std::cout << " (with query params)";
        }
        std::cout << std::endl;
        
        HTTPRequest request;
        request.method = req.method;
        request.path = req.path;
        request.body = req.body;
        
        for (const auto& [key, value] : req.headers) {
            request.headers[key] = value;
        }
        
        for (const auto& [key, value] : req.params) {
            request.queryParams[key] = value;
        }
        
        HTTPResponse response = handler(request);
        
        // Log response status
        std::cout << "    → " << response.statusCode << " " 
                  << (response.statusCode < 300 ? "✓" : (response.statusCode < 400 ? "→" : "✗"))
                  << std::endl;
        
        res.status = response.statusCode;
        
        std::string contentType = "application/json";
        auto it = response.headers.find("Content-Type");
        if (it != response.headers.end()) {
            contentType = it->second;
        }
        
        res.set_content(response.body, contentType);
        
        for (const auto& [key, value] : response.headers) {
            if (key != "Content-Type") {
                res.set_header(key, value);
            }
        }
    };
    
    // Convert .* wildcard pattern to cpp-httplib regex format
    // e.g., "/api/items/.*" becomes R"(/api/items/([^/]+))"
    std::string cppHttplibPath = path;
    if (path.find(".*") != std::string::npos) {
        // Replace .* with a regex capture group for any characters
        size_t pos = path.find(".*");
        cppHttplibPath = path.substr(0, pos) + R"(([^/]+))";
    }
    
    if (method == "GET") {
        impl_->server->Get(cppHttplibPath, wrapHandler);
    } else if (method == "POST") {
        impl_->server->Post(cppHttplibPath, wrapHandler);
    } else if (method == "PUT") {
        impl_->server->Put(cppHttplibPath, wrapHandler);
    } else if (method == "DELETE") {
        impl_->server->Delete(cppHttplibPath, wrapHandler);
    } else if (method == "PATCH") {
        impl_->server->Patch(cppHttplibPath, wrapHandler);
    }
}

void HTTPServer::removeRoute(const std::string& method, const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto methodIt = routes_.find(method);
    if (methodIt != routes_.end()) {
        methodIt->second.erase(path);
        if (methodIt->second.empty()) {
            routes_.erase(methodIt);
        }
    }
}

HTTPResponse HTTPServer::handleRequest(const HTTPRequest& request) {
    RouteHandler handler = findHandler(request.method, request.path);
    
    if (handler) {
        return handler(request);
    }
    
    return HTTPResponse::notFound("Route not found");
}

RouteHandler HTTPServer::findHandler(const std::string& method, const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto methodIt = routes_.find(method);
    if (methodIt == routes_.end()) {
        return nullptr;
    }
    
    auto pathIt = methodIt->second.find(path);
    if (pathIt != methodIt->second.end()) {
        return pathIt->second;
    }
    
    for (const auto& [pattern, handler] : methodIt->second) {
        if (pathMatches(pattern, path)) {
            return handler;
        }
    }
    
    return nullptr;
}

std::string HTTPServer::extractPathSegment(const std::string& path, int segmentIndex) {
    size_t start = 0;
    int currentIndex = 0;
    
    while (start < path.length()) {
        while (start < path.length() && path[start] == '/') {
            start++;
        }
        
        if (start >= path.length()) break;
        
        size_t end = path.find('/', start);
        if (end == std::string::npos) {
            end = path.length();
        }
        
        if (currentIndex == segmentIndex) {
            return path.substr(start, end - start);
        }
        
        start = end;
        currentIndex++;
    }
    
    return "";
}
