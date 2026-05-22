#ifndef SEARCH_ROUTES_H
#define SEARCH_ROUTES_H

#include <memory>
#include "../http/RouteHandler.h"
#include "../../include/Database.h"
#include "../../include/InventoryManager.h"

/**
 * @brief Search API Routes
 * 
 * Handles all HTTP endpoints related to searching:
 * - POST /api/search - Advanced search with filters
 * - GET /api/search/items - Simple item search by query
 * - GET /api/search/category/:id - Search items by category
 * - GET /api/search/location/:id - Search items in location
 * - GET /api/search/project/:id - Search items in project
 * - GET /api/search/container/:id - Search items in container
 */
class SearchRoutes {
public:
    SearchRoutes(std::shared_ptr<IDatabase> database, std::shared_ptr<InventoryManager> inventoryManager);
    ~SearchRoutes() = default;
    
    // Route handlers
    HTTPResponse handleAdvancedSearch(const HTTPRequest& request);
    HTTPResponse handleSimpleSearch(const HTTPRequest& request);
    HTTPResponse handleSearchByCategory(const HTTPRequest& request);
    HTTPResponse handleSearchByLocation(const HTTPRequest& request);
    HTTPResponse handleSearchByProject(const HTTPRequest& request);
    HTTPResponse handleSearchByContainer(const HTTPRequest& request);
    
private:
    std::shared_ptr<IDatabase> database_;
    std::shared_ptr<InventoryManager> inventoryManager_;
    
    // Helper methods
    std::string extractIdFromPath(const std::string& path);
};

#endif // SEARCH_ROUTES_H
