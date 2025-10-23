#include "../include/routes/LocationRoutes.h"
#include "../include/routes/RouteHelpers.h"
#include "../include/serialization/JSONSerializer.h"
#include "../include/serialization/JSONDeserializer.h"
#include "../../include/UUID.h"
#include <stdexcept>

LocationRoutes::LocationRoutes(std::shared_ptr<IDatabase> db) : database_(db) {}

HTTPResponse LocationRoutes::handleGetAll(const HTTPRequest& req) {
    try {
        auto locations = database_->loadAllLocations();
        std::string json = JSONSerializer::serialize(locations);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse LocationRoutes::handleGetById(const HTTPRequest& req) {
    try {
        UUID id = RouteHelpers::extractUUID(req.path);
        auto location = database_->loadLocation(id);
        
        if (!location) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("Location not found"));
        }
        
        std::string json = JSONSerializer::serialize(location);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse LocationRoutes::handleCreate(const HTTPRequest& req) {
    try {
        auto location = JSONDeserializer::deserializeLocation(req.body);
        
        if (!database_->saveLocation(location)) {
            return HTTPResponse::internalError(JSONSerializer::serializeError("Failed to save location"));
        }
        
        std::string json = JSONSerializer::serialize(location);
        return HTTPResponse::created(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::badRequest(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse LocationRoutes::handleUpdate(const HTTPRequest& req) {
    try {
        UUID id = RouteHelpers::extractUUID(req.path);
        auto location = database_->loadLocation(id);
        
        if (!location) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("Location not found"));
        }
        
        JSONDeserializer::updateLocation(location, req.body);
        
        if (!database_->saveLocation(location)) {
            return HTTPResponse::internalError(JSONSerializer::serializeError("Failed to update location"));
        }
        
        std::string json = JSONSerializer::serialize(location);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::badRequest(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse LocationRoutes::handleDelete(const HTTPRequest& req) {
    try {
        UUID id = RouteHelpers::extractUUID(req.path);
        
        if (!database_->deleteLocation(id)) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("Location not found"));
        }
        
        return HTTPResponse::noContent();
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

std::string LocationRoutes::extractIdFromPath(const std::string& path) {
    size_t lastSlash = path.find_last_of('/');
    if (lastSlash == std::string::npos) {
        throw std::runtime_error("Invalid path format");
    }
    
    return path.substr(lastSlash + 1);
}
