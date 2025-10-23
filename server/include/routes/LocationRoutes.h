#ifndef LOCATION_ROUTES_H
#define LOCATION_ROUTES_H

#include <memory>
#include "../http/RouteHandler.h"
#include "../../include/Database.h"

/**
 * @brief Location API Routes
 * 
 * Handles all HTTP endpoints related to locations:
 * - GET /api/locations - List all locations
 * - GET /api/locations/:id - Get single location
 * - POST /api/locations - Create location
 * - PUT /api/locations/:id - Update location
 * - DELETE /api/locations/:id - Delete location
 */
class LocationRoutes {
public:
    explicit LocationRoutes(std::shared_ptr<IDatabase> database);
    ~LocationRoutes() = default;
    
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

#endif // LOCATION_ROUTES_H
