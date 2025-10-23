#include "../include/routes/ActivityLogRoutes.h"
#include "../include/routes/RouteHelpers.h"
#include "../include/serialization/JSONSerializer.h"
#include "../../include/UUID.h"
#include <stdexcept>

ActivityLogRoutes::ActivityLogRoutes(std::shared_ptr<IDatabase> db) : database_(db) {}

HTTPResponse ActivityLogRoutes::handleGetRecent(const HTTPRequest& req) {
    try {
        auto logs = database_->loadRecentActivityLogs(100);
        std::string json = JSONSerializer::serialize(logs);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse ActivityLogRoutes::handleGetById(const HTTPRequest& req) {
    try {
        UUID id = RouteHelpers::extractUUID(req.path);
        // Note: IDatabase interface doesn't have loadActivityLog by id
        // We'll need to implement this or return not implemented
        return HTTPResponse::notImplemented(JSONSerializer::serializeError("Get activity log by ID not implemented"));
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse ActivityLogRoutes::handleGetByItemId(const HTTPRequest& req) {
    try {
        UUID itemId = RouteHelpers::extractUUID(req.path);
        auto item = database_->loadItem(itemId);
        
        if (!item) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("Item not found"));
        }
        
        auto logs = database_->loadActivityLogsForItem(itemId);
        std::string json = JSONSerializer::serialize(logs);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

std::string ActivityLogRoutes::extractIdFromPath(const std::string& path) {
    size_t lastSlash = path.find_last_of('/');
    if (lastSlash == std::string::npos) {
        throw std::runtime_error("Invalid path format");
    }
    
    return path.substr(lastSlash + 1);
}
