#include "../include/routes/SearchRoutes.h"
#include "../include/serialization/JSONSerializer.h"
#include "../include/serialization/JSONDeserializer.h"
#include "../../include/Item.h"
#include "../../include/Container.h"
#include "../../include/UUID.h"
#include <nlohmann/json.hpp>
#include <algorithm>

SearchRoutes::SearchRoutes(std::shared_ptr<IDatabase> database, std::shared_ptr<InventoryManager> inventoryManager)
    : database_(database), inventoryManager_(inventoryManager) {}

HTTPResponse SearchRoutes::handleAdvancedSearch(const HTTPRequest& request) {
    try {
        auto json = nlohmann::json::parse(request.body);
        
        // Get all items first
        auto allItems = inventoryManager_->getAllItems();
        std::vector<std::shared_ptr<Item>> results = allItems;
        
        // Apply filters
        if (json.contains("query") && json["query"].is_string()) {
            std::string query = json["query"];
            std::string lowerQuery = query;
            std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
            
            results.erase(std::remove_if(results.begin(), results.end(),
                [&lowerQuery](const std::shared_ptr<Item>& item) {
                    std::string name = item->getName();
                    std::string desc = item->getDescription();
                    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
                    std::transform(desc.begin(), desc.end(), desc.begin(), ::tolower);
                    return name.find(lowerQuery) == std::string::npos && 
                           desc.find(lowerQuery) == std::string::npos;
                }), results.end());
        }
        
        // Filter by category
        if (json.contains("categoryId") && json["categoryId"].is_string()) {
            UUID categoryId = UUID::fromString(json["categoryId"]);
            results.erase(std::remove_if(results.begin(), results.end(),
                [&categoryId](const std::shared_ptr<Item>& item) {
                    return !item->getCategory() || item->getCategory()->getId() != categoryId;
                }), results.end());
        }
        
        // Filter by location
        if (json.contains("locationId") && json["locationId"].is_string()) {
            UUID locationId = UUID::fromString(json["locationId"]);
            auto locationItems = inventoryManager_->findItemsInLocation(locationId);
            
            // Intersect results with location items
            results.erase(std::remove_if(results.begin(), results.end(),
                [&locationItems](const std::shared_ptr<Item>& item) {
                    return std::find_if(locationItems.begin(), locationItems.end(),
                        [&item](const std::shared_ptr<Item>& li) {
                            return li->getId() == item->getId();
                        }) == locationItems.end();
                }), results.end());
        }
        
        // Filter by project
        if (json.contains("projectId") && json["projectId"].is_string()) {
            UUID projectId = UUID::fromString(json["projectId"]);
            auto projectItems = inventoryManager_->findItemsInProject(projectId);
            
            results.erase(std::remove_if(results.begin(), results.end(),
                [&projectItems](const std::shared_ptr<Item>& item) {
                    return std::find_if(projectItems.begin(), projectItems.end(),
                        [&item](const std::shared_ptr<Item>& pi) {
                            return pi->getId() == item->getId();
                        }) == projectItems.end();
                }), results.end());
        }
        
        // Filter by container
        if (json.contains("containerId") && json["containerId"].is_string()) {
            UUID containerId = UUID::fromString(json["containerId"]);
            auto container = inventoryManager_->getContainer(containerId);
            
            if (container) {
                auto containerItems = container->getAllItems();
                results.erase(std::remove_if(results.begin(), results.end(),
                    [&containerItems](const std::shared_ptr<Item>& item) {
                        return std::find_if(containerItems.begin(), containerItems.end(),
                            [&item](const std::shared_ptr<Item>& ci) {
                                return ci->getId() == item->getId();
                            }) == containerItems.end();
                    }), results.end());
            } else {
                results.clear();
            }
        }
        
        // Filter by checked out status
        if (json.contains("checkedOut") && json["checkedOut"].is_boolean()) {
            bool checkedOut = json["checkedOut"];
            results.erase(std::remove_if(results.begin(), results.end(),
                [checkedOut](const std::shared_ptr<Item>& item) {
                    return item->isCheckedOut() != checkedOut;
                }), results.end());
        }
        
        // Filter by minimum quantity
        if (json.contains("minQuantity") && json["minQuantity"].is_number()) {
            int minQuantity = json["minQuantity"];
            results.erase(std::remove_if(results.begin(), results.end(),
                [minQuantity](const std::shared_ptr<Item>& item) {
                    return item->getQuantity() < minQuantity;
                }), results.end());
        }
        
        // Filter by maximum quantity
        if (json.contains("maxQuantity") && json["maxQuantity"].is_number()) {
            int maxQuantity = json["maxQuantity"];
            results.erase(std::remove_if(results.begin(), results.end(),
                [maxQuantity](const std::shared_ptr<Item>& item) {
                    return item->getQuantity() > maxQuantity;
                }), results.end());
        }
        
        // Apply sorting if requested
        if (json.contains("sortBy") && json["sortBy"].is_string()) {
            std::string sortBy = json["sortBy"];
            bool descending = json.contains("descending") && json["descending"].is_boolean() && json["descending"];
            
            if (sortBy == "name") {
                std::sort(results.begin(), results.end(),
                    [descending](const std::shared_ptr<Item>& a, const std::shared_ptr<Item>& b) {
                        return descending ? a->getName() > b->getName() : a->getName() < b->getName();
                    });
            } else if (sortBy == "quantity") {
                std::sort(results.begin(), results.end(),
                    [descending](const std::shared_ptr<Item>& a, const std::shared_ptr<Item>& b) {
                        return descending ? a->getQuantity() > b->getQuantity() : a->getQuantity() < b->getQuantity();
                    });
            }
            // Note: createdAt sorting not available - Item class doesn't track creation time
        }
        
        // Apply limit if requested
        if (json.contains("limit") && json["limit"].is_number()) {
            int limit = json["limit"];
            if (limit > 0 && static_cast<size_t>(limit) < results.size()) {
                results.resize(limit);
            }
        }
        
        std::string responseJson = JSONSerializer::serialize(results);
        return HTTPResponse::ok(responseJson);
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(std::string("Failed to perform search: ") + e.what());
    }
}

