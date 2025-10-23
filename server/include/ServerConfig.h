#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include <string>

/**
 * @brief Server Configuration
 * 
 * Holds configuration settings for the database server.
 */
struct ServerConfig {
    int port;
    bool authRequired;
    std::string apiKey;
    bool enableCORS;
    int maxRequestSize;
    int timeoutSeconds;
    
    // Default configuration
    ServerConfig()
        : port(8080),
          authRequired(false),
          apiKey(""),
          enableCORS(true),
          maxRequestSize(10 * 1024 * 1024),  // 10 MB
          timeoutSeconds(30) {}
};

#endif // SERVER_CONFIG_H
