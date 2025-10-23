#include "../include/routes/CategoryRoutes.h"
#include "../include/routes/RouteHelpers.h"
#include "../include/serialization/JSONSerializer.h"
#include "../include/serialization/JSONDeserializer.h"
#include "../../include/UUID.h"
#include <stdexcept>

CategoryRoutes::CategoryRoutes(std::shared_ptr<IDatabase> db) : database_(db) {}

HTTPResponse CategoryRoutes::handleGetAll(const HTTPRequest& req) {
    try {
        auto categories = database_->loadAllCategories();
        std::string json = JSONSerializer::serialize(categories);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse CategoryRoutes::handleGetById(const HTTPRequest& req) {
    try {
        UUID id = RouteHelpers::extractUUID(req.path);
        auto category = database_->loadCategory(id);
        
        if (!category) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("Category not found"));
        }
        
        std::string json = JSONSerializer::serialize(category);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse CategoryRoutes::handleCreate(const HTTPRequest& req) {
    try {
        auto category = JSONDeserializer::deserializeCategory(req.body);
        
        if (!database_->saveCategory(category)) {
            return HTTPResponse::internalError(JSONSerializer::serializeError("Failed to save category"));
        }
        
        std::string json = JSONSerializer::serialize(category);
        return HTTPResponse::created(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::badRequest(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse CategoryRoutes::handleUpdate(const HTTPRequest& req) {
    try {
        UUID id = RouteHelpers::extractUUID(req.path);
        auto category = database_->loadCategory(id);
        
        if (!category) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("Category not found"));
        }
        
        JSONDeserializer::updateCategory(category, req.body);
        
        if (!database_->saveCategory(category)) {
            return HTTPResponse::internalError(JSONSerializer::serializeError("Failed to update category"));
        }
        
        std::string json = JSONSerializer::serialize(category);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::badRequest(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse CategoryRoutes::handleDelete(const HTTPRequest& req) {
    try {
        UUID id = RouteHelpers::extractUUID(req.path);
        
        if (!database_->deleteCategory(id)) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("Category not found"));
        }
        
        return HTTPResponse::noContent();
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

std::string CategoryRoutes::extractIdFromPath(const std::string& path) {
    size_t lastSlash = path.find_last_of('/');
    if (lastSlash == std::string::npos) {
        throw std::runtime_error("Invalid path format");
    }
    
    return path.substr(lastSlash + 1);
}
