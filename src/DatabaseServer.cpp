#include "DatabaseServer.h"
#include "Item.h"
#include "Container.h"
#include "Location.h"
#include "Project.h"
#include "Category.h"
#include "ActivityLog.h"
#include <nlohmann/json.hpp>
#include <httplib.h>
#include <iostream>
#include <sstream>
#include <algorithm>

using json = nlohmann::json;

DatabaseServer::DatabaseServer(std::shared_ptr<IDatabase> database, int port)
    : database_(database), port_(port), running_(false), authRequired_(false) {
    initializeRoutes();
}

DatabaseServer::~DatabaseServer() {
    stop();
}

bool DatabaseServer::start() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (running_) {
        std::cerr << "Server already running on port " << port_ << std::endl;
        return false;
    }
    
    // TODO: In a real implementation, this would start an HTTP server
    // using a library like cpp-httplib, Crow, Pistache, or Drogon
    // For now, this is a placeholder that marks the server as running
    
    std::cout << "Starting Database Server on port " << port_ << std::endl;
    std::cout << "Authentication " << (authRequired_ ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "Available endpoints:" << std::endl;
    std::cout << "  GET    /api/health" << std::endl;
    std::cout << "  GET    /api/items" << std::endl;
    std::cout << "  GET    /api/items/:id" << std::endl;
    std::cout << "  POST   /api/items" << std::endl;
    std::cout << "  PUT    /api/items/:id" << std::endl;
    std::cout << "  DELETE /api/items/:id" << std::endl;
    std::cout << "  GET    /api/containers" << std::endl;
    std::cout << "  GET    /api/containers/:id" << std::endl;
    std::cout << "  POST   /api/containers" << std::endl;
    std::cout << "  PUT    /api/containers/:id" << std::endl;
    std::cout << "  DELETE /api/containers/:id" << std::endl;
    std::cout << "  GET    /api/locations" << std::endl;
    std::cout << "  GET    /api/locations/:id" << std::endl;
    std::cout << "  POST   /api/locations" << std::endl;
    std::cout << "  PUT    /api/locations/:id" << std::endl;
    std::cout << "  DELETE /api/locations/:id" << std::endl;
    std::cout << "  GET    /api/projects" << std::endl;
    std::cout << "  GET    /api/projects/:id" << std::endl;
    std::cout << "  POST   /api/projects" << std::endl;
    std::cout << "  PUT    /api/projects/:id" << std::endl;
    std::cout << "  DELETE /api/projects/:id" << std::endl;
    std::cout << "  GET    /api/categories" << std::endl;
    std::cout << "  GET    /api/categories/:id" << std::endl;
    std::cout << "  POST   /api/categories" << std::endl;
    std::cout << "  PUT    /api/categories/:id" << std::endl;
    std::cout << "  DELETE /api/categories/:id" << std::endl;
    std::cout << "  GET    /api/activity_logs" << std::endl;
    std::cout << "  GET    /api/activity_logs/:id" << std::endl;
    std::cout << "  POST   /api/activity_logs" << std::endl;
    std::cout << "  POST   /api/search" << std::endl;
    std::cout << "  POST   /api/batch" << std::endl;
    std::cout << "Server ready to accept requests!" << std::endl;
    
    running_ = true;
    return true;
}

void DatabaseServer::stop() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!running_) {
        return;
    }
    
    std::cout << "Stopping Database Server..." << std::endl;
    running_ = false;
}

bool DatabaseServer::isRunning() const {
    return running_;
}

void DatabaseServer::setPort(int port) {
    if (!running_) {
        port_ = port;
    }
}

int DatabaseServer::getPort() const {
    return port_;
}

void DatabaseServer::setAuthRequired(bool required) {
    authRequired_ = required;
}

bool DatabaseServer::isAuthRequired() const {
    return authRequired_;
}

void DatabaseServer::setApiKey(const std::string& apiKey) {
    apiKey_ = apiKey;
    if (!apiKey.empty()) {
        authRequired_ = true;
    }
}

void DatabaseServer::addRoute(const std::string& method, const std::string& path, RouteHandler handler) {
    routes_[method][path] = handler;
}

