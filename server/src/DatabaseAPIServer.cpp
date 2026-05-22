#include "../include/DatabaseAPIServer.h"
#include "../include/serialization/JSONSerializer.h"
#include "../../include/InventoryManager.h"
#include <nlohmann/json.hpp>
#include <iostream>

DatabaseAPIServer::DatabaseAPIServer(std::shared_ptr<IDatabase> db, const ServerConfig& config)
    : database(db), config(config), httpServer(std::make_unique<HTTPServer>(config.port)) {
    
    // Initialize inventory manager
    inventoryManager = std::make_shared<InventoryManager>(database);
    inventoryManager->initialize();
    
    // Initialize authenticator if auth is required
    if (config.authRequired && !config.apiKey.empty()) {
        authenticator = std::make_unique<Authenticator>();
        authenticator->setAuthRequired(true);
        authenticator->setApiKey(config.apiKey);
    }
    
    // Initialize route handlers
    itemRoutes = std::make_unique<ItemRoutes>(database);
    itemTypeRoutes = std::make_unique<ItemTypeRoutes>(database, inventoryManager);
    containerRoutes = std::make_unique<ContainerRoutes>(database);
    locationRoutes = std::make_unique<LocationRoutes>(database);
    projectRoutes = std::make_unique<ProjectRoutes>(database);
    categoryRoutes = std::make_unique<CategoryRoutes>(database);
    activityLogRoutes = std::make_unique<ActivityLogRoutes>(database);
    searchRoutes = std::make_unique<SearchRoutes>(database, inventoryManager);
}

DatabaseAPIServer::~DatabaseAPIServer() {
    stop();
}

void DatabaseAPIServer::start() {
    registerAllRoutes();
    
    // TODO: Implement middleware support in HTTPServer
    // if (config.authRequired && authenticator) {
    //     setupAuthenticationMiddleware();
    // }
    
    httpServer->start();
    std::cout << "Database API Server started on port " << config.port << std::endl;
    std::cout << "Authentication: " << (config.authRequired ? "Enabled" : "Disabled") << std::endl;
    std::cout << "CORS: " << (config.enableCORS ? "Enabled" : "Disabled") << std::endl;
}

void DatabaseAPIServer::stop() {
    httpServer->stop();
    std::cout << "Database API Server stopped" << std::endl;
}

bool DatabaseAPIServer::isRunning() const {
    return httpServer->isRunning();
}

