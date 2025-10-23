//This file is deprecated. Use server/include/DatabaseAPIServer.h instead.
#ifndef DATABASE_SERVER_H
#define DATABASE_SERVER_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include <functional>
#include "Database.h"

// HTTP Request structure
struct HTTPRequest {
    std::string method;           // GET, POST, PUT, DELETE
    std::string path;             // /api/items, /api/containers, etc.
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> queryParams;
    std::string body;             // JSON body
};

// HTTP Response structure
struct HTTPResponse {
    int statusCode;               // 200, 201, 400, 404, 500, etc.
    std::map<std::string, std::string> headers;
    std::string body;             // JSON body
    
    HTTPResponse() : statusCode(200) {
        headers["Content-Type"] = "application/json";
        headers["Access-Control-Allow-Origin"] = "*"; // CORS support
    }
};

// Route handler function type
using RouteHandler = std::function<HTTPResponse(const HTTPRequest&)>;

/**
 * @brief HTTP Server for database API
 * 
 * Provides RESTful API endpoints for inventory management operations.
 * Can use any backend database (LocalDatabase, SQLDatabase, or even another APIDatabase).
 */
class DatabaseServer {
public:
    DatabaseServer(std::shared_ptr<IDatabase> database, int port = 8080);
    ~DatabaseServer();
    
    // Server lifecycle
    bool start();
    void stop();
    bool isRunning() const;
    
    // Configuration
    void setPort(int port);
    int getPort() const;
    void setAuthRequired(bool required);
    bool isAuthRequired() const;
    void setApiKey(const std::string& apiKey);
    
    // Route management
    void addRoute(const std::string& method, const std::string& path, RouteHandler handler);
    void removeRoute(const std::string& method, const std::string& path);
    
    // Request handling (can be used for testing without starting server)
    HTTPResponse handleRequest(const HTTPRequest& request);
    
private:
    std::shared_ptr<IDatabase> database_;
    int port_;
    bool running_;
    bool authRequired_;
    std::string apiKey_;
    std::mutex mutex_;
    
    // Route table: method -> path -> handler
    std::map<std::string, std::map<std::string, RouteHandler>> routes_;
    
    // Initialize default routes
    void initializeRoutes();
    
    // Authentication
    bool authenticate(const HTTPRequest& request);
    HTTPResponse unauthorizedResponse();
    
    // Default route handlers
    HTTPResponse handleGetItems(const HTTPRequest& request);
    HTTPResponse handleGetItem(const HTTPRequest& request);
    HTTPResponse handleCreateItem(const HTTPRequest& request);
    HTTPResponse handleUpdateItem(const HTTPRequest& request);
    HTTPResponse handleDeleteItem(const HTTPRequest& request);
    
    HTTPResponse handleGetContainers(const HTTPRequest& request);
    HTTPResponse handleGetContainer(const HTTPRequest& request);
    HTTPResponse handleCreateContainer(const HTTPRequest& request);
    HTTPResponse handleUpdateContainer(const HTTPRequest& request);
    HTTPResponse handleDeleteContainer(const HTTPRequest& request);
    
    HTTPResponse handleGetLocations(const HTTPRequest& request);
    HTTPResponse handleGetLocation(const HTTPRequest& request);
    HTTPResponse handleCreateLocation(const HTTPRequest& request);
    HTTPResponse handleUpdateLocation(const HTTPRequest& request);
    HTTPResponse handleDeleteLocation(const HTTPRequest& request);
    
    HTTPResponse handleGetProjects(const HTTPRequest& request);
    HTTPResponse handleGetProject(const HTTPRequest& request);
    HTTPResponse handleCreateProject(const HTTPRequest& request);
    HTTPResponse handleUpdateProject(const HTTPRequest& request);
    HTTPResponse handleDeleteProject(const HTTPRequest& request);
    
    HTTPResponse handleGetCategories(const HTTPRequest& request);
    HTTPResponse handleGetCategory(const HTTPRequest& request);
    HTTPResponse handleCreateCategory(const HTTPRequest& request);
    HTTPResponse handleUpdateCategory(const HTTPRequest& request);
    HTTPResponse handleDeleteCategory(const HTTPRequest& request);
    
    HTTPResponse handleGetActivityLogs(const HTTPRequest& request);
    HTTPResponse handleGetActivityLog(const HTTPRequest& request);
    HTTPResponse handleCreateActivityLog(const HTTPRequest& request);
    
    HTTPResponse handleSearch(const HTTPRequest& request);
    HTTPResponse handleBatchOperation(const HTTPRequest& request);
    HTTPResponse handleHealthCheck(const HTTPRequest& request);
    
    // Helper methods
    std::string extractIdFromPath(const std::string& path, const std::string& prefix);
    HTTPResponse errorResponse(int statusCode, const std::string& message);
    HTTPResponse successResponse(const std::string& data, int statusCode = 200);
    
    // JSON serialization helpers
    std::string itemToJson(std::shared_ptr<Item> item);
    std::string containerToJson(std::shared_ptr<Container> container);
    std::string locationToJson(std::shared_ptr<Location> location);
    std::string projectToJson(std::shared_ptr<Project> project);
    std::string categoryToJson(std::shared_ptr<Category> category);
    std::string activityLogToJson(std::shared_ptr<ActivityLog> log);
    
    // JSON deserialization helpers
    std::shared_ptr<Item> jsonToItem(const std::string& json);
    std::shared_ptr<Container> jsonToContainer(const std::string& json);
    std::shared_ptr<Location> jsonToLocation(const std::string& json);
    std::shared_ptr<Project> jsonToProject(const std::string& json);
    std::shared_ptr<Category> jsonToCategory(const std::string& json);
    std::shared_ptr<ActivityLog> jsonToActivityLog(const std::string& json);
    
    // JSON array helpers
    std::string itemArrayToJson(const std::vector<std::shared_ptr<Item>>& items);
    std::string containerArrayToJson(const std::vector<std::shared_ptr<Container>>& containers);
    std::string locationArrayToJson(const std::vector<std::shared_ptr<Location>>& locations);
    std::string projectArrayToJson(const std::vector<std::shared_ptr<Project>>& projects);
    std::string categoryArrayToJson(const std::vector<std::shared_ptr<Category>>& categories);
    std::string activityLogArrayToJson(const std::vector<std::shared_ptr<ActivityLog>>& logs);
};

#endif // DATABASE_SERVER_H
