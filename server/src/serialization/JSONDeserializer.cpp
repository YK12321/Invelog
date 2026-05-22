#include "../include/serialization/JSONDeserializer.h"
#include "../../include/Item.h"
#include "../../include/Container.h"
#include "../../include/Location.h"
#include "../../include/Project.h"
#include "../../include/Category.h"
#include "../../include/ActivityLog.h"
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <chrono>
#include <iomanip>
#include <sstream>

using json = nlohmann::json;

// Helper function to parse ISO 8601 string to time_point
static std::chrono::system_clock::time_point stringToTimePoint(const std::string& str) {
    std::tm tm = {};
    std::stringstream ss(str);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    if (ss.fail()) {
        // Try alternative format
        ss.clear();
        ss.str(str);
        ss >> std::get_time(&tm, "%Y-%m-%d");
    }
    auto time_t = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time_t);
}

std::shared_ptr<Item> JSONDeserializer::deserializeItem(const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        
        std::string name = j.value("name", "");
        std::string description = j.value("description", "");
        int quantity = j.value("quantity", 1);
        
        if (name.empty()) {
            throw std::runtime_error("Item name is required");
        }
        
        // Check if ID is provided (for updates/creates with specific IDs)
        std::shared_ptr<Item> item;
        if (j.contains("id")) {
            UUID id = UUID::fromString(j["id"].get<std::string>());
            item = std::make_shared<Item>(id, name, nullptr, quantity, description);
        } else {
            // Create item with auto-generated ID
            item = std::make_shared<Item>(name, nullptr, quantity, description);
        }
        
        // Note: Category and Container relationships are set via separate endpoints
        // as they require looking up existing entities in the database
        
        return item;
    } catch (const json::exception& e) {
        throw std::runtime_error("Failed to parse Item JSON: " + std::string(e.what()));
    }
}

std::shared_ptr<Container> JSONDeserializer::deserializeContainer(const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        
        std::string name = j.value("name", "");
        std::string description = j.value("description", "");
        int type = j.value("type", 0);
        
        if (name.empty()) {
            throw std::runtime_error("Container name is required");
        }
        
        auto container = std::make_shared<Container>(
            name,
            static_cast<ContainerType>(type),
            description
        );
        
        // Note: Location and parent container relationships are set via separate endpoints
        
        return container;
    } catch (const json::exception& e) {
        throw std::runtime_error("Failed to parse Container JSON: " + std::string(e.what()));
    }
}

std::shared_ptr<Location> JSONDeserializer::deserializeLocation(const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        
        std::string name = j.value("name", "");
        std::string address = j.value("address", "");
        
        if (name.empty()) {
            throw std::runtime_error("Location name is required");
        }
        
        auto location = std::make_shared<Location>(name, address);
        
        return location;
    } catch (const json::exception& e) {
        throw std::runtime_error("Failed to parse Location JSON: " + std::string(e.what()));
    }
}

std::shared_ptr<Project> JSONDeserializer::deserializeProject(const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        
        std::string name = j.value("name", "");
        std::string description = j.value("description", "");
        int status = j.value("status", 0);
        std::string startDate = j.value("start_date", "");
        std::string endDate = j.value("end_date", "");
        
        if (name.empty()) {
            throw std::runtime_error("Project name is required");
        }
        
        auto project = std::make_shared<Project>(
            name,
            description
        );
        
        if (status >= 0 && status <= 4) {
            project->setStatus(static_cast<ProjectStatus>(status));
        }
        
        if (!startDate.empty()) {
            project->setStartDate(stringToTimePoint(startDate));
        }
        
        if (!endDate.empty()) {
            project->setEndDate(stringToTimePoint(endDate));
        }
        
        return project;
    } catch (const json::exception& e) {
        throw std::runtime_error("Failed to parse Project JSON: " + std::string(e.what()));
    }
}

std::shared_ptr<Category> JSONDeserializer::deserializeCategory(const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        
        std::string name = j.value("name", "");
        std::string description = j.value("description", "");
        
        if (name.empty()) {
            throw std::runtime_error("Category name is required");
        }
        
        auto category = std::make_shared<Category>(name, description);
        
        return category;
    } catch (const json::exception& e) {
        throw std::runtime_error("Failed to parse Category JSON: " + std::string(e.what()));
    }
}

void JSONDeserializer::updateItem(std::shared_ptr<Item> item, const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        
        if (j.contains("name")) {
            item->setName(j["name"]);
        }
        
        if (j.contains("description")) {
            item->setDescription(j["description"]);
        }
        
        if (j.contains("quantity")) {
            item->setQuantity(j["quantity"]);
        }
        
        // Note: Category and Container updates should be done via separate endpoints
        
    } catch (const json::exception& e) {
        throw std::runtime_error("Failed to update Item from JSON: " + std::string(e.what()));
    }
}

void JSONDeserializer::updateContainer(std::shared_ptr<Container> container, const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        
        if (j.contains("name")) {
            container->setName(j["name"]);
        }
        
        if (j.contains("description")) {
            container->setDescription(j["description"]);
        }
        
        // Note: Type is set at construction time and typically shouldn't be changed
        // Note: Location and parent container updates should be done via separate endpoints
        
    } catch (const json::exception& e) {
        throw std::runtime_error("Failed to update Container from JSON: " + std::string(e.what()));
    }
}

void JSONDeserializer::updateLocation(std::shared_ptr<Location> location, const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        
        if (j.contains("name")) {
            location->setName(j["name"]);
        }
        
        if (j.contains("address")) {
            location->setAddress(j["address"]);
        }
        
    } catch (const json::exception& e) {
        throw std::runtime_error("Failed to update Location from JSON: " + std::string(e.what()));
    }
}

void JSONDeserializer::updateProject(std::shared_ptr<Project> project, const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        
        if (j.contains("name")) {
            project->setName(j["name"]);
        }
        
        if (j.contains("description")) {
            project->setDescription(j["description"]);
        }
        
        if (j.contains("status")) {
            project->setStatus(static_cast<ProjectStatus>(j["status"].get<int>()));
        }
        
        if (j.contains("start_date")) {
            project->setStartDate(stringToTimePoint(j["start_date"].get<std::string>()));
        }
        
        if (j.contains("end_date")) {
            project->setEndDate(stringToTimePoint(j["end_date"].get<std::string>()));
        }
        
    } catch (const json::exception& e) {
        throw std::runtime_error("Failed to update Project from JSON: " + std::string(e.what()));
    }
}

void JSONDeserializer::updateCategory(std::shared_ptr<Category> category, const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        
        if (j.contains("name")) {
            category->setName(j["name"]);
        }
        
        if (j.contains("description")) {
            category->setDescription(j["description"]);
        }
        
    } catch (const json::exception& e) {
        throw std::runtime_error("Failed to update Category from JSON: " + std::string(e.what()));
    }
}
