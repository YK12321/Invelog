#ifndef PROJECT_ROUTES_H
#define PROJECT_ROUTES_H

#include <memory>
#include "../http/RouteHandler.h"
#include "../../include/Database.h"

/**
 * @brief Project API Routes
 * 
 * Handles all HTTP endpoints related to projects:
 * - GET /api/projects - List all projects
 * - GET /api/projects/:id - Get single project
 * - POST /api/projects - Create project
 * - PUT /api/projects/:id - Update project
 * - DELETE /api/projects/:id - Delete project
 */
class ProjectRoutes {
public:
    explicit ProjectRoutes(std::shared_ptr<IDatabase> database);
    ~ProjectRoutes() = default;
    
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

#endif // PROJECT_ROUTES_H
