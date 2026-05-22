#include "../include/routes/ItemRoutes.h"
#include "../include/serialization/JSONSerializer.h"
#include "../include/serialization/JSONDeserializer.h"
#include "../../include/Item.h"
#include "../../include/UUID.h"
#include <algorithm>

ItemRoutes::ItemRoutes(std::shared_ptr<IDatabase> database)
    : database_(database) {}

HTTPResponse ItemRoutes::handleGetAll(const HTTPRequest& request) {
    try {
        auto items = database_->loadAllItems();
        std::string json = JSONSerializer::serialize(items);
        return HTTPResponse::ok(json);
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(std::string("Failed to load items: ") + e.what());
    }
}

HTTPResponse ItemRoutes::handleGetById(const HTTPRequest& request) {
    try {
        std::string idStr = extractIdFromPath(request.path);
        if (idStr.empty()) {
            return HTTPResponse::badRequest("Invalid item ID");
        }
        
        UUID id = UUID::fromString(idStr);
        auto item = database_->loadItem(id);
        
        if (!item) {
            return HTTPResponse::notFound("Item not found");
        }
        
        std::string json = JSONSerializer::serialize(item);
        return HTTPResponse::ok(json);
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(std::string("Failed to load item: ") + e.what());
    }
}

HTTPResponse ItemRoutes::handleCreate(const HTTPRequest& request) {
    try {
        auto item = JSONDeserializer::deserializeItem(request.body);
        if (!item) {
            return HTTPResponse::badRequest("Invalid item data");
        }
        
        if (database_->saveItem(item)) {
            std::string json = JSONSerializer::serialize(item);
            return HTTPResponse::created(json);
        }
        
        return HTTPResponse::internalError("Failed to create item");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(std::string("Failed to create item: ") + e.what());
    }
}

HTTPResponse ItemRoutes::handleUpdate(const HTTPRequest& request) {
    try {
        std::string idStr = extractIdFromPath(request.path);
        if (idStr.empty()) {
            return HTTPResponse::badRequest("Invalid item ID");
        }
        
        UUID id = UUID::fromString(idStr);
        auto existingItem = database_->loadItem(id);
        
        if (!existingItem) {
            return HTTPResponse::notFound("Item not found");
        }
        
        auto updatedItem = JSONDeserializer::deserializeItem(request.body);
        if (!updatedItem) {
            return HTTPResponse::badRequest("Invalid item data");
        }
        
        // Preserve the ID
        // Note: In a full implementation, you'd merge the updates with existing data
        
        if (database_->saveItem(updatedItem)) {
            std::string json = JSONSerializer::serialize(updatedItem);
            return HTTPResponse::ok(json);
        }
        
        return HTTPResponse::internalError("Failed to update item");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(std::string("Failed to update item: ") + e.what());
    }
}

HTTPResponse ItemRoutes::handleDelete(const HTTPRequest& request) {
    try {
        std::string idStr = extractIdFromPath(request.path);
        if (idStr.empty()) {
            return HTTPResponse::badRequest("Invalid item ID");
        }
        
        UUID id = UUID::fromString(idStr);
        
        if (database_->deleteItem(id)) {
            return HTTPResponse::noContent();
        }
        
        return HTTPResponse::notFound("Item not found");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(std::string("Failed to delete item: ") + e.what());
    }
}

HTTPResponse ItemRoutes::handleMove(const HTTPRequest& request) {
    // TODO: Implement item move logic
    return HTTPResponse::internalError("Move operation not yet implemented");
}

HTTPResponse ItemRoutes::handleCheckout(const HTTPRequest& request) {
    // TODO: Implement checkout logic
    return HTTPResponse::internalError("Checkout operation not yet implemented");
}

HTTPResponse ItemRoutes::handleCheckin(const HTTPRequest& request) {
    // TODO: Implement checkin logic
    return HTTPResponse::internalError("Checkin operation not yet implemented");
}

std::string ItemRoutes::extractIdFromPath(const std::string& path) {
    // Extract ID from path like "/api/items/550e8400-e29b-41d4-a716-446655440000"
    size_t lastSlash = path.find_last_of('/');
    if (lastSlash != std::string::npos && lastSlash + 1 < path.length()) {
        return path.substr(lastSlash + 1);
    }
    return "";
}