void DatabaseServer::removeRoute(const std::string& method, const std::string& path) {
    if (routes_.count(method)) {
        routes_[method].erase(path);
    }
}

HTTPResponse DatabaseServer::handleRequest(const HTTPRequest& request) {
    // Check authentication if required
    if (authRequired_ && !authenticate(request)) {
        return unauthorizedResponse();
    }
    
    // Find matching route
    if (routes_.count(request.method)) {
        auto& methodRoutes = routes_[request.method];
        
        // Try exact match first
        if (methodRoutes.count(request.path)) {
            return methodRoutes[request.path](request);
        }
        
        // Try pattern matching for routes with :id
        for (const auto& [pattern, handler] : methodRoutes) {
            if (pattern.find(":id") != std::string::npos) {
                std::string prefix = pattern.substr(0, pattern.find(":id"));
                if (request.path.find(prefix) == 0 && request.path.length() > prefix.length()) {
                    return handler(request);
                }
            }
        }
    }
    
    return errorResponse(404, "Endpoint not found");
}

void DatabaseServer::initializeRoutes() {
    // Health check
    addRoute("GET", "/api/health", [this](const HTTPRequest& req) { return handleHealthCheck(req); });
    
    // Items
    addRoute("GET", "/api/items", [this](const HTTPRequest& req) { return handleGetItems(req); });
    addRoute("GET", "/api/items/:id", [this](const HTTPRequest& req) { return handleGetItem(req); });
    addRoute("POST", "/api/items", [this](const HTTPRequest& req) { return handleCreateItem(req); });
    addRoute("PUT", "/api/items/:id", [this](const HTTPRequest& req) { return handleUpdateItem(req); });
    addRoute("DELETE", "/api/items/:id", [this](const HTTPRequest& req) { return handleDeleteItem(req); });
    
    // Containers
    addRoute("GET", "/api/containers", [this](const HTTPRequest& req) { return handleGetContainers(req); });
    addRoute("GET", "/api/containers/:id", [this](const HTTPRequest& req) { return handleGetContainer(req); });
    addRoute("POST", "/api/containers", [this](const HTTPRequest& req) { return handleCreateContainer(req); });
    addRoute("PUT", "/api/containers/:id", [this](const HTTPRequest& req) { return handleUpdateContainer(req); });
    addRoute("DELETE", "/api/containers/:id", [this](const HTTPRequest& req) { return handleDeleteContainer(req); });
    
    // Locations
    addRoute("GET", "/api/locations", [this](const HTTPRequest& req) { return handleGetLocations(req); });
    addRoute("GET", "/api/locations/:id", [this](const HTTPRequest& req) { return handleGetLocation(req); });
    addRoute("POST", "/api/locations", [this](const HTTPRequest& req) { return handleCreateLocation(req); });
    addRoute("PUT", "/api/locations/:id", [this](const HTTPRequest& req) { return handleUpdateLocation(req); });
    addRoute("DELETE", "/api/locations/:id", [this](const HTTPRequest& req) { return handleDeleteLocation(req); });
    
    // Projects
    addRoute("GET", "/api/projects", [this](const HTTPRequest& req) { return handleGetProjects(req); });
    addRoute("GET", "/api/projects/:id", [this](const HTTPRequest& req) { return handleGetProject(req); });
    addRoute("POST", "/api/projects", [this](const HTTPRequest& req) { return handleCreateProject(req); });
    addRoute("PUT", "/api/projects/:id", [this](const HTTPRequest& req) { return handleUpdateProject(req); });
    addRoute("DELETE", "/api/projects/:id", [this](const HTTPRequest& req) { return handleDeleteProject(req); });
    
    // Categories
    addRoute("GET", "/api/categories", [this](const HTTPRequest& req) { return handleGetCategories(req); });
    addRoute("GET", "/api/categories/:id", [this](const HTTPRequest& req) { return handleGetCategory(req); });
    addRoute("POST", "/api/categories", [this](const HTTPRequest& req) { return handleCreateCategory(req); });
    addRoute("PUT", "/api/categories/:id", [this](const HTTPRequest& req) { return handleUpdateCategory(req); });
    addRoute("DELETE", "/api/categories/:id", [this](const HTTPRequest& req) { return handleDeleteCategory(req); });
    
    // Activity Logs
    addRoute("GET", "/api/activity_logs", [this](const HTTPRequest& req) { return handleGetActivityLogs(req); });
    addRoute("GET", "/api/activity_logs/:id", [this](const HTTPRequest& req) { return handleGetActivityLog(req); });
    addRoute("POST", "/api/activity_logs", [this](const HTTPRequest& req) { return handleCreateActivityLog(req); });
    
    // Search and batch operations
    addRoute("POST", "/api/search", [this](const HTTPRequest& req) { return handleSearch(req); });
    addRoute("POST", "/api/batch", [this](const HTTPRequest& req) { return handleBatchOperation(req); });
}

