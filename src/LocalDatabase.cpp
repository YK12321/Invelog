#include "LocalDatabase.h"
#include "Item.h"
#include "Container.h"
#include "Location.h"
#include "Project.h"
#include "Category.h"
#include "ActivityLog.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <iostream>

using json = nlohmann::json;

LocalDatabase::LocalDatabase(const std::string& dataDirectory)
    : dataDirectory_(dataDirectory), connected_(false) {
}

LocalDatabase::~LocalDatabase() {
    disconnect();
}

bool LocalDatabase::connect() {
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
    
    std::string filePath = getFilePath("items", item->getId());
    std::ofstream file(filePath);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filePath << std::endl;
        return false;
    }
    
    // JSON serialization
    json j;
    j["id"] = item->getId().toString();
    j["name"] = item->getName();
    j["description"] = item->getDescription();
    j["quantity"] = item->getQuantity();
    j["checked_out"] = item->isCheckedOut();
    
    if (item->getCategory()) {
        j["category_id"] = item->getCategory()->getId().toString();
    }
    
    if (item->getCurrentContainer()) {
        j["container_id"] = item->getCurrentContainer()->getId().toString();
    }
    
    // Serialize activity history
    json activityArray = json::array();
    for (const auto& activity : item->getActivityHistory()) {
        activityArray.push_back(activity.getId().toString());
    }
    j["activity_history"] = activityArray;
    
    file << j.dump(2);  // Pretty print with 2-space indent
    file.close();
    return true;
}

std::shared_ptr<Item> LocalDatabase::loadItem(const UUID& id) {
    if (!connected_) return nullptr;
    
    std::string filePath = getFilePath("items", id);
    if (!fileExists(filePath)) {
        return nullptr;
    }
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for reading: " << filePath << std::endl;
        return nullptr;
    }
    
    try {
        json j;
        file >> j;
        file.close();
        
        // Deserialize item
        auto item = std::make_shared<Item>(
            UUID::fromString(j["id"].get<std::string>()),
            j["name"].get<std::string>(),
            j["description"].get<std::string>()
        );
        
        item->setQuantity(j["quantity"].get<int>());
        
        if (j.contains("checked_out")) {
            // The checked out state will be managed by the application
        }
        
        // Note: Category and Container relationships need to be resolved
        // by the application after loading all entities
        
        return item;
    } catch (const std::exception& e) {
        std::cerr << "Error deserializing item: " << e.what() << std::endl;
        return nullptr;
    }
}

bool LocalDatabase::deleteItem(const UUID& id) {
    if (!connected_) return false;
    
    std::string filePath = getFilePath("items", id);
    try {
        return std::filesystem::remove(filePath);
    } catch (const std::exception& e) {
        std::cerr << "Failed to delete item: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::shared_ptr<Item>> LocalDatabase::loadAllItems() {
    std::vector<std::shared_ptr<Item>> items;
    if (!connected_) return items;
    
    std::string dirPath = dataDirectory_ + "/items";
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string filename = entry.path().stem().string();
                auto item = loadItem(UUID::fromString(filename));
                if (item) {
                    items.push_back(item);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading items: " << e.what() << std::endl;
    }
    
    return items;
}

// Container operations
bool LocalDatabase::saveContainer(std::shared_ptr<Container> container) {
    if (!connected_ || !container) return false;
    
    std::string filePath = getFilePath("containers", container->getId());
    std::ofstream file(filePath);
    
    if (!file.is_open()) return false;
    
    // JSON serialization
    json j;
    j["id"] = container->getId().toString();
    j["name"] = container->getName();
    j["description"] = container->getDescription();
    j["type"] = static_cast<int>(container->getType());
    
    if (container->getLocation()) {
        j["location_id"] = container->getLocation()->getId().toString();
    }
    
    if (container->getParentContainer()) {
        j["parent_container_id"] = container->getParentContainer()->getId().toString();
    }
    
    // Serialize item IDs
    json itemsArray = json::array();
    for (const auto& item : container->getItems()) {
        itemsArray.push_back(item.getId().toString());
    }
    j["items"] = itemsArray;
    
    // Serialize subcontainer IDs
    json subcontainersArray = json::array();
    for (const auto& subcontainer : container->getSubContainers()) {
        subcontainersArray.push_back(subcontainer.getId().toString());
    }
    j["subcontainers"] = subcontainersArray;
    
    file << j.dump(2);
    file.close();
    return true;
}

std::shared_ptr<Container> LocalDatabase::loadContainer(const UUID& id) {
    if (!connected_) return nullptr;
    
    std::string filePath = getFilePath("containers", id);
    if (!fileExists(filePath)) {
        return nullptr;
    }
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return nullptr;
    }
    
    try {
        json j;
        file >> j;
        file.close();
        
        auto type = static_cast<ContainerType>(j["type"].get<int>());
        auto container = std::make_shared<Container>(
            UUID::fromString(j["id"].get<std::string>()),
            j["name"].get<std::string>(),
            j["description"].get<std::string>(),
            type
        );
        
        // Note: Location, parent, items, and subcontainers relationships
        // need to be resolved by the application after loading all entities
        
        return container;
    } catch (const std::exception& e) {
        std::cerr << "Error deserializing container: " << e.what() << std::endl;
        return nullptr;
    }
}

