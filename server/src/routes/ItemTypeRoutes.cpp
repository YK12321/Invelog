#include "../include/routes/ItemTypeRoutes.h"
#include "../include/serialization/JSONSerializer.h"
#include "../../include/ItemType.h"
#include "../../include/Item.h"
#include "../../include/Category.h"
#include "../../include/Container.h"
#include "../../include/Location.h"
#include "../../include/UUID.h"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <sstream>

using json = nlohmann::json;

ItemTypeRoutes::ItemTypeRoutes(std::shared_ptr<IDatabase> database, std::shared_ptr<InventoryManager> inventoryManager)
    : database_(database), inventoryManager_(inventoryManager) {}

std::string ItemTypeRoutes::extractIdFromPath(const std::string& path) {
    // Extract ID from paths like "/api/item-types/123e4567-..."
    size_t lastSlash = path.find_last_of('/');
    if (lastSlash != std::string::npos) {
        return path.substr(lastSlash + 1);
    }
    return "";
}

// POST /api/item-types - Create new ItemType
HTTPResponse ItemTypeRoutes::handleCreate(const HTTPRequest& request) {
    try {
        auto j = json::parse(request.body);
        
        // Required fields
        if (!j.contains("name")) {
            return HTTPResponse::badRequest(JSONSerializer::serializeError("Missing required field: name"));
        }
        
        std::string name = j["name"].get<std::string>();
        std::string description = j.value("description", "");
        std::string specifications = j.value("specifications", "");
        std::string manufacturer = j.value("manufacturer", "");
        std::string partNumber = j.value("part_number", "");
        
        // Load category if provided
        std::shared_ptr<Category> category = nullptr;
        if (j.contains("category_id")) {
            UUID categoryId(j["category_id"].get<std::string>());
            category = inventoryManager_->getCategory(categoryId);
            if (!category) {
                return HTTPResponse::notFound(JSONSerializer::serializeError("Category not found"));
            }
        }
        
        // Create ItemType
        auto itemType = inventoryManager_->createItemType(name, category, description, 
                                                          specifications, manufacturer, partNumber);
        
        if (!itemType) {
            return HTTPResponse::internalError(JSONSerializer::serializeError("Failed to create ItemType"));
        }
        
        // Serialize response
        json response;
        response["id"] = itemType->getId().toString();
        response["name"] = itemType->getName();
        response["description"] = itemType->getDescription();
        response["specifications"] = itemType->getSpecifications();
        response["manufacturer"] = itemType->getManufacturer();
        response["part_number"] = itemType->getPartNumber();
        
        if (itemType->getCategory()) {
            response["category"] = {
                {"id", itemType->getCategory()->getId().toString()},
                {"name", itemType->getCategory()->getName()}
            };
        }
        
        return HTTPResponse::created(response.dump(2), "application/json");
        
    } catch (const std::exception& e) {
        return HTTPResponse::badRequest(JSONSerializer::serializeError(e.what()));
    }
}

// GET /api/item-types - List all ItemTypes
HTTPResponse ItemTypeRoutes::handleGetAll(const HTTPRequest& request) {
    try {
        auto itemTypes = inventoryManager_->getAllItemTypes();
        
        json response = json::array();
        for (const auto& itemType : itemTypes) {
            json typeJson;
            typeJson["id"] = itemType->getId().toString();
            typeJson["name"] = itemType->getName();
            typeJson["description"] = itemType->getDescription();
            typeJson["specifications"] = itemType->getSpecifications();
            typeJson["manufacturer"] = itemType->getManufacturer();
            typeJson["part_number"] = itemType->getPartNumber();
            
            if (itemType->getCategory()) {
                typeJson["category"] = {
                    {"id", itemType->getCategory()->getId().toString()},
                    {"name", itemType->getCategory()->getName()}
                };
            }
            
            // Include item count
            int count = inventoryManager_->getItemCountByType(itemType->getId());
            typeJson["item_count"] = count;
            
            response.push_back(typeJson);
        }
        
        return HTTPResponse::ok(response.dump(2), "application/json");
        
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}

// GET /api/item-types/:id - Get specific ItemType
HTTPResponse ItemTypeRoutes::handleGetById(const HTTPRequest& request) {
    try {
        std::string idStr = extractIdFromPath(request.path);
        if (idStr.empty()) {
            return HTTPResponse::badRequest(JSONSerializer::serializeError("Invalid ID"));
        }
        
        UUID id(idStr);
        auto itemType = inventoryManager_->getItemType(id);
        
        if (!itemType) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("ItemType not found"));
        }
        
        json response;
        response["id"] = itemType->getId().toString();
        response["name"] = itemType->getName();
        response["description"] = itemType->getDescription();
        response["specifications"] = itemType->getSpecifications();
        response["manufacturer"] = itemType->getManufacturer();
        response["part_number"] = itemType->getPartNumber();
        
        if (itemType->getCategory()) {
            response["category"] = {
                {"id", itemType->getCategory()->getId().toString()},
                {"name", itemType->getCategory()->getName()}
            };
        }
        
        // Include item count
        int count = inventoryManager_->getItemCountByType(itemType->getId());
        response["item_count"] = count;
        
        return HTTPResponse::ok(response.dump(2), "application/json");
        
    } catch (const std::exception& e) {
        return HTTPResponse::badRequest(JSONSerializer::serializeError(e.what()));
    }
}