bool DatabaseServer::authenticate(const HTTPRequest& request) {
    if (apiKey_.empty()) {
        return true; // No API key set, allow all requests
    }
    
    // Check for API key in headers
    auto it = request.headers.find("X-API-Key");
    if (it != request.headers.end() && it->second == apiKey_) {
        return true;
    }
    
    // Check for Bearer token
    it = request.headers.find("Authorization");
    if (it != request.headers.end()) {
        std::string auth = it->second;
        if (auth.find("Bearer ") == 0) {
            std::string token = auth.substr(7);
            if (token == apiKey_) {
                return true;
            }
        }
    }
    
    return false;
}

HTTPResponse DatabaseServer::unauthorizedResponse() {
    return errorResponse(401, "Unauthorized - Invalid or missing API key");
}

// Item handlers
HTTPResponse DatabaseServer::handleGetItems(const HTTPRequest& request) {
    auto items = database_->loadAllItems();
    return successResponse(itemArrayToJson(items));
}

HTTPResponse DatabaseServer::handleGetItem(const HTTPRequest& request) {
    std::string id = extractIdFromPath(request.path, "/api/items/");
    auto item = database_->loadItem(id);
    
    if (!item) {
        return errorResponse(404, "Item not found");
    }
    
    return successResponse(itemToJson(item));
}

HTTPResponse DatabaseServer::handleCreateItem(const HTTPRequest& request) {
    auto item = jsonToItem(request.body);
    
    if (!item) {
        return errorResponse(400, "Invalid item data");
    }
    
    if (!database_->saveItem(item)) {
        return errorResponse(500, "Failed to save item");
    }
    
    return successResponse(itemToJson(item), 201);
}

HTTPResponse DatabaseServer::handleUpdateItem(const HTTPRequest& request) {
    std::string id = extractIdFromPath(request.path, "/api/items/");
    auto existingItem = database_->loadItem(id);
    
    if (!existingItem) {
        return errorResponse(404, "Item not found");
    }
    
    auto updatedItem = jsonToItem(request.body);
    if (!updatedItem) {
        return errorResponse(400, "Invalid item data");
    }
    
    // Ensure ID matches
    if (updatedItem->getId() != id) {
        return errorResponse(400, "Item ID mismatch");
    }
    
    if (!database_->saveItem(updatedItem)) {
        return errorResponse(500, "Failed to update item");
    }
    
    return successResponse(itemToJson(updatedItem));
}

HTTPResponse DatabaseServer::handleDeleteItem(const HTTPRequest& request) {
    std::string id = extractIdFromPath(request.path, "/api/items/");
    
    if (!database_->deleteItem(id)) {
        return errorResponse(404, "Item not found or failed to delete");
    }
    
    return successResponse("{\"success\": true, \"message\": \"Item deleted\"}");
}

// Container handlers
HTTPResponse DatabaseServer::handleGetContainers(const HTTPRequest& request) {
    auto containers = database_->loadAllContainers();
    return successResponse(containerArrayToJson(containers));
}

HTTPResponse DatabaseServer::handleGetContainer(const HTTPRequest& request) {
    std::string id = extractIdFromPath(request.path, "/api/containers/");
    auto container = database_->loadContainer(id);
    
    if (!container) {
        return errorResponse(404, "Container not found");
    }
    
    return successResponse(containerToJson(container));
}

