#include "../include/http/HTTPServer.h"
#include <iostream>

HTTPServer::HTTPServer(int port) : port(port), server(std::make_unique<httplib::Server>()) {}

HTTPServer::~HTTPServer() {
    stop();
}

void HTTPServer::registerRoute(const std::string& method, const std::string& pattern, RouteHandler handler) {
    if (method == "GET") {
        server->Get(pattern.c_str(), [handler](const httplib::Request& req, httplib::Response& res) {
            HTTPRequest request;
            request.method = "GET";
            request.path = req.path;
            request.body = req.body;
            request.headers = req.headers;
            request.params = req.params;
            
            HTTPResponse response = handler(request);
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType.c_str());
            
            for (const auto& [key, value] : response.headers) {
                res.set_header(key.c_str(), value.c_str());
            }
        });
    } else if (method == "POST") {
        server->Post(pattern.c_str(), [handler](const httplib::Request& req, httplib::Response& res) {
            HTTPRequest request;
            request.method = "POST";
            request.path = req.path;
            request.body = req.body;
            request.headers = req.headers;
            request.params = req.params;
            
            HTTPResponse response = handler(request);
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType.c_str());
            
            for (const auto& [key, value] : response.headers) {
                res.set_header(key.c_str(), value.c_str());
            }
        });
    } else if (method == "PUT") {
        server->Put(pattern.c_str(), [handler](const httplib::Request& req, httplib::Response& res) {
            HTTPRequest request;
            request.method = "PUT";
            request.path = req.path;
            request.body = req.body;
            request.headers = req.headers;
            request.params = req.params;
            
            HTTPResponse response = handler(request);
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType.c_str());
            
            for (const auto& [key, value] : response.headers) {
                res.set_header(key.c_str(), value.c_str());
            }
        });
    } else if (method == "DELETE") {
        server->Delete(pattern.c_str(), [handler](const httplib::Request& req, httplib::Response& res) {
            HTTPRequest request;
            request.method = "DELETE";
            request.path = req.path;
            request.body = req.body;
            request.headers = req.headers;
            request.params = req.params;
            
            HTTPResponse response = handler(request);
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType.c_str());
            
            for (const auto& [key, value] : response.headers) {
                res.set_header(key.c_str(), value.c_str());
            }
        });
    }
}

void HTTPServer::setMiddleware(std::function<bool(const HTTPRequest&, HTTPResponse&)> middleware) {
    this->middleware = middleware;
}

bool HTTPServer::start() {
    if (running) {
        return false;
    }
    
    std::cout << "Starting HTTP server on port " << port << "..." << std::endl;
    
    // Run server in a separate thread
    serverThread = std::thread([this]() {
        running = true;
        if (!server->listen("0.0.0.0", port)) {
            std::cerr << "Failed to start server on port " << port << std::endl;
            running = false;
        }
    });
    
    return true;
}

void HTTPServer::stop() {
    if (running) {
        std::cout << "Stopping HTTP server..." << std::endl;
        server->stop();
        if (serverThread.joinable()) {
            serverThread.join();
        }
        running = false;
    }
}

bool HTTPServer::isRunning() const {
    return running;
}

int HTTPServer::getPort() const {
    return port;
}
