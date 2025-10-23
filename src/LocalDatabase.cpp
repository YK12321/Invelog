#include "LocalDatabase.h"
#include "Item.h"
#include "Container.h"
#include "Location.h"
#include "Project.h"
#include "Category.h"
#include "ActivityLog.h"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

using json = nlohmann::json;

// Helper functions for time conversion
std::string timeToString(const std::chrono::system_clock::time_point& timePoint) {
    auto time = std::chrono::system_clock::to_time_t(timePoint);
    std::tm tm;
    #ifdef _WIN32
        localtime_s(&tm, &time);
    #else
        localtime_r(&time, &tm);
    #endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}

std::chrono::system_clock::time_point stringToTime(const std::string& timeStr) {
    std::tm tm = {};
    std::istringstream ss(timeStr);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

LocalDatabase::LocalDatabase(const std::string& dataDirectory)
    : dataDirectory_(dataDirectory), connected_(false) {}

LocalDatabase::~LocalDatabase() {
    disconnect();
}

bool LocalDatabase::connect() {
    if (connected_) return true;
    
    try {
        // Create main data directory
        if (!ensureDirectoryExists(dataDirectory_)) {
            std::cerr << "Failed to create data directory: " << dataDirectory_ << std::endl;
            return false;
        }
        
        // Create subdirectories for each entity type
        std::vector<std::string> subdirs = {
            "items", "containers", "locations", 
            "projects", "categories", "activity_logs"
        };
        
        for (const auto& subdir : subdirs) {
            std::string path = dataDirectory_ + "/" + subdir;
            if (!ensureDirectoryExists(path)) {
                std::cerr << "Failed to create subdirectory: " << path << std::endl;
                return false;
            }
        }
        
        connected_ = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error connecting to local database: " << e.what() << std::endl;
        return false;
    }
}

bool LocalDatabase::disconnect() {
    connected_ = false;
    return true;
}

bool LocalDatabase::isConnected() const {
    return connected_;
}

// Item operations
bool LocalDatabase::saveItem(std::shared_ptr<Item> item) {
    if (!connected_ || !item) return false;
    
    try {
        json j;
        j["id"] = item->getId().toString();
        j["name"] = item->getName();
        j["description"] = item->getDescription();
        j["quantity"] = item->getQuantity();
        j["checked_out"] = item->isCheckedOut();
        j["last_checkout_time"] = timeToString(item->getLastCheckOutTime());
        
        if (item->getCategory()) {
            j["category_id"] = item->getCategory()->getId().toString();
        }
        
        if (item->getCurrentContainer()) {
            j["container_id"] = item->getCurrentContainer()->getId().toString();
        }
        
        // Save activity history as array of IDs
        json activityIds = json::array();
        for (const auto& activity : item->getActivityHistory()) {
            activityIds.push_back(activity->getId().toString());
        }
        j["activity_ids"] = activityIds;
        
        std::string filePath = getFilePath("items", item->getId());
        std::ofstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filePath << std::endl;
            return false;
        }
        
        file << j.dump(4);
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving item: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<Item> LocalDatabase::loadItem(const UUID& id) {
    if (!connected_) return nullptr;
    
    try {
        std::string filePath = getFilePath("items", id);
        if (!fileExists(filePath)) {
            return nullptr;
        }
        
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return nullptr;
        }
        
        json j;
        file >> j;
        file.close();
        
        // Create item with basic info
        // Note: Category and Container need to be resolved by the caller
        auto item = std::make_shared<Item>(
            j["name"].get<std::string>(),
            nullptr,  // Category will be set by caller
            j["quantity"].get<int>(),
            j["description"].get<std::string>()
        );
        
        return item;
    } catch (const std::exception& e) {
        std::cerr << "Error loading item: " << e.what() << std::endl;
        return nullptr;
    }
}

bool LocalDatabase::deleteItem(const UUID& id) {
    if (!connected_) return false;
    
    try {
        std::string filePath = getFilePath("items", id);
        return std::filesystem::remove(filePath);
    } catch (const std::exception& e) {
        std::cerr << "Error deleting item: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::shared_ptr<Item>> LocalDatabase::loadAllItems() {
    std::vector<std::shared_ptr<Item>> items;
    if (!connected_) return items;
    
    try {
        std::string itemsDir = dataDirectory_ + "/items";
        if (!std::filesystem::exists(itemsDir)) {
            return items;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(itemsDir)) {
            if (entry.path().extension() == ".json") {
                std::string idStr = entry.path().stem().string();
                auto item = loadItem(UUID::fromString(idStr));
                if (item) {
                    items.push_back(item);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading all items: " << e.what() << std::endl;
    }
    
    return items;
}

// Container operations
bool LocalDatabase::saveContainer(std::shared_ptr<Container> container) {
    if (!connected_ || !container) return false;
    
    try {
        json j;
        j["id"] = container->getId().toString();
        j["name"] = container->getName();
        j["description"] = container->getDescription();
        j["type"] = static_cast<int>(container->getType());
        
        if (container->getLocation()) {
            j["location_id"] = container->getLocation()->getId().toString();
        }
        
        if (container->getParentContainer()) {
            j["parent_id"] = container->getParentContainer()->getId().toString();
        }
        
        // Save item IDs
        json itemIds = json::array();
        for (const auto& item : container->getAllItems()) {
            itemIds.push_back(item->getId().toString());
        }
        j["item_ids"] = itemIds;
        
        // Save subcontainer IDs
        json subcontainerIds = json::array();
        for (const auto& sub : container->getAllSubcontainers()) {
            subcontainerIds.push_back(sub->getId().toString());
        }
        j["subcontainer_ids"] = subcontainerIds;
        
        std::string filePath = getFilePath("containers", container->getId());
        std::ofstream file(filePath);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(4);
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving container: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<Container> LocalDatabase::loadContainer(const UUID& id) {
    if (!connected_) return nullptr;
    
    try {
        std::string filePath = getFilePath("containers", id);
        if (!fileExists(filePath)) {
            return nullptr;
        }
        
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return nullptr;
        }
        
        json j;
        file >> j;
        file.close();
        
        auto type = static_cast<ContainerType>(j["type"].get<int>());
        auto container = std::make_shared<Container>(
            j["name"].get<std::string>(),
            type,
            j["description"].get<std::string>()
        );
        
        return container;
    } catch (const std::exception& e) {
        std::cerr << "Error loading container: " << e.what() << std::endl;
        return nullptr;
    }
}

bool LocalDatabase::deleteContainer(const UUID& id) {
    if (!connected_) return false;
    
    try {
        std::string filePath = getFilePath("containers", id);
        return std::filesystem::remove(filePath);
    } catch (const std::exception& e) {
        std::cerr << "Error deleting container: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::shared_ptr<Container>> LocalDatabase::loadAllContainers() {
    std::vector<std::shared_ptr<Container>> containers;
    if (!connected_) return containers;
    
    try {
        std::string containersDir = dataDirectory_ + "/containers";
        if (!std::filesystem::exists(containersDir)) {
            return containers;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(containersDir)) {
            if (entry.path().extension() == ".json") {
                std::string idStr = entry.path().stem().string();
                auto container = loadContainer(UUID::fromString(idStr));
                if (container) {
                    containers.push_back(container);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading all containers: " << e.what() << std::endl;
    }
    
    return containers;
}

// Location operations
bool LocalDatabase::saveLocation(std::shared_ptr<Location> location) {
    if (!connected_ || !location) return false;
    
    try {
        json j;
        j["id"] = location->getId().toString();
        j["name"] = location->getName();
        j["address"] = location->getAddress();
        
        // Save container IDs
        json containerIds = json::array();
        for (const auto& container : location->getAllContainers()) {
            containerIds.push_back(container->getId().toString());
        }
        j["container_ids"] = containerIds;
        
        std::string filePath = getFilePath("locations", location->getId());
        std::ofstream file(filePath);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(4);
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving location: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<Location> LocalDatabase::loadLocation(const UUID& id) {
    if (!connected_) return nullptr;
    
    try {
        std::string filePath = getFilePath("locations", id);
        if (!fileExists(filePath)) {
            return nullptr;
        }
        
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return nullptr;
        }
        
        json j;
        file >> j;
        file.close();
        
        auto location = std::make_shared<Location>(
            j["name"].get<std::string>(),
            j["address"].get<std::string>()
        );
        
        return location;
    } catch (const std::exception& e) {
        std::cerr << "Error loading location: " << e.what() << std::endl;
        return nullptr;
    }
}

bool LocalDatabase::deleteLocation(const UUID& id) {
    if (!connected_) return false;
    
    try {
        std::string filePath = getFilePath("locations", id);
        return std::filesystem::remove(filePath);
    } catch (const std::exception& e) {
        std::cerr << "Error deleting location: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::shared_ptr<Location>> LocalDatabase::loadAllLocations() {
    std::vector<std::shared_ptr<Location>> locations;
    if (!connected_) return locations;
    
    try {
        std::string locationsDir = dataDirectory_ + "/locations";
        if (!std::filesystem::exists(locationsDir)) {
            return locations;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(locationsDir)) {
            if (entry.path().extension() == ".json") {
                std::string idStr = entry.path().stem().string();
                auto location = loadLocation(UUID::fromString(idStr));
                if (location) {
                    locations.push_back(location);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading all locations: " << e.what() << std::endl;
    }
    
    return locations;
}

// Project operations
bool LocalDatabase::saveProject(std::shared_ptr<Project> project) {
    if (!connected_ || !project) return false;
    
    try {
        json j;
        j["id"] = project->getId().toString();
        j["name"] = project->getName();
        j["description"] = project->getDescription();
        j["status"] = static_cast<int>(project->getStatus());
        j["created_date"] = timeToString(project->getCreatedDate());
        j["start_date"] = timeToString(project->getStartDate());
        j["end_date"] = timeToString(project->getEndDate());
        
        // Save container IDs
        json containerIds = json::array();
        for (const auto& container : project->getAllContainers()) {
            containerIds.push_back(container->getId().toString());
        }
        j["container_ids"] = containerIds;
        
        std::string filePath = getFilePath("projects", project->getId());
        std::ofstream file(filePath);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(4);
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving project: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<Project> LocalDatabase::loadProject(const UUID& id) {
    if (!connected_) return nullptr;
    
    try {
        std::string filePath = getFilePath("projects", id);
        if (!fileExists(filePath)) {
            return nullptr;
        }
        
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return nullptr;
        }
        
        json j;
        file >> j;
        file.close();
        
        auto project = std::make_shared<Project>(
            j["name"].get<std::string>(),
            j["description"].get<std::string>()
        );
        
        project->setStatus(static_cast<ProjectStatus>(j["status"].get<int>()));
        if (j.contains("start_date")) {
            project->setStartDate(stringToTime(j["start_date"].get<std::string>()));
        }
        if (j.contains("end_date")) {
            project->setEndDate(stringToTime(j["end_date"].get<std::string>()));
        }
        
        return project;
    } catch (const std::exception& e) {
        std::cerr << "Error loading project: " << e.what() << std::endl;
        return nullptr;
    }
}

bool LocalDatabase::deleteProject(const UUID& id) {
    if (!connected_) return false;
    
    try {
        std::string filePath = getFilePath("projects", id);
        return std::filesystem::remove(filePath);
    } catch (const std::exception& e) {
        std::cerr << "Error deleting project: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::shared_ptr<Project>> LocalDatabase::loadAllProjects() {
    std::vector<std::shared_ptr<Project>> projects;
    if (!connected_) return projects;
    
    try {
        std::string projectsDir = dataDirectory_ + "/projects";
        if (!std::filesystem::exists(projectsDir)) {
            return projects;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(projectsDir)) {
            if (entry.path().extension() == ".json") {
                std::string idStr = entry.path().stem().string();
                auto project = loadProject(UUID::fromString(idStr));
                if (project) {
                    projects.push_back(project);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading all projects: " << e.what() << std::endl;
    }
    
    return projects;
}

// Category operations
bool LocalDatabase::saveCategory(std::shared_ptr<Category> category) {
    if (!connected_ || !category) return false;
    
    try {
        json j;
        j["id"] = category->getId().toString();
        j["name"] = category->getName();
        j["description"] = category->getDescription();
        
        // Save subcategory IDs
        json subcategoryIds = json::array();
        for (const auto& sub : category->getSubcategories()) {
            subcategoryIds.push_back(sub->getId().toString());
        }
        j["subcategory_ids"] = subcategoryIds;
        
        std::string filePath = getFilePath("categories", category->getId());
        std::ofstream file(filePath);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(4);
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving category: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<Category> LocalDatabase::loadCategory(const UUID& id) {
    if (!connected_) return nullptr;
    
    try {
        std::string filePath = getFilePath("categories", id);
        if (!fileExists(filePath)) {
            return nullptr;
        }
        
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return nullptr;
        }
        
        json j;
        file >> j;
        file.close();
        
        auto category = std::make_shared<Category>(
            j["name"].get<std::string>(),
            j["description"].get<std::string>()
        );
        
        return category;
    } catch (const std::exception& e) {
        std::cerr << "Error loading category: " << e.what() << std::endl;
        return nullptr;
    }
}

bool LocalDatabase::deleteCategory(const UUID& id) {
    if (!connected_) return false;
    
    try {
        std::string filePath = getFilePath("categories", id);
        return std::filesystem::remove(filePath);
    } catch (const std::exception& e) {
        std::cerr << "Error deleting category: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::shared_ptr<Category>> LocalDatabase::loadAllCategories() {
    std::vector<std::shared_ptr<Category>> categories;
    if (!connected_) return categories;
    
    try {
        std::string categoriesDir = dataDirectory_ + "/categories";
        if (!std::filesystem::exists(categoriesDir)) {
            return categories;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(categoriesDir)) {
            if (entry.path().extension() == ".json") {
                std::string idStr = entry.path().stem().string();
                auto category = loadCategory(UUID::fromString(idStr));
                if (category) {
                    categories.push_back(category);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading all categories: " << e.what() << std::endl;
    }
    
    return categories;
}

// Activity log operations
bool LocalDatabase::saveActivityLog(std::shared_ptr<ActivityLog> log) {
    if (!connected_ || !log) return false;
    
    try {
        json j;
        j["id"] = log->getId().toString();
        j["type"] = static_cast<int>(log->getType());
        j["description"] = log->getDescription();
        j["timestamp"] = timeToString(log->getTimestamp());
        j["user_id"] = log->getUserId();
        j["quantity_change"] = log->getQuantityChange();
        
        if (log->getItem()) {
            j["item_id"] = log->getItem()->getId().toString();
        }
        
        if (log->getFromContainer()) {
            j["from_container_id"] = log->getFromContainer()->getId().toString();
        }
        
        if (log->getToContainer()) {
            j["to_container_id"] = log->getToContainer()->getId().toString();
        }
        
        if (log->getProject()) {
            j["project_id"] = log->getProject()->getId().toString();
        }
        
        std::string filePath = getFilePath("activity_logs", log->getId());
        std::ofstream file(filePath);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(4);
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving activity log: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::shared_ptr<ActivityLog>> LocalDatabase::loadActivityLogsForItem(const UUID& itemId) {
    std::vector<std::shared_ptr<ActivityLog>> logs;
    if (!connected_) return logs;
    
    try {
        std::string logsDir = dataDirectory_ + "/activity_logs";
        if (!std::filesystem::exists(logsDir)) {
            return logs;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(logsDir)) {
            if (entry.path().extension() == ".json") {
                std::ifstream file(entry.path());
                if (file.is_open()) {
                    json j;
                    file >> j;
                    file.close();
                    
                    // Check if this log is for the requested item
                    if (j.contains("item_id") && 
                        j["item_id"].get<std::string>() == itemId.toString()) {
                        // Note: We'd need full reconstruction here
                        // For now, just note this matches
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading activity logs for item: " << e.what() << std::endl;
    }
    
    return logs;
}

std::vector<std::shared_ptr<ActivityLog>> LocalDatabase::loadRecentActivityLogs(int limit) {
    std::vector<std::shared_ptr<ActivityLog>> logs;
    if (!connected_) return logs;
    
    // This would require sorting by timestamp and limiting results
    // Simplified implementation for now
    
    return logs;
}

// Private helper methods
bool LocalDatabase::ensureDirectoryExists(const std::string& path) {
    try {
        return std::filesystem::create_directories(path) || std::filesystem::exists(path);
    } catch (const std::exception& e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        return false;
    }
}

std::string LocalDatabase::getFilePath(const std::string& type, const UUID& id) const {
    return dataDirectory_ + "/" + type + "/" + id.toString() + ".json";
}

bool LocalDatabase::fileExists(const std::string& path) const {
    return std::filesystem::exists(path);
}