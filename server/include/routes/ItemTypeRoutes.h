#ifndef ITEMTYPEROUTES_H
#define ITEMTYPEROUTES_H

#include <memory>
#include "../http/RouteHandler.h"
#include "../../include/Database.h"
#include "../../include/InventoryManager.h"

/**
 * @brief ItemType API Routes
 * 
 * Handles all HTTP endpoints related to ItemTypes:
 * - POST /api/item-types - Create new ItemType
 * - GET /api/item-types - List all ItemTypes
 * - GET /api/item-types/:id - Get specific ItemType
 * - PUT /api/item-types/:id - Update ItemType
 * - DELETE /api/item-types/:id - Delete ItemType
 * - GET /api/item-types/:id/items - Get all items of type
 * - GET /api/item-types/:id/count - Count items of type
 * - GET /api/item-types/search - Search ItemTypes
 */
class ItemTypeRoutes {
public:
    ItemTypeRoutes(std::shared_ptr<IDatabase> database, std::shared_ptr<InventoryManager> inventoryManager);
    ~ItemTypeRoutes() = default;
    
    // Route handlers
    HTTPResponse handleCreate(const HTTPRequest& request);
    HTTPResponse handleGetAll(const HTTPRequest& request);
    HTTPResponse handleGetById(const HTTPRequest& request);
    HTTPResponse handleUpdate(const HTTPRequest& request);
    HTTPResponse handleDelete(const HTTPRequest& request);
    HTTPResponse handleGetItemsByType(const HTTPRequest& request);
    HTTPResponse handleGetItemCountByType(const HTTPRequest& request);
    HTTPResponse handleSearch(const HTTPRequest& request);
    
private:
    std::shared_ptr<IDatabase> database_;
    std::shared_ptr<InventoryManager> inventoryManager_;
    
    // Helper methods
    std::string extractIdFromPath(const std::string& path);
};

#endif // ITEMTYPEROUTES_H
