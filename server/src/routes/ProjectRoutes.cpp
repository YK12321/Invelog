#include "../include/routes/ProjectRoutes.h"
#include "../include/routes/RouteHelpers.h"
#include "../include/serialization/JSONSerializer.h"
#include "../include/serialization/JSONDeserializer.h"
#include "../../include/UUID.h"
#include <stdexcept>

ProjectRoutes::ProjectRoutes(std::shared_ptr<IDatabase> db) : database_(db) {}

HTTPResponse ProjectRoutes::handleGetAll(const HTTPRequest& req) {
    try {
        auto projects = database_->loadAllProjects();
        std::string json = JSONSerializer::serialize(projects);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse ProjectRoutes::handleGetById(const HTTPRequest& req) {
    try {
        UUID id = RouteHelpers::extractUUID(req.path);
        auto project = database_->loadProject(id);
        
        if (!project) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("Project not found"));
        }
        
        std::string json = JSONSerializer::serialize(project);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse ProjectRoutes::handleCreate(const HTTPRequest& req) {
    try {
        auto project = JSONDeserializer::deserializeProject(req.body);
        
        if (!database_->saveProject(project)) {
            return HTTPResponse::internalError(JSONSerializer::serializeError("Failed to save project"));
        }
        
        std::string json = JSONSerializer::serialize(project);
        return HTTPResponse::created(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::badRequest(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse ProjectRoutes::handleUpdate(const HTTPRequest& req) {
    try {
        UUID id = RouteHelpers::extractUUID(req.path);
        auto project = database_->loadProject(id);
        
        if (!project) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("Project not found"));
        }
        
        JSONDeserializer::updateProject(project, req.body);
        
        if (!database_->saveProject(project)) {
            return HTTPResponse::internalError(JSONSerializer::serializeError("Failed to update project"));
        }
        
        std::string json = JSONSerializer::serialize(project);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::badRequest(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse ProjectRoutes::handleDelete(const HTTPRequest& req) {
    try {
        UUID id = RouteHelpers::extractUUID(req.path);
        
        if (!database_->deleteProject(id)) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("Project not found"));
        }
        
        return HTTPResponse::noContent();
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

std::string ProjectRoutes::extractIdFromPath(const std::string& path) {
    size_t lastSlash = path.find_last_of('/');
    if (lastSlash == std::string::npos) {
        throw std::runtime_error("Invalid path format");
    }
    
    return path.substr(lastSlash + 1);
}
