#ifndef DATABASE_H
#define DATABASE_H

#include <memory>
#include <vector>
#include <string>
#include "UUID.h"

// Forward declarations
class Item;
class Container;
class Location;
class Project;
class Category;
class ActivityLog;

// Abstract base class for database operations
class IDatabase {
public:
    virtual ~IDatabase() = default;
    
    // Connection management
    virtual bool connect() = 0;
    virtual bool disconnect() = 0;
    virtual bool isConnected() const = 0;
    
    // Item operations
    virtual bool saveItem(std::shared_ptr<Item> item) = 0;
    virtual std::shared_ptr<Item> loadItem(const UUID& id) = 0;
    virtual bool deleteItem(const UUID& id) = 0;
    virtual std::vector<std::shared_ptr<Item>> loadAllItems() = 0;
    
    // Container operations
    virtual bool saveContainer(std::shared_ptr<Container> container) = 0;
    virtual std::shared_ptr<Container> loadContainer(const UUID& id) = 0;
    virtual bool deleteContainer(const UUID& id) = 0;
    virtual std::vector<std::shared_ptr<Container>> loadAllContainers() = 0;
    
    // Location operations
    virtual bool saveLocation(std::shared_ptr<Location> location) = 0;
    virtual std::shared_ptr<Location> loadLocation(const UUID& id) = 0;
    virtual bool deleteLocation(const UUID& id) = 0;
    virtual std::vector<std::shared_ptr<Location>> loadAllLocations() = 0;
    
    // Project operations
    virtual bool saveProject(std::shared_ptr<Project> project) = 0;
    virtual std::shared_ptr<Project> loadProject(const UUID& id) = 0;
    virtual bool deleteProject(const UUID& id) = 0;
    virtual std::vector<std::shared_ptr<Project>> loadAllProjects() = 0;
    
    // Category operations
    virtual bool saveCategory(std::shared_ptr<Category> category) = 0;
    virtual std::shared_ptr<Category> loadCategory(const UUID& id) = 0;
    virtual bool deleteCategory(const UUID& id) = 0;
    virtual std::vector<std::shared_ptr<Category>> loadAllCategories() = 0;
    
    // Activity log operations
    virtual bool saveActivityLog(std::shared_ptr<ActivityLog> log) = 0;
    virtual std::vector<std::shared_ptr<ActivityLog>> loadActivityLogsForItem(const UUID& itemId) = 0;
    virtual std::vector<std::shared_ptr<ActivityLog>> loadRecentActivityLogs(int limit) = 0;
};

#endif // DATABASE_H
