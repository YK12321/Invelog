#include "../include/routes/ContainerRoutes.h"
#include "../include/routes/RouteHelpers.h"
#include "../include/serialization/JSONSerializer.h"
#include "../include/serialization/JSONDeserializer.h"
#include "../../include/UUID.h"
#include <stdexcept>

ContainerRoutes::ContainerRoutes(std::shared_ptr<IDatabase> db) : database_(db) {}

HTTPResponse ContainerRoutes::handleGetAll(const HTTPRequest& req) {
    try {
        auto containers = database_->loadAllContainers();
        std::string json = JSONSerializer::serialize(containers);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse ContainerRoutes::handleGetById(const HTTPRequest& req) {
    try {
        UUID id = RouteHelpers::extractUUID(req.path);
        auto container = database_->loadContainer(id);
        
        if (!container) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("Container not found"));
        }
        
        std::string json = JSONSerializer::serialize(container);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse ContainerRoutes::handleCreate(const HTTPRequest& req) {
    try {
        auto container = JSONDeserializer::deserializeContainer(req.body);
        
        if (!database_->saveContainer(container)) {
            return HTTPResponse::internalError(JSONSerializer::serializeError("Failed to save container"));
        }
        
        std::string json = JSONSerializer::serialize(container);
        return HTTPResponse::created(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::badRequest(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse ContainerRoutes::handleUpdate(const HTTPRequest& req) {
    try {
        UUID id = RouteHelpers::extractUUID(req.path);
        auto container = database_->loadContainer(id);
        
        if (!container) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("Container not found"));
        }
        
        JSONDeserializer::updateContainer(container, req.body);
        
        if (!database_->saveContainer(container)) {
            return HTTPResponse::internalError(JSONSerializer::serializeError("Failed to update container"));
        }
        
        std::string json = JSONSerializer::serialize(container);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::badRequest(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse ContainerRoutes::handleDelete(const HTTPRequest& req) {
    try {
        UUID id = RouteHelpers::extractUUID(req.path);
        
        if (!database_->deleteContainer(id)) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("Container not found"));
        }
        
        return HTTPResponse::noContent();
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

std::string ContainerRoutes::extractIdFromPath(const std::string& path) {
    size_t lastSlash = path.find_last_of('/');
    if (lastSlash == std::string::npos) {
        throw std::runtime_error("Invalid path format");
    }
    
    return path.substr(lastSlash + 1);
}