bool LocalDatabase::deleteContainer(const UUID& id) {
    if (!connected_) return false;
    
    std::string filePath = getFilePath("containers", id);
    try {
        return std::filesystem::remove(filePath);
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::shared_ptr<Container>> LocalDatabase::loadAllContainers() {
    std::vector<std::shared_ptr<Container>> containers;
    if (!connected_) return containers;
    
    std::string dirPath = dataDirectory_ + "/containers";
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string filename = entry.path().stem().string();
                auto container = loadContainer(UUID::fromString(filename));
                if (container) {
                    containers.push_back(container);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading containers: " << e.what() << std::endl;
    }
    
    return containers;
}

// Location operations
bool LocalDatabase::saveLocation(std::shared_ptr<Location> location) {
    if (!connected_ || !location) return false;
    
    std::string filePath = getFilePath("locations", location->getId());
    std::ofstream file(filePath);
    
    if (!file.is_open()) return false;
    
    // JSON serialization
    json j;
    j["id"] = location->getId().toString();
    j["name"] = location->getName();
    j["address"] = location->getAddress();
    
    // Serialize container IDs
    json containersArray = json::array();
    for (const auto& container : location->getContainers()) {
        containersArray.push_back(container.getId().toString());
    }
    j["containers"] = containersArray;
    
    file << j.dump(2);
    file.close();
    return true;
}

std::shared_ptr<Location> LocalDatabase::loadLocation(const UUID& id) {
    if (!connected_) return nullptr;
    
    std::string filePath = getFilePath("locations", id);
    if (!fileExists(filePath)) {
        return nullptr;
    }
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return nullptr;
    }
    
    try {
        json j;
        file >> j;
        file.close();
        
        auto location = std::make_shared<Location>(
            UUID::fromString(j["id"].get<std::string>()),
            j["name"].get<std::string>(),
            j["address"].get<std::string>()
        );
        
        return location;
    } catch (const std::exception& e) {
        std::cerr << "Error deserializing location: " << e.what() << std::endl;
        return nullptr;
    }
}

bool LocalDatabase::deleteLocation(const UUID& id) {
    if (!connected_) return false;
    
    std::string filePath = getFilePath("locations", id);
    try {
        return std::filesystem::remove(filePath);
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::shared_ptr<Location>> LocalDatabase::loadAllLocations() {
    std::vector<std::shared_ptr<Location>> locations;
    if (!connected_) return locations;
    
    std::string dirPath = dataDirectory_ + "/locations";
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string filename = entry.path().stem().string();
                auto location = loadLocation(UUID::fromString(filename));
                if (location) {
                    locations.push_back(location);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading locations: " << e.what() << std::endl;
    }
    
    return locations;
}

// Project operations
bool LocalDatabase::saveProject(std::shared_ptr<Project> project) {
    if (!connected_ || !project) return false;
    
    std::string filePath = getFilePath("projects", project->getId());
    std::ofstream file(filePath);
    
    if (!file.is_open()) return false;
    
    // JSON serialization
    json j;
    j["id"] = project->getId().toString();
    j["name"] = project->getName();
    j["description"] = project->getDescription();
    j["status"] = static_cast<int>(project->getStatus());
    j["created_date"] = project->getCreatedDate();
    j["start_date"] = project->getStartDate();
    j["end_date"] = project->getEndDate();
    
    // Serialize container IDs
    json containersArray = json::array();
    for (const auto& container : project->getContainers()) {
        containersArray.push_back(container.getId().toString());
    }
    j["containers"] = containersArray;
    
    file << j.dump(2);
    file.close();
    return true;
}

std::shared_ptr<Project> LocalDatabase::loadProject(const UUID& id) {
    if (!connected_) return nullptr;
    
    std::string filePath = getFilePath("projects", id);
    if (!fileExists(filePath)) {
        return nullptr;
    }
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return nullptr;
    }
    
    try {
        json j;
        file >> j;
        file.close();
        
        auto status = static_cast<ProjectStatus>(j["status"].get<int>());
        auto project = std::make_shared<Project>(
            UUID::fromString(j["id"].get<std::string>()),
            j["name"].get<std::string>(),
            j["description"].get<std::string>(),
            status
        );
        
        // Set dates if available
        if (j.contains("created_date") && !j["created_date"].is_null()) {
            // Note: Date handling would need proper implementation
        }
        
        return project;
    } catch (const std::exception& e) {
        std::cerr << "Error deserializing project: " << e.what() << std::endl;
        return nullptr;
    }
}

bool LocalDatabase::deleteProject(const UUID& id) {
    if (!connected_) return false;
    
    std::string filePath = getFilePath("projects", id);
    try {
        return std::filesystem::remove(filePath);
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::shared_ptr<Project>> LocalDatabase::loadAllProjects() {
    std::vector<std::shared_ptr<Project>> projects;
    if (!connected_) return projects;
    
    std::string dirPath = dataDirectory_ + "/projects";
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string filename = entry.path().stem().string();
                auto project = loadProject(UUID::fromString(filename));
                if (project) {
                    projects.push_back(project);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading projects: " << e.what() << std::endl;
    }
    
    return projects;
}

// Category operations
bool LocalDatabase::saveCategory(std::shared_ptr<Category> category) {
    if (!connected_ || !category) return false;
    
    std::string filePath = getFilePath("categories", category->getId());
    std::ofstream file(filePath);
    
    if (!file.is_open()) return false;
    
    // JSON serialization
    json j;
    j["id"] = category->getId().toString();
    j["name"] = category->getName();
    j["description"] = category->getDescription();
    
    // Serialize subcategory IDs
    json subcategoriesArray = json::array();
    for (const auto& subcategory : category->getSubcategories()) {
        subcategoriesArray.push_back(subcategory.getId().toString());
    }
    j["subcategories"] = subcategoriesArray;
    
    file << j.dump(2);
    file.close();
    return true;
}

std::shared_ptr<Category> LocalDatabase::loadCategory(const UUID& id) {
    if (!connected_) return nullptr;
    
    std::string filePath = getFilePath("categories", id);
    if (!fileExists(filePath)) {
        return nullptr;
    }
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return nullptr;
    }
    
    try {
        json j;
        file >> j;
        file.close();
        
        auto category = std::make_shared<Category>(
            UUID::fromString(j["id"].get<std::string>()),
            j["name"].get<std::string>(),
            j["description"].get<std::string>()
        );
        
        return category;
    } catch (const std::exception& e) {
        std::cerr << "Error deserializing category: " << e.what() << std::endl;
        return nullptr;
    }
}

bool LocalDatabase::deleteCategory(const UUID& id) {
    if (!connected_) return false;
    
    std::string filePath = getFilePath("categories", id);
    try {
        return std::filesystem::remove(filePath);
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::shared_ptr<Category>> LocalDatabase::loadAllCategories() {
    std::vector<std::shared_ptr<Category>> categories;
    if (!connected_) return categories;
    
    std::string dirPath = dataDirectory_ + "/categories";
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string filename = entry.path().stem().string();
                auto category = loadCategory(UUID::fromString(filename));
                if (category) {
                    categories.push_back(category);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading categories: " << e.what() << std::endl;
    }
    
    return categories;
}

// Activity log operations
bool LocalDatabase::saveActivityLog(std::shared_ptr<ActivityLog> log) {
    if (!connected_ || !log) return false;
    
    std::string filePath = getFilePath("activity_logs", log->getId());
    std::ofstream file(filePath);
    
    if (!file.is_open()) return false;
    
    // JSON serialization
    json j;
    j["id"] = log->getId().toString();
    j["type"] = log->getTypeString();
    j["description"] = log->getDescription();
    j["user_id"] = log->getUserId();
    j["timestamp"] = log->getTimestamp();
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
    
    file << j.dump(2);
    file.close();
    return true;
}

std::vector<std::shared_ptr<ActivityLog>> LocalDatabase::loadActivityLogsForItem(const UUID& itemId) {
    std::vector<std::shared_ptr<ActivityLog>> logs;
    if (!connected_) return logs;
    
    // Note: This would require indexing or scanning all activity logs
    // For production, consider a database with proper querying capabilities
    std::string dirPath = dataDirectory_ + "/activity_logs";
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::ifstream file(entry.path());
                if (file.is_open()) {
                    json j;
                    file >> j;
                    file.close();
                    
                    if (j.contains("item_id") && 
                        j["item_id"].get<std::string>() == itemId.toString()) {
                        // Would need to fully deserialize the activity log
                        // This is a simplified implementation
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading activity logs: " << e.what() << std::endl;
    }
    
    return logs;
}

std::vector<std::shared_ptr<ActivityLog>> LocalDatabase::loadRecentActivityLogs(int limit) {
    std::vector<std::shared_ptr<ActivityLog>> logs;
    if (!connected_) return logs;
    
    // Note: This would require timestamp-based sorting
    // For production, use a proper database with timestamp indexing
    return logs;
}

// Helper methods
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