// PUT /api/item-types/:id - Update ItemType
HTTPResponse ItemTypeRoutes::handleUpdate(const HTTPRequest& request) {
    try {
        std::string idStr = extractIdFromPath(request.path);
        if (idStr.empty()) {
            return HTTPResponse::badRequest(JSONSerializer::serializeError("Invalid ID"));
        }
        
        UUID id(idStr);
        auto itemType = inventoryManager_->getItemType(id);
        
        if (!itemType) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("ItemType not found"));
        }
        
        auto j = json::parse(request.body);
        
        // Update fields if provided
        if (j.contains("name")) {
            itemType->setName(j["name"].get<std::string>());
        }
        if (j.contains("description")) {
            itemType->setDescription(j["description"].get<std::string>());
        }
        if (j.contains("specifications")) {
            itemType->setSpecifications(j["specifications"].get<std::string>());
        }
        if (j.contains("manufacturer")) {
            itemType->setManufacturer(j["manufacturer"].get<std::string>());
        }
        if (j.contains("part_number")) {
            itemType->setPartNumber(j["part_number"].get<std::string>());
        }
        if (j.contains("category_id")) {
            UUID categoryId(j["category_id"].get<std::string>());
            auto category = inventoryManager_->getCategory(categoryId);
            if (category) {
                itemType->setCategory(category);
            }
        }
        
        // Save changes
        database_->saveItemType(itemType);
        
        json response;
        response["id"] = itemType->getId().toString();
        response["name"] = itemType->getName();
        response["description"] = itemType->getDescription();
        response["specifications"] = itemType->getSpecifications();
        response["manufacturer"] = itemType->getManufacturer();
        response["part_number"] = itemType->getPartNumber();
        
        if (itemType->getCategory()) {
            response["category"] = {
                {"id", itemType->getCategory()->getId().toString()},
                {"name", itemType->getCategory()->getName()}
            };
        }
        
        return HTTPResponse::ok(response.dump(2), "application/json");
        
    } catch (const std::exception& e) {
        return HTTPResponse::badRequest(JSONSerializer::serializeError(e.what()));
    }
}

// DELETE /api/item-types/:id - Delete ItemType
HTTPResponse ItemTypeRoutes::handleDelete(const HTTPRequest& request) {
    try {
        std::string idStr = extractIdFromPath(request.path);
        if (idStr.empty()) {
            return HTTPResponse::badRequest(JSONSerializer::serializeError("Invalid ID"));
        }
        
        UUID id(idStr);
        
        // Check if any items reference this type
        int itemCount = inventoryManager_->getItemCountByType(id);
        if (itemCount > 0) {
            json error = {
                {"error", "Cannot delete ItemType"},
                {"reason", "Items still reference this type"},
                {"item_count", itemCount}
            };
            HTTPResponse response(409, error.dump());
            response.setContentType("application/json");
            return response;
        }
        
        bool success = inventoryManager_->deleteItemType(id);
        
        if (success) {
            return HTTPResponse::noContent();
        } else {
            return HTTPResponse::notFound(JSONSerializer::serializeError("ItemType not found"));
        }
        
    } catch (const std::exception& e) {
        return HTTPResponse::badRequest(JSONSerializer::serializeError(e.what()));
    }
}

