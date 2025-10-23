#ifndef DATABASE_API_SERVER_H
#define DATABASE_API_SERVER_H

#include <memory>
#include "ServerConfig.h"
#include "http/HTTPServer.h"
#include "auth/Authenticator.h"
#include "routes/ItemRoutes.h"
#include "routes/ContainerRoutes.h"
#include "routes/LocationRoutes.h"
#include "routes/ProjectRoutes.h"
#include "routes/CategoryRoutes.h"
#include "routes/ActivityLogRoutes.h"
#include "../include/Database.h"

/**
 * @brief Database API Server (Main Server Coordinator)
 * 
 * Coordinates all server components:
 * - HTTP server
 * - Authentication
 * - Route handlers
 * - JSON serialization/deserialization
 * 
 * This is the main entry point for the database server.
 */
class DatabaseAPIServer {
public:
    DatabaseAPIServer(std::shared_ptr<IDatabase> database, const ServerConfig& config);
    ~DatabaseAPIServer();
    
    // Server lifecycle
    void start();
    void stop();
    bool isRunning() const;
    
private:
    std::shared_ptr<IDatabase> database;
    ServerConfig config;
    
    // Components
    std::unique_ptr<HTTPServer> httpServer;
    std::unique_ptr<Authenticator> authenticator;
    std::unique_ptr<ItemRoutes> itemRoutes;
    std::unique_ptr<ContainerRoutes> containerRoutes;
    std::unique_ptr<LocationRoutes> locationRoutes;
    std::unique_ptr<ProjectRoutes> projectRoutes;
    std::unique_ptr<CategoryRoutes> categoryRoutes;
    std::unique_ptr<ActivityLogRoutes> activityLogRoutes;
    
    // Initialization
    void registerAllRoutes();
    void setupAuthenticationMiddleware();
    HTTPResponse handleSearch(const HTTPRequest& req);
};

#endif // DATABASE_API_SERVER_H
