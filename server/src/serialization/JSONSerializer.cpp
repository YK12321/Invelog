#include "../include/serialization/JSONSerializer.h"
#include "../../include/Item.h"
#include "../../include/Container.h"
#include "../../include/Location.h"
#include "../../include/Project.h"
#include "../../include/Category.h"
#include "../../include/ActivityLog.h"
#include <nlohmann/json.hpp>
#include <chrono>
#include <iomanip>
#include <sstream>

using json = nlohmann::json;

// Helper function to convert time_point to ISO 8601 string
static std::string timePointToString(const std::chrono::system_clock::time_point& tp) {
    auto time_t = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

// Single entity serialization

std::string JSONSerializer::serialize(std::shared_ptr<Item> item) {
    if (!item) {
        return "null";
    }
    
    json j;
    j["id"] = item->getId().toString();
    j["name"] = item->getName();
    j["description"] = item->getDescription();
    j["quantity"] = item->getQuantity();
    j["checked_out"] = item->isCheckedOut();
    
    if (item->getCategory()) {
        j["category_id"] = item->getCategory()->getId().toString();
        j["category_name"] = item->getCategory()->getName();
    } else {
        j["category_id"] = nullptr;
    }
    
    if (item->getCurrentContainer()) {
        j["container_id"] = item->getCurrentContainer()->getId().toString();
        j["container_name"] = item->getCurrentContainer()->getName();
    } else {
        j["container_id"] = nullptr;
    }
    
    return j.dump();
}

std::string JSONSerializer::serialize(std::shared_ptr<Container> container) {
    if (!container) {
        return "null";
    }
    
    json j;
    j["id"] = container->getId().toString();
    j["name"] = container->getName();
    j["description"] = container->getDescription();
    j["type"] = static_cast<int>(container->getType());
    
    if (container->getLocation()) {
        j["location_id"] = container->getLocation()->getId().toString();
        j["location_name"] = container->getLocation()->getName();
    } else {
        j["location_id"] = nullptr;
    }
    
    if (container->getParentContainer()) {
        j["parent_container_id"] = container->getParentContainer()->getId().toString();
    } else {
        j["parent_container_id"] = nullptr;
    }
    
    j["item_count"] = container->getAllItems().size();
    j["subcontainer_count"] = container->getAllSubcontainers().size();
    
    return j.dump();
}

std::string JSONSerializer::serialize(std::shared_ptr<Location> location) {
    if (!location) {
        return "null";
    }
    
    json j;
    j["id"] = location->getId().toString();
    j["name"] = location->getName();
    j["address"] = location->getAddress();
    j["container_count"] = location->getAllContainers().size();
    
    return j.dump();
}

std::string JSONSerializer::serialize(std::shared_ptr<Project> project) {
    if (!project) {
        return "null";
    }
    
    json j;
    j["id"] = project->getId().toString();
    j["name"] = project->getName();
    j["description"] = project->getDescription();
    j["status"] = static_cast<int>(project->getStatus());
    j["created_date"] = timePointToString(project->getCreatedDate());
    j["start_date"] = timePointToString(project->getStartDate());
    j["end_date"] = timePointToString(project->getEndDate());
    j["container_count"] = project->getAllContainers().size();
    j["allocated_items"] = project->getTotalItemCount();
    
    return j.dump();
}

std::string JSONSerializer::serialize(std::shared_ptr<Category> category) {
    if (!category) {
        return "null";
    }
    
    json j;
    j["id"] = category->getId().toString();
    j["name"] = category->getName();
    j["description"] = category->getDescription();
    j["subcategory_count"] = category->getSubcategories().size();
    
    return j.dump();
}

std::string JSONSerializer::serialize(std::shared_ptr<ActivityLog> log) {
    if (!log) {
        return "null";
    }
    
    json j;
    j["id"] = log->getId().toString();
    j["type"] = log->getTypeString();
    j["description"] = log->getDescription();
    j["user_id"] = log->getUserId();
    j["timestamp"] = timePointToString(log->getTimestamp());
    j["quantity_change"] = log->getQuantityChange();
    
    if (log->getItem()) {
        j["item_id"] = log->getItem()->getId().toString();
        j["item_name"] = log->getItem()->getName();
    } else {
        j["item_id"] = nullptr;
    }
    
    return j.dump();
}

// Array serialization

std::string JSONSerializer::serialize(const std::vector<std::shared_ptr<Item>>& items) {
    json j = json::array();
    for (const auto& item : items) {
        if (item) {
            j.push_back(json::parse(serialize(item)));
        }
    }
    return j.dump();
}

std::string JSONSerializer::serialize(const std::vector<std::shared_ptr<Container>>& containers) {
    json j = json::array();
    for (const auto& container : containers) {
        if (container) {
            j.push_back(json::parse(serialize(container)));
        }
    }
    return j.dump();
}

std::string JSONSerializer::serialize(const std::vector<std::shared_ptr<Location>>& locations) {
    json j = json::array();
    for (const auto& location : locations) {
        if (location) {
            j.push_back(json::parse(serialize(location)));
        }
    }
    return j.dump();
}

std::string JSONSerializer::serialize(const std::vector<std::shared_ptr<Project>>& projects) {
    json j = json::array();
    for (const auto& project : projects) {
        if (project) {
            j.push_back(json::parse(serialize(project)));
        }
    }
    return j.dump();
}

std::string JSONSerializer::serialize(const std::vector<std::shared_ptr<Category>>& categories) {
    json j = json::array();
    for (const auto& category : categories) {
        if (category) {
            j.push_back(json::parse(serialize(category)));
        }
    }
    return j.dump();
}

std::string JSONSerializer::serialize(const std::vector<std::shared_ptr<ActivityLog>>& logs) {
    json j = json::array();
    for (const auto& log : logs) {
        if (log) {
            j.push_back(json::parse(serialize(log)));
        }
    }
    return j.dump();
}

// Error response serialization

std::string JSONSerializer::serializeError(const std::string& message) {
    json j;
    j["error"] = message;
    j["success"] = false;
    return j.dump();
}

std::string JSONSerializer::serializeSuccess(const std::string& message) {
    json j;
    j["message"] = message;
    j["success"] = true;
    return j.dump();
}
