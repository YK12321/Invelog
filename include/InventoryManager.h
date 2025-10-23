#ifndef INVENTORYMANAGER_H
#define INVENTORYMANAGER_H

#include <memory>
#include <vector>
#include <string>
#include "UUID.h"
#include "Database.h"
#include "ActivityLog.h"
#include "Container.h"

class Item;
class Location;
class Project;
class Category;
class ActivityLog;

// Main facade class for managing the inventory system
class InventoryManager {
public:
    explicit InventoryManager(std::shared_ptr<IDatabase> database);
    
    // Initialization
    bool initialize();
    bool shutdown();
    
    // Item management
    std::shared_ptr<Item> createItem(const std::string& name,
                                     std::shared_ptr<Category> category,
                                     int quantity = 1,
                                     const std::string& description = "");
    bool deleteItem(const UUID& itemId);
    std::shared_ptr<Item> getItem(const UUID& itemId);
    std::vector<std::shared_ptr<Item>> getAllItems();
    std::vector<std::shared_ptr<Item>> searchItems(const std::string& query);
    
    // Container management
    std::shared_ptr<Container> createContainer(const std::string& name,
                                              ContainerType type = ContainerType::INVENTORY,
                                              const std::string& description = "");
    bool deleteContainer(const UUID& containerId);
    std::shared_ptr<Container> getContainer(const UUID& containerId);
    std::vector<std::shared_ptr<Container>> getAllContainers();
    
    // Location management
    std::shared_ptr<Location> createLocation(const std::string& name,
                                            const std::string& address = "");
    bool deleteLocation(const UUID& locationId);
    std::shared_ptr<Location> getLocation(const UUID& locationId);
    std::vector<std::shared_ptr<Location>> getAllLocations();
    
    // Project management
    std::shared_ptr<Project> createProject(const std::string& name,
                                          const std::string& description = "");
    bool deleteProject(const UUID& projectId);
    std::shared_ptr<Project> getProject(const UUID& projectId);
    std::vector<std::shared_ptr<Project>> getAllProjects();
    
    // Category management
    std::shared_ptr<Category> createCategory(const std::string& name,
                                            const std::string& description = "");
    bool deleteCategory(const UUID& categoryId);
    std::shared_ptr<Category> getCategory(const UUID& categoryId);
    std::vector<std::shared_ptr<Category>> getAllCategories();
    
    // Item operations
    bool moveItem(const UUID& itemId, const UUID& toContainerId);
    bool checkOutItem(const UUID& itemId, const std::string& userId = "system");
    bool checkInItem(const UUID& itemId, const std::string& userId = "system");
    bool assignItemToProject(const UUID& itemId, const UUID& projectId);
    bool returnItemFromProject(const UUID& itemId);
    
    // Activity tracking
    std::vector<std::shared_ptr<ActivityLog>> getItemHistory(const UUID& itemId);
    std::vector<std::shared_ptr<ActivityLog>> getRecentActivity(int limit = 50);
    
    // Search and query
    std::shared_ptr<Item> findItemByName(const std::string& name);
    std::vector<std::shared_ptr<Item>> findItemsByCategory(const UUID& categoryId);
    std::vector<std::shared_ptr<Item>> findItemsInLocation(const UUID& locationId);
    std::vector<std::shared_ptr<Item>> findItemsInProject(const UUID& projectId);
    
private:
    std::shared_ptr<IDatabase> database_;
    
    // Cached data
    std::vector<std::shared_ptr<Item>> items_;
    std::vector<std::shared_ptr<Container>> containers_;
    std::vector<std::shared_ptr<Location>> locations_;
    std::vector<std::shared_ptr<Project>> projects_;
    std::vector<std::shared_ptr<Category>> categories_;
    
    // Helper methods
    void logActivity(ActivityType type, std::shared_ptr<Item> item, 
                    const std::string& description, const std::string& userId);
    bool saveAll();
    bool loadAll();
};

#endif // INVENTORYMANAGER_H
