#ifndef ITEM_ROUTES_H
#define ITEM_ROUTES_H

#include <memory>
#include "../http/RouteHandler.h"
#include "../../include/Database.h"

/**
 * @brief Item API Routes
 * 
 * Handles all HTTP endpoints related to items:
 * - GET /api/items - List all items
 * - GET /api/items/:id - Get single item
 * - POST /api/items - Create item
 * - PUT /api/items/:id - Update item
 * - DELETE /api/items/:id - Delete item
 * - POST /api/items/:id/move - Move item to container
 * - POST /api/items/:id/checkout - Check out item
 * - POST /api/items/:id/checkin - Check in item
 */
class ItemRoutes {
public:
    explicit ItemRoutes(std::shared_ptr<IDatabase> database);
    ~ItemRoutes() = default;
    
    // Route handlers
    HTTPResponse handleGetAll(const HTTPRequest& request);
    HTTPResponse handleGetById(const HTTPRequest& request);
    HTTPResponse handleCreate(const HTTPRequest& request);
    HTTPResponse handleUpdate(const HTTPRequest& request);
    HTTPResponse handleDelete(const HTTPRequest& request);
    HTTPResponse handleMove(const HTTPRequest& request);
    HTTPResponse handleCheckout(const HTTPRequest& request);
    HTTPResponse handleCheckin(const HTTPRequest& request);
    
private:
    std::shared_ptr<IDatabase> database_;
    
    // Helper methods
    std::string extractIdFromPath(const std::string& path);
};

#endif // ITEM_ROUTES_H