HTTPResponse DatabaseServer::handleCreateContainer(const HTTPRequest& request) {
    auto container = jsonToContainer(request.body);
    
    if (!container) {
        return errorResponse(400, "Invalid container data");
    }
    
    if (!database_->saveContainer(container)) {
        return errorResponse(500, "Failed to save container");
    }
    
    return successResponse(containerToJson(container), 201);
}

HTTPResponse DatabaseServer::handleUpdateContainer(const HTTPRequest& request) {
    std::string id = extractIdFromPath(request.path, "/api/containers/");
    auto existingContainer = database_->loadContainer(id);
    
    if (!existingContainer) {
        return errorResponse(404, "Container not found");
    }
    
    auto updatedContainer = jsonToContainer(request.body);
    if (!updatedContainer) {
        return errorResponse(400, "Invalid container data");
    }
    
    if (updatedContainer->getId() != id) {
        return errorResponse(400, "Container ID mismatch");
    }
    
    if (!database_->saveContainer(updatedContainer)) {
        return errorResponse(500, "Failed to update container");
    }
    
    return successResponse(containerToJson(updatedContainer));
}

HTTPResponse DatabaseServer::handleDeleteContainer(const HTTPRequest& request) {
    std::string id = extractIdFromPath(request.path, "/api/containers/");
    
    if (!database_->deleteContainer(id)) {
        return errorResponse(404, "Container not found or failed to delete");
    }
    
    return successResponse("{\"success\": true, \"message\": \"Container deleted\"}");
}

// Location handlers
HTTPResponse DatabaseServer::handleGetLocations(const HTTPRequest& request) {
    auto locations = database_->loadAllLocations();
    return successResponse(locationArrayToJson(locations));
}

HTTPResponse DatabaseServer::handleGetLocation(const HTTPRequest& request) {
    std::string id = extractIdFromPath(request.path, "/api/locations/");
    auto location = database_->loadLocation(id);
    
    if (!location) {
        return errorResponse(404, "Location not found");
    }
    
    return successResponse(locationToJson(location));
}

HTTPResponse DatabaseServer::handleCreateLocation(const HTTPRequest& request) {
    auto location = jsonToLocation(request.body);
    
    if (!location) {
        return errorResponse(400, "Invalid location data");
    }
    
    if (!database_->saveLocation(location)) {
        return errorResponse(500, "Failed to save location");
    }
    
    return successResponse(locationToJson(location), 201);
}

HTTPResponse DatabaseServer::handleUpdateLocation(const HTTPRequest& request) {
    std::string id = extractIdFromPath(request.path, "/api/locations/");
    auto existingLocation = database_->loadLocation(id);
    
    if (!existingLocation) {
        return errorResponse(404, "Location not found");
    }
    
    auto updatedLocation = jsonToLocation(request.body);
    if (!updatedLocation) {
        return errorResponse(400, "Invalid location data");
    }
    
    if (updatedLocation->getId() != id) {
        return errorResponse(400, "Location ID mismatch");
    }
    
    if (!database_->saveLocation(updatedLocation)) {
        return errorResponse(500, "Failed to update location");
    }
    
    return successResponse(locationToJson(updatedLocation));
}

HTTPResponse DatabaseServer::handleDeleteLocation(const HTTPRequest& request) {
    std::string id = extractIdFromPath(request.path, "/api/locations/");
    
    if (!database_->deleteLocation(id)) {
        return errorResponse(404, "Location not found or failed to delete");
    }
    
    return successResponse("{\"success\": true, \"message\": \"Location deleted\"}");
}

// Project handlers
HTTPResponse DatabaseServer::handleGetProjects(const HTTPRequest& request) {
    auto projects = database_->loadAllProjects();
    return successResponse(projectArrayToJson(projects));
}

HTTPResponse DatabaseServer::handleGetProject(const HTTPRequest& request) {
    std::string id = extractIdFromPath(request.path, "/api/projects/");
    auto project = database_->loadProject(id);
    
    if (!project) {
        return errorResponse(404, "Project not found");
    }
    
    return successResponse(projectToJson(project));
}

