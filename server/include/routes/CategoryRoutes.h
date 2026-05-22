#ifndef CATEGORY_ROUTES_H
#define CATEGORY_ROUTES_H

#include <memory>
#include "../http/RouteHandler.h"
#include "../../include/Database.h"

/**
 * @brief Category API Routes
 * 
 * Handles all HTTP endpoints related to categories:
 * - GET /api/categories - List all categories
 * - GET /api/categories/:id - Get single category
 * - POST /api/categories - Create category
 * - PUT /api/categories/:id - Update category
 * - DELETE /api/categories/:id - Delete category
 */
class CategoryRoutes {
public:
    explicit CategoryRoutes(std::shared_ptr<IDatabase> database);
    ~CategoryRoutes() = default;
    
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

#endif // CATEGORY_ROUTES_H