HTTPResponse SearchRoutes::handleSimpleSearch(const HTTPRequest& request) {
    try {
        if (!request.hasQueryParam("q")) {
            return HTTPResponse::badRequest("Missing 'q' query parameter");
        }
        
        std::string query = request.getQueryParam("q");
        auto results = inventoryManager_->searchItems(query);
        
        std::string json = JSONSerializer::serialize(results);
        return HTTPResponse::ok(json);
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(std::string("Failed to search items: ") + e.what());
    }
}

HTTPResponse SearchRoutes::handleSearchByCategory(const HTTPRequest& request) {
    try {
        std::string idStr = extractIdFromPath(request.path);
        if (idStr.empty()) {
            return HTTPResponse::badRequest("Invalid category ID");
        }
        
        UUID categoryId = UUID::fromString(idStr);
        auto results = inventoryManager_->findItemsByCategory(categoryId);
        
        std::string json = JSONSerializer::serialize(results);
        return HTTPResponse::ok(json);
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(std::string("Failed to search by category: ") + e.what());
    }
}

HTTPResponse SearchRoutes::handleSearchByLocation(const HTTPRequest& request) {
    try {
        std::string idStr = extractIdFromPath(request.path);
        if (idStr.empty()) {
            return HTTPResponse::badRequest("Invalid location ID");
        }
        
        UUID locationId = UUID::fromString(idStr);
        auto results = inventoryManager_->findItemsInLocation(locationId);
        
        std::string json = JSONSerializer::serialize(results);
        return HTTPResponse::ok(json);
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(std::string("Failed to search by location: ") + e.what());
    }
}

HTTPResponse SearchRoutes::handleSearchByProject(const HTTPRequest& request) {
    try {
        std::string idStr = extractIdFromPath(request.path);
        if (idStr.empty()) {
            return HTTPResponse::badRequest("Invalid project ID");
        }
        
        UUID projectId = UUID::fromString(idStr);
        auto results = inventoryManager_->findItemsInProject(projectId);
        
        std::string json = JSONSerializer::serialize(results);
        return HTTPResponse::ok(json);
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(std::string("Failed to search by project: ") + e.what());
    }
}

HTTPResponse SearchRoutes::handleSearchByContainer(const HTTPRequest& request) {
    try {
        std::string idStr = extractIdFromPath(request.path);
        if (idStr.empty()) {
            return HTTPResponse::badRequest("Invalid container ID");
        }
        
        UUID containerId = UUID::fromString(idStr);
        auto container = inventoryManager_->getContainer(containerId);
        
        if (!container) {
            return HTTPResponse::notFound("Container not found");
        }
        
        auto results = container->getAllItems();
        std::string json = JSONSerializer::serialize(results);
        return HTTPResponse::ok(json);
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(std::string("Failed to search by container: ") + e.what());
    }
}

std::string SearchRoutes::extractIdFromPath(const std::string& path) {
    // Extract the last segment of the path as the ID
    size_t lastSlash = path.find_last_of('/');
    if (lastSlash != std::string::npos && lastSlash + 1 < path.length()) {
        return path.substr(lastSlash + 1);
    }
    return "";
}
