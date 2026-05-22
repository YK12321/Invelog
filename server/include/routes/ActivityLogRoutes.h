#ifndef ACTIVITY_LOG_ROUTES_H
#define ACTIVITY_LOG_ROUTES_H

#include <memory>
#include "../http/RouteHandler.h"
#include "../../include/Database.h"

/**
 * @brief Activity Log API Routes
 * 
 * Handles all HTTP endpoints related to activity logs:
 * - GET /api/activity - List recent activity logs
 * - GET /api/activity/:id - Get single activity log
 * - GET /api/activity/item/:itemId - Get logs for specific item
 */
class ActivityLogRoutes {
public:
    explicit ActivityLogRoutes(std::shared_ptr<IDatabase> database);
    ~ActivityLogRoutes() = default;
    
    // Route handlers
    HTTPResponse handleGetRecent(const HTTPRequest& request);
    HTTPResponse handleGetById(const HTTPRequest& request);
    HTTPResponse handleGetByItemId(const HTTPRequest& request);
    
private:
    std::shared_ptr<IDatabase> database_;
    
    // Helper methods
    std::string extractIdFromPath(const std::string& path);
    int extractLimitFromQuery(const HTTPRequest& request, int defaultLimit = 100);
};

#endif // ACTIVITY_LOG_ROUTES_H
