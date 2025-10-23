#include "../include/routes/ActivityLogRoutes.h"
#include "../include/serialization/JSONSerializer.h"
#include "../../include/UUID.h"
#include <stdexcept>

ActivityLogRoutes::ActivityLogRoutes(std::shared_ptr<Database> db) : database(db) {}

HTTPResponse ActivityLogRoutes::handleGetAll(const HTTPRequest& req) {
    try {
        auto logs = database->loadAllActivityLogs();
        std::string json = JSONSerializer::serialize(logs);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse ActivityLogRoutes::handleGetById(const HTTPRequest& req) {
    try {
        UUID id = extractUUID(req.path);
        auto log = database->loadActivityLog(id);
        
        if (!log) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("Activity log not found"));
        }
        
        std::string json = JSONSerializer::serialize(log);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse ActivityLogRoutes::handleGetByItemId(const HTTPRequest& req) {
    try {
        UUID itemId = extractUUID(req.path);
        auto item = database->loadItem(itemId);
        
        if (!item) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("Item not found"));
        }
        
        auto logs = database->getActivityLogsForItem(itemId);
        std::string json = JSONSerializer::serialize(logs);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse ActivityLogRoutes::handleGetByUserId(const HTTPRequest& req) {
    try {
        // Extract user_id from query parameters
        if (!req.hasQueryParam("user_id")) {
            return HTTPResponse::badRequest(JSONSerializer::serializeError("user_id query parameter required"));
        }
        
        std::string userId = req.getQueryParam("user_id");
        auto logs = database->getActivityLogsByUser(userId);
        std::string json = JSONSerializer::serialize(logs);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse ActivityLogRoutes::handleGetByDateRange(const HTTPRequest& req) {
    try {
        // Extract start_date and end_date from query parameters
        if (!req.hasQueryParam("start_date") || !req.hasQueryParam("end_date")) {
            return HTTPResponse::badRequest(
                JSONSerializer::serializeError("start_date and end_date query parameters required")
            );
        }
        
        std::string startDate = req.getQueryParam("start_date");
        std::string endDate = req.getQueryParam("end_date");
        
        auto logs = database->getActivityLogsByDateRange(startDate, endDate);
        std::string json = JSONSerializer::serialize(logs);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

UUID ActivityLogRoutes::extractUUID(const std::string& path) {
    size_t lastSlash = path.find_last_of('/');
    if (lastSlash == std::string::npos) {
        throw std::runtime_error("Invalid path format");
    }
    
    std::string uuidStr = path.substr(lastSlash + 1);
    return UUID::fromString(uuidStr);
}