void DatabaseAPIServer::registerAllRoutes() {
    // Health check endpoint
    httpServer->addRoute("GET", "/api/health", [](const HTTPRequest& req) {
        nlohmann::json j;
        j["status"] = "healthy";
        j["timestamp"] = std::time(nullptr);
        return HTTPResponse::ok(j.dump(), "application/json");
    });
    
    // Item routes
    httpServer->addRoute("GET", "/api/items", 
        [this](const HTTPRequest& req) { return itemRoutes->handleGetAll(req); });
    httpServer->addRoute("GET", "/api/items/.*", 
        [this](const HTTPRequest& req) { return itemRoutes->handleGetById(req); });
    httpServer->addRoute("POST", "/api/items", 
        [this](const HTTPRequest& req) { return itemRoutes->handleCreate(req); });
    httpServer->addRoute("PUT", "/api/items/.*", 
        [this](const HTTPRequest& req) { return itemRoutes->handleUpdate(req); });
    httpServer->addRoute("DELETE", "/api/items/.*", 
        [this](const HTTPRequest& req) { return itemRoutes->handleDelete(req); });
    
    // Container routes
    httpServer->addRoute("GET", "/api/containers", 
        [this](const HTTPRequest& req) { return containerRoutes->handleGetAll(req); });
    httpServer->addRoute("GET", "/api/containers/.*", 
        [this](const HTTPRequest& req) { return containerRoutes->handleGetById(req); });
    httpServer->addRoute("POST", "/api/containers", 
        [this](const HTTPRequest& req) { return containerRoutes->handleCreate(req); });
    httpServer->addRoute("PUT", "/api/containers/.*", 
        [this](const HTTPRequest& req) { return containerRoutes->handleUpdate(req); });
    httpServer->addRoute("DELETE", "/api/containers/.*", 
        [this](const HTTPRequest& req) { return containerRoutes->handleDelete(req); });
    // TODO: Implement handleGetItems and handleGetSubContainers in ContainerRoutes
    // httpServer->addRoute("GET", "/api/containers/.*/items", 
    //     [this](const HTTPRequest& req) { return containerRoutes->handleGetItems(req); });
    // httpServer->addRoute("GET", "/api/containers/.*/subcontainers", 
    //     [this](const HTTPRequest& req) { return containerRoutes->handleGetSubContainers(req); });
    
    // Location routes
    httpServer->addRoute("GET", "/api/locations", 
        [this](const HTTPRequest& req) { return locationRoutes->handleGetAll(req); });
    httpServer->addRoute("GET", "/api/locations/.*", 
        [this](const HTTPRequest& req) { return locationRoutes->handleGetById(req); });
    httpServer->addRoute("POST", "/api/locations", 
        [this](const HTTPRequest& req) { return locationRoutes->handleCreate(req); });
    httpServer->addRoute("PUT", "/api/locations/.*", 
        [this](const HTTPRequest& req) { return locationRoutes->handleUpdate(req); });
    httpServer->addRoute("DELETE", "/api/locations/.*", 
        [this](const HTTPRequest& req) { return locationRoutes->handleDelete(req); });
    // TODO: Implement handleGetContainers in LocationRoutes
    // httpServer->addRoute("GET", "/api/locations/.*/containers", 
    //     [this](const HTTPRequest& req) { return locationRoutes->handleGetContainers(req); });
    
    // Project routes
    httpServer->addRoute("GET", "/api/projects", 
        [this](const HTTPRequest& req) { return projectRoutes->handleGetAll(req); });
    httpServer->addRoute("GET", "/api/projects/.*", 
        [this](const HTTPRequest& req) { return projectRoutes->handleGetById(req); });
    httpServer->addRoute("POST", "/api/projects", 
        [this](const HTTPRequest& req) { return projectRoutes->handleCreate(req); });
    httpServer->addRoute("PUT", "/api/projects/.*", 
        [this](const HTTPRequest& req) { return projectRoutes->handleUpdate(req); });
    httpServer->addRoute("DELETE", "/api/projects/.*", 
        [this](const HTTPRequest& req) { return projectRoutes->handleDelete(req); });
    // TODO: Implement handleGetContainers in ProjectRoutes
    // httpServer->addRoute("GET", "/api/projects/.*/containers", 
    //     [this](const HTTPRequest& req) { return projectRoutes->handleGetContainers(req); });
    
    // Category routes
    httpServer->addRoute("GET", "/api/categories", 
        [this](const HTTPRequest& req) { return categoryRoutes->handleGetAll(req); });
    httpServer->addRoute("GET", "/api/categories/.*", 
        [this](const HTTPRequest& req) { return categoryRoutes->handleGetById(req); });
    httpServer->addRoute("POST", "/api/categories", 
        [this](const HTTPRequest& req) { return categoryRoutes->handleCreate(req); });
    httpServer->addRoute("PUT", "/api/categories/.*", 
        [this](const HTTPRequest& req) { return categoryRoutes->handleUpdate(req); });
    httpServer->addRoute("DELETE", "/api/categories/.*", 
        [this](const HTTPRequest& req) { return categoryRoutes->handleDelete(req); });
    
    // Activity log routes
    httpServer->addRoute("GET", "/api/logs", 
        [this](const HTTPRequest& req) { return activityLogRoutes->handleGetRecent(req); });
    httpServer->addRoute("GET", "/api/logs/item/.*", 
        [this](const HTTPRequest& req) { return activityLogRoutes->handleGetByItemId(req); });
    // TODO: Implement handleGetByUserId and handleGetByDateRange in ActivityLogRoutes
    // httpServer->addRoute("GET", "/api/logs/user", 
    //     [this](const HTTPRequest& req) { return activityLogRoutes->handleGetByUserId(req); });
    // httpServer->addRoute("GET", "/api/logs/date-range", 
    //     [this](const HTTPRequest& req) { return activityLogRoutes->handleGetByDateRange(req); });
    
    // Search routes
    httpServer->addRoute("POST", "/api/search", 
        [this](const HTTPRequest& req) { return searchRoutes->handleAdvancedSearch(req); });
    httpServer->addRoute("GET", "/api/search/items", 
        [this](const HTTPRequest& req) { return searchRoutes->handleSimpleSearch(req); });
    httpServer->addRoute("GET", "/api/search/category/.*", 
        [this](const HTTPRequest& req) { return searchRoutes->handleSearchByCategory(req); });
    httpServer->addRoute("GET", "/api/search/location/.*", 
        [this](const HTTPRequest& req) { return searchRoutes->handleSearchByLocation(req); });
    httpServer->addRoute("GET", "/api/search/project/.*", 
        [this](const HTTPRequest& req) { return searchRoutes->handleSearchByProject(req); });
    httpServer->addRoute("GET", "/api/search/container/.*", 
        [this](const HTTPRequest& req) { return searchRoutes->handleSearchByContainer(req); });
    
    // ItemType routes
    httpServer->addRoute("POST", "/api/item-types", 
        [this](const HTTPRequest& req) { return itemTypeRoutes->handleCreate(req); });
    httpServer->addRoute("GET", "/api/item-types", 
        [this](const HTTPRequest& req) { return itemTypeRoutes->handleGetAll(req); });
    httpServer->addRoute("GET", "/api/item-types/search", 
        [this](const HTTPRequest& req) { return itemTypeRoutes->handleSearch(req); });
    httpServer->addRoute("GET", "/api/item-types/.*/items", 
        [this](const HTTPRequest& req) { return itemTypeRoutes->handleGetItemsByType(req); });
    httpServer->addRoute("GET", "/api/item-types/.*/count", 
        [this](const HTTPRequest& req) { return itemTypeRoutes->handleGetItemCountByType(req); });
    httpServer->addRoute("GET", "/api/item-types/.*", 
        [this](const HTTPRequest& req) { return itemTypeRoutes->handleGetById(req); });
    httpServer->addRoute("PUT", "/api/item-types/.*", 
        [this](const HTTPRequest& req) { return itemTypeRoutes->handleUpdate(req); });
    httpServer->addRoute("DELETE", "/api/item-types/.*", 
        [this](const HTTPRequest& req) { return itemTypeRoutes->handleDelete(req); });
}

void DatabaseAPIServer::setupAuthenticationMiddleware() {
    // TODO: Implement middleware support in HTTPServer
    /*
    httpServer->setMiddleware([this](const HTTPRequest& req, HTTPResponse& res) {
        // Skip auth for health check
        if (req.path == "/api/health") {
            return true;
        }
        
        if (!authenticator->authenticate(req)) {
            res = HTTPResponse::unauthorized(JSONSerializer::serializeError("Authentication required"));
            return false;
        }
        
        return true;
    });
    */
}
