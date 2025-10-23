#include "../include/routes/ProjectRoutes.h"
#include "../include/serialization/JSONSerializer.h"
#include "../include/serialization/JSONDeserializer.h"
#include "../../include/UUID.h"
#include <stdexcept>

ProjectRoutes::ProjectRoutes(std::shared_ptr<Database> db) : database(db) {}

HTTPResponse ProjectRoutes::handleGetAll(const HTTPRequest& req) {
    try {
        auto projects = database->loadAllProjects();
        std::string json = JSONSerializer::serialize(projects);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse ProjectRoutes::handleGetById(const HTTPRequest& req) {
    try {
        UUID id = extractUUID(req.path);
        auto project = database->loadProject(id);
        
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
        
        if (!database->saveProject(project)) {
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
        UUID id = extractUUID(req.path);
        auto project = database->loadProject(id);
        
        if (!project) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("Project not found"));
        }
        
        JSONDeserializer::updateProject(project, req.body);
        
        if (!database->saveProject(project)) {
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
        UUID id = extractUUID(req.path);
        
        if (!database->deleteProject(id)) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("Project not found"));
        }
        
        return HTTPResponse::noContent();
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

HTTPResponse ProjectRoutes::handleGetContainers(const HTTPRequest& req) {
    try {
        UUID id = extractUUID(req.path);
        auto project = database->loadProject(id);
        
        if (!project) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("Project not found"));
        }
        
        std::string json = JSONSerializer::serialize(project->getContainers());
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

UUID ProjectRoutes::extractUUID(const std::string& path) {
    size_t lastSlash = path.find_last_of('/');
    if (lastSlash == std::string::npos) {
        throw std::runtime_error("Invalid path format");
    }
    
    std::string uuidStr = path.substr(lastSlash + 1);
    return UUID::fromString(uuidStr);
}