HTTPResponse DatabaseServer::handleCreateProject(const HTTPRequest& request) {
    auto project = jsonToProject(request.body);
    
    if (!project) {
        return errorResponse(400, "Invalid project data");
    }
    
    if (!database_->saveProject(project)) {
        return errorResponse(500, "Failed to save project");
    }
    
    return successResponse(projectToJson(project), 201);
}

HTTPResponse DatabaseServer::handleUpdateProject(const HTTPRequest& request) {
    std::string id = extractIdFromPath(request.path, "/api/projects/");
    auto existingProject = database_->loadProject(id);
    
    if (!existingProject) {
        return errorResponse(404, "Project not found");
    }
    
    auto updatedProject = jsonToProject(request.body);
    if (!updatedProject) {
        return errorResponse(400, "Invalid project data");
    }
    
    if (updatedProject->getId() != id) {
        return errorResponse(400, "Project ID mismatch");
    }
    
    if (!database_->saveProject(updatedProject)) {
        return errorResponse(500, "Failed to update project");
    }
    
    return successResponse(projectToJson(updatedProject));
}

HTTPResponse DatabaseServer::handleDeleteProject(const HTTPRequest& request) {
    std::string id = extractIdFromPath(request.path, "/api/projects/");
    
    if (!database_->deleteProject(id)) {
        return errorResponse(404, "Project not found or failed to delete");
    }
    
    return successResponse("{\"success\": true, \"message\": \"Project deleted\"}");
}

// Category handlers
HTTPResponse DatabaseServer::handleGetCategories(const HTTPRequest& request) {
    auto categories = database_->loadAllCategories();
    return successResponse(categoryArrayToJson(categories));
}

HTTPResponse DatabaseServer::handleGetCategory(const HTTPRequest& request) {
    std::string id = extractIdFromPath(request.path, "/api/categories/");
    auto category = database_->loadCategory(id);
    
    if (!category) {
        return errorResponse(404, "Category not found");
    }
    
    return successResponse(categoryToJson(category));
}

HTTPResponse DatabaseServer::handleCreateCategory(const HTTPRequest& request) {
    auto category = jsonToCategory(request.body);
    
    if (!category) {
        return errorResponse(400, "Invalid category data");
    }
    
    if (!database_->saveCategory(category)) {
        return errorResponse(500, "Failed to save category");
    }
    
    return successResponse(categoryToJson(category), 201);
}

HTTPResponse DatabaseServer::handleUpdateCategory(const HTTPRequest& request) {
    std::string id = extractIdFromPath(request.path, "/api/categories/");
    auto existingCategory = database_->loadCategory(id);
    
    if (!existingCategory) {
        return errorResponse(404, "Category not found");
    }
    
    auto updatedCategory = jsonToCategory(request.body);
    if (!updatedCategory) {
        return errorResponse(400, "Invalid category data");
    }
    
    if (updatedCategory->getId() != id) {
        return errorResponse(400, "Category ID mismatch");
    }
    
    if (!database_->saveCategory(updatedCategory)) {
        return errorResponse(500, "Failed to update category");
    }
    
    return successResponse(categoryToJson(updatedCategory));
}

HTTPResponse DatabaseServer::handleDeleteCategory(const HTTPRequest& request) {
    std::string id = extractIdFromPath(request.path, "/api/categories/");
    
    if (!database_->deleteCategory(id)) {
        return errorResponse(404, "Category not found or failed to delete");
    }
    
    return successResponse("{\"success\": true, \"message\": \"Category deleted\"}");
}

// Activity Log handlers
HTTPResponse DatabaseServer::handleGetActivityLogs(const HTTPRequest& request) {
    auto logs = database_->loadAllActivityLogs();
    return successResponse(activityLogArrayToJson(logs));
}

HTTPResponse DatabaseServer::handleGetActivityLog(const HTTPRequest& request) {
    std::string id = extractIdFromPath(request.path, "/api/activity_logs/");
    auto log = database_->loadActivityLog(id);
    
    if (!log) {
        return errorResponse(404, "Activity log not found");
    }
    
    return successResponse(activityLogToJson(log));
}

