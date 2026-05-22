#ifndef CONTAINER_ROUTES_H
#define CONTAINER_ROUTES_H

#include <memory>
#include "../http/RouteHandler.h"
#include "../../include/Database.h"

/**
 * @brief Container API Routes
 * 
 * Handles all HTTP endpoints related to containers:
 * - GET /api/containers - List all containers
 * - GET /api/containers/:id - Get single container
 * - POST /api/containers - Create container
 * - PUT /api/containers/:id - Update container
 * - DELETE /api/containers/:id - Delete container
 */
class ContainerRoutes {
public:
    explicit ContainerRoutes(std::shared_ptr<IDatabase> database);
    ~ContainerRoutes() = default;
    
    // Route handlers
    HTTPResponse handleGetAll(const HTTPRequest& request);
    HTTPResponse handleGetById(const HTTPRequest& request);
    HTTPResponse handleCreate(const HTTPRequest& request);
    HTTPResponse handleUpdate(const HTTPRequest& request);
    HTTPResponse handleDelete(const HTTPRequest& request);
    
private:
    std::shared_ptr<IDatabase> database_;
    
    // Helper methods
    std::string extractIdFromPath(const std::string& path);
};

#endif // CONTAINER_ROUTES_H