// GET /api/item-types/:id/items - Get all items of this type
HTTPResponse ItemTypeRoutes::handleGetItemsByType(const HTTPRequest& request) {
    try {
        // Extract ID from path like "/api/item-types/123/items"
        std::string path = request.path;
        size_t lastSlash = path.find_last_of('/');
        size_t secondLastSlash = path.find_last_of('/', lastSlash - 1);
        std::string idStr = path.substr(secondLastSlash + 1, lastSlash - secondLastSlash - 1);
        
        UUID typeId(idStr);
        
        // Verify ItemType exists
        auto itemType = inventoryManager_->getItemType(typeId);
        if (!itemType) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("ItemType not found"));
        }
        
        auto items = inventoryManager_->getItemsByType(typeId);
        
        json response = json::array();
        for (const auto& item : items) {
            json itemJson;
            itemJson["id"] = item->getId().toString();
            itemJson["name"] = item->getName();
            itemJson["individual_notes"] = item->getIndividualNotes();
            itemJson["serial_number"] = item->getSerialNumber();
            itemJson["created_by"] = item->getCreatedBy();
            
            if (item->getCurrentContainer()) {
                itemJson["container"] = {
                    {"id", item->getCurrentContainer()->getId().toString()},
                    {"name", item->getCurrentContainer()->getName()}
                };
            }
            
            if (item->getOriginLocation()) {
                itemJson["origin_location"] = {
                    {"id", item->getOriginLocation()->getId().toString()},
                    {"name", item->getOriginLocation()->getName()}
                };
            }
            
            itemJson["checked_out"] = item->isCheckedOut();
            
            response.push_back(itemJson);
        }
        
        json result;
        result["item_type"] = {
            {"id", itemType->getId().toString()},
            {"name", itemType->getName()}
        };
        result["count"] = items.size();
        result["items"] = response;
        
        return HTTPResponse::ok(result.dump(2), "application/json");
        
    } catch (const std::exception& e) {
        return HTTPResponse::badRequest(JSONSerializer::serializeError(e.what()));
    }
}

// GET /api/item-types/:id/count - Count items of this type
HTTPResponse ItemTypeRoutes::handleGetItemCountByType(const HTTPRequest& request) {
    try {
        // Extract ID from path like "/api/item-types/123/count"
        std::string path = request.path;
        size_t lastSlash = path.find_last_of('/');
        size_t secondLastSlash = path.find_last_of('/', lastSlash - 1);
        std::string idStr = path.substr(secondLastSlash + 1, lastSlash - secondLastSlash - 1);
        
        UUID typeId(idStr);
        
        // Verify ItemType exists
        auto itemType = inventoryManager_->getItemType(typeId);
        if (!itemType) {
            return HTTPResponse::notFound(JSONSerializer::serializeError("ItemType not found"));
        }
        
        int count = inventoryManager_->getItemCountByType(typeId);
        
        json response;
        response["item_type_id"] = typeId.toString();
        response["item_type_name"] = itemType->getName();
        response["count"] = count;
        
        return HTTPResponse::ok(response.dump(2), "application/json");
        
    } catch (const std::exception& e) {
        return HTTPResponse::badRequest(JSONSerializer::serializeError(e.what()));
    }
}

// GET /api/item-types/search?q=query - Search ItemTypes
HTTPResponse ItemTypeRoutes::handleSearch(const HTTPRequest& request) {
    try {
        // Extract query parameter from request
        std::string query;
        size_t queryPos = request.path.find("?q=");
        if (queryPos != std::string::npos) {
            query = request.path.substr(queryPos + 3);
            // URL decode if needed (basic implementation)
            // For production, use proper URL decoding
        }
        
        if (query.empty()) {
            return HTTPResponse::badRequest(JSONSerializer::serializeError("Missing query parameter: q"));
        }
        
        auto itemTypes = inventoryManager_->searchItemTypes(query);
        
        json response = json::array();
        for (const auto& itemType : itemTypes) {
            json typeJson;
            typeJson["id"] = itemType->getId().toString();
            typeJson["name"] = itemType->getName();
            typeJson["description"] = itemType->getDescription();
            typeJson["specifications"] = itemType->getSpecifications();
            typeJson["manufacturer"] = itemType->getManufacturer();
            typeJson["part_number"] = itemType->getPartNumber();
            
            if (itemType->getCategory()) {
                typeJson["category"] = {
                    {"id", itemType->getCategory()->getId().toString()},
                    {"name", itemType->getCategory()->getName()}
                };
            }
            
            // Include item count
            int count = inventoryManager_->getItemCountByType(itemType->getId());
            typeJson["item_count"] = count;
            
            response.push_back(typeJson);
        }
        
        json result;
        result["query"] = query;
        result["count"] = response.size();
        result["results"] = response;
        
        return HTTPResponse::ok(result.dump(2), "application/json");
        
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(JSONSerializer::serializeError(e.what()));
    }
}