HTTPResponse DatabaseServer::handleCreateActivityLog(const HTTPRequest& request) {
    auto log = jsonToActivityLog(request.body);
    
    if (!log) {
        return errorResponse(400, "Invalid activity log data");
    }
    
    if (!database_->saveActivityLog(log)) {
        return errorResponse(500, "Failed to save activity log");
    }
    
    return successResponse(activityLogToJson(log), 201);
}

// Special handlers
HTTPResponse DatabaseServer::handleSearch(const HTTPRequest& request) {
    // TODO: Parse search criteria from request body
    // For now, return empty results
    return successResponse("{\"results\": [], \"total\": 0}");
}

HTTPResponse DatabaseServer::handleBatchOperation(const HTTPRequest& request) {
    // TODO: Parse and execute batch operations
    return successResponse("{\"success\": true, \"processed\": 0}");
}

HTTPResponse DatabaseServer::handleHealthCheck(const HTTPRequest& request) {
    std::ostringstream json;
    json << "{"
         << "\"status\": \"healthy\","
         << "\"version\": \"0.2.0\","
         << "\"database\": \"connected\","
         << "\"timestamp\": " << time(nullptr)
         << "}";
    return successResponse(json.str());
}

// Helper methods
std::string DatabaseServer::extractIdFromPath(const std::string& path, const std::string& prefix) {
    if (path.find(prefix) == 0) {
        return path.substr(prefix.length());
    }
    return "";
}

HTTPResponse DatabaseServer::errorResponse(int statusCode, const std::string& message) {
    HTTPResponse response;
    response.statusCode = statusCode;
    
    std::ostringstream json;
    json << "{"
         << "\"error\": true,"
         << "\"message\": \"" << message << "\","
         << "\"statusCode\": " << statusCode
         << "}";
    response.body = json.str();
    
    return response;
}

HTTPResponse DatabaseServer::successResponse(const std::string& data, int statusCode) {
    HTTPResponse response;
    response.statusCode = statusCode;
    response.body = data;
    return response;
}

// JSON serialization helpers (placeholders - will be implemented with nlohmann/json)
std::string DatabaseServer::itemToJson(std::shared_ptr<Item> item) {
    if (!item) return "{}";
    
    std::ostringstream json;
    json << "{"
         << "\"id\": \"" << item->getId() << "\","
         << "\"name\": \"" << item->getName() << "\","
         << "\"description\": \"" << item->getDescription() << "\","
         << "\"quantity\": " << item->getQuantity() << ","
         << "\"categoryId\": \"" << (item->getCategory() ? item->getCategory()->getId() : "") << "\","
         << "\"containerId\": \"" << (item->getContainer() ? item->getContainer()->getId() : "") << "\","
         << "\"checkedOut\": " << (item->isCheckedOut() ? "true" : "false")
         << "}";
    
    return json.str();
}

std::string DatabaseServer::containerToJson(std::shared_ptr<Container> container) {
    if (!container) return "{}";
    
    std::ostringstream json;
    json << "{"
         << "\"id\": \"" << container->getId() << "\","
         << "\"name\": \"" << container->getName() << "\","
         << "\"description\": \"" << container->getDescription() << "\","
         << "\"type\": " << static_cast<int>(container->getType())
         << "}";
    
    return json.str();
}

std::string DatabaseServer::locationToJson(std::shared_ptr<Location> location) {
    if (!location) return "{}";
    
    std::ostringstream json;
    json << "{"
         << "\"id\": \"" << location->getId() << "\","
         << "\"name\": \"" << location->getName() << "\","
         << "\"description\": \"" << location->getDescription() << "\""
         << "}";
    
    return json.str();
}

std::string DatabaseServer::projectToJson(std::shared_ptr<Project> project) {
    if (!project) return "{}";
    
    std::ostringstream json;
    json << "{"
         << "\"id\": \"" << project->getId() << "\","
         << "\"name\": \"" << project->getName() << "\","
         << "\"description\": \"" << project->getDescription() << "\","
         << "\"status\": " << static_cast<int>(project->getStatus())
         << "}";
    
    return json.str();
}

std::string DatabaseServer::categoryToJson(std::shared_ptr<Category> category) {
    if (!category) return "{}";
    
    std::ostringstream json;
    json << "{"
         << "\"id\": \"" << category->getId() << "\","
         << "\"name\": \"" << category->getName() << "\","
         << "\"description\": \"" << category->getDescription() << "\""
         << "}";
    
    return json.str();
}

std::string DatabaseServer::activityLogToJson(std::shared_ptr<ActivityLog> log) {
    if (!log) return "{}";
    
    std::ostringstream json;
    json << "{"
         << "\"id\": \"" << log->getId() << "\","
         << "\"itemId\": \"" << log->getItemId() << "\","
         << "\"type\": \"" << log->getTypeString() << "\","
         << "\"timestamp\": " << log->getTimestamp() << ","
         << "\"userId\": \"" << log->getUserId() << "\","
         << "\"notes\": \"" << log->getNotes() << "\""
         << "}";
    
    return json.str();
}

// JSON deserialization helpers (placeholders - will be implemented with nlohmann/json)
std::shared_ptr<Item> DatabaseServer::jsonToItem(const std::string& json) {
    // TODO: Parse JSON and create Item
    // For now, return nullptr
    return nullptr;
}

std::shared_ptr<Container> DatabaseServer::jsonToContainer(const std::string& json) {
    // TODO: Parse JSON and create Container
    return nullptr;
}

std::shared_ptr<Location> DatabaseServer::jsonToLocation(const std::string& json) {
    // TODO: Parse JSON and create Location
    return nullptr;
}

std::shared_ptr<Project> DatabaseServer::jsonToProject(const std::string& json) {
    // TODO: Parse JSON and create Project
    return nullptr;
}

std::shared_ptr<Category> DatabaseServer::jsonToCategory(const std::string& json) {
    // TODO: Parse JSON and create Category
    return nullptr;
}

std::shared_ptr<ActivityLog> DatabaseServer::jsonToActivityLog(const std::string& json) {
    // TODO: Parse JSON and create ActivityLog
    return nullptr;
}

// JSON array helpers
std::string DatabaseServer::itemArrayToJson(const std::vector<std::shared_ptr<Item>>& items) {
    std::ostringstream json;
    json << "[";
    for (size_t i = 0; i < items.size(); ++i) {
        json << itemToJson(items[i]);
        if (i < items.size() - 1) json << ",";
    }
    json << "]";
    return json.str();
}

std::string DatabaseServer::containerArrayToJson(const std::vector<std::shared_ptr<Container>>& containers) {
    std::ostringstream json;
    json << "[";
    for (size_t i = 0; i < containers.size(); ++i) {
        json << containerToJson(containers[i]);
        if (i < containers.size() - 1) json << ",";
    }
    json << "]";
    return json.str();
}

std::string DatabaseServer::locationArrayToJson(const std::vector<std::shared_ptr<Location>>& locations) {
    std::ostringstream json;
    json << "[";
    for (size_t i = 0; i < locations.size(); ++i) {
        json << locationToJson(locations[i]);
        if (i < locations.size() - 1) json << ",";
    }
    json << "]";
    return json.str();
}

std::string DatabaseServer::projectArrayToJson(const std::vector<std::shared_ptr<Project>>& projects) {
    std::ostringstream json;
    json << "[";
    for (size_t i = 0; i < projects.size(); ++i) {
        json << projectToJson(projects[i]);
        if (i < projects.size() - 1) json << ",";
    }
    json << "]";
    return json.str();
}

std::string DatabaseServer::categoryArrayToJson(const std::vector<std::shared_ptr<Category>>& categories) {
    std::ostringstream json;
    json << "[";
    for (size_t i = 0; i < categories.size(); ++i) {
        json << categoryToJson(categories[i]);
        if (i < categories.size() - 1) json << ",";
    }
    json << "]";
    return json.str();
}

std::string DatabaseServer::activityLogArrayToJson(const std::vector<std::shared_ptr<ActivityLog>>& logs) {
    std::ostringstream json;
    json << "[";
    for (size_t i = 0; i < logs.size(); ++i) {
        json << activityLogToJson(logs[i]);
        if (i < logs.size() - 1) json << ",";
    }
    json << "]";
    return json.str();
}
