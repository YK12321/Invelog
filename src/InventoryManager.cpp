#include "InventoryManager.h"
#include "Item.h"
#include "ItemType.h"
#include "Container.h"
#include "Location.h"
#include "Project.h"
#include "Category.h"
#include "ActivityLog.h"
#include <algorithm>
#include <iostream>

InventoryManager::InventoryManager(std::shared_ptr<IDatabase> database)
    : database_(database) {
}

bool InventoryManager::initialize() {
    if (!database_) {
        std::cerr << "Database not set" << std::endl;
        return false;
    }
    
    if (!database_->connect()) {
        std::cerr << "Failed to connect to database" << std::endl;
        return false;
    }
    
    return loadAll();
}

bool InventoryManager::shutdown() {
    if (!saveAll()) {
        std::cerr << "Failed to save all data" << std::endl;
        return false;
    }
    
    if (database_) {
        return database_->disconnect();
    }
    
    return true;
}

// ========================================
// ItemType Management (NEW)
// ========================================

std::shared_ptr<ItemType> InventoryManager::createItemType(const std::string& name,
                                                            std::shared_ptr<Category> category,
                                                            const std::string& description,
                                                            const std::string& specifications,
                                                            const std::string& manufacturer,
                                                            const std::string& partNumber) {
    auto itemType = std::make_shared<ItemType>(name, category, description, 
                                                specifications, manufacturer, partNumber);
    itemTypes_.push_back(itemType);
    
    database_->saveItemType(itemType);
    
    return itemType;
}

bool InventoryManager::deleteItemType(const UUID& itemTypeId) {
    // Check if any items are using this type
    auto itemsOfType = getItemsByType(itemTypeId);
    if (!itemsOfType.empty()) {
        std::cerr << "Cannot delete ItemType: " << itemsOfType.size() 
                  << " items still reference it" << std::endl;
        return false;
    }
    
    auto it = std::find_if(itemTypes_.begin(), itemTypes_.end(),
        [&itemTypeId](const std::shared_ptr<ItemType>& type) {
            return type->getId() == itemTypeId;
        });
    
    if (it != itemTypes_.end()) {
        database_->deleteItemType(itemTypeId);
        itemTypes_.erase(it);
        return true;
    }
    
    return false;
}

std::shared_ptr<ItemType> InventoryManager::getItemType(const UUID& itemTypeId) {
    auto it = std::find_if(itemTypes_.begin(), itemTypes_.end(),
        [&itemTypeId](const std::shared_ptr<ItemType>& type) {
            return type->getId() == itemTypeId;
        });
    
    return (it != itemTypes_.end()) ? *it : nullptr;
}

std::vector<std::shared_ptr<ItemType>> InventoryManager::getAllItemTypes() {
    return itemTypes_;
}

std::vector<std::shared_ptr<ItemType>> InventoryManager::searchItemTypes(const std::string& query) {
    std::vector<std::shared_ptr<ItemType>> results;
    
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    for (const auto& itemType : itemTypes_) {
        std::string name = itemType->getName();
        std::string desc = itemType->getDescription();
        std::string mfg = itemType->getManufacturer();
        std::string partNum = itemType->getPartNumber();
        
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        std::transform(desc.begin(), desc.end(), desc.begin(), ::tolower);
        std::transform(mfg.begin(), mfg.end(), mfg.begin(), ::tolower);
        std::transform(partNum.begin(), partNum.end(), partNum.begin(), ::tolower);
        
        if (name.find(lowerQuery) != std::string::npos ||
            desc.find(lowerQuery) != std::string::npos ||
            mfg.find(lowerQuery) != std::string::npos ||
            partNum.find(lowerQuery) != std::string::npos) {
            results.push_back(itemType);
        }
    }
    
    return results;
}

// Individual item instance creation
std::shared_ptr<Item> InventoryManager::createItemInstance(std::shared_ptr<ItemType> itemType,
                                                            std::shared_ptr<Container> initialContainer,
                                                            std::shared_ptr<Location> originLocation,
                                                            const std::string& individualNotes,
                                                            const std::string& serialNumber,
                                                            const std::string& createdBy) {
    auto item = std::make_shared<Item>(itemType, individualNotes, serialNumber, createdBy);
    
    // Set origin location
    if (originLocation) {
        item->setOriginLocation(originLocation);
    }
    
    // Place in initial container
    if (initialContainer) {
        item->setContainer(initialContainer);
        initialContainer->addItem(item);
    }
    
    items_.push_back(item);
    
    // Log creation with full context
    std::string logDesc = "Item instance created";
    if (originLocation) {
        logDesc += " at " + originLocation->getName();
    }
    if (initialContainer) {
        logDesc += " in container " + initialContainer->getName();
    }
    
    logActivity(ActivityType::CREATED, item, logDesc, createdBy);
    
    database_->saveItem(item);
    return item;
}

// Grouping and counting by type
std::vector<std::shared_ptr<Item>> InventoryManager::getItemsByType(const UUID& itemTypeId) {
    std::vector<std::shared_ptr<Item>> results;
    
    for (const auto& item : items_) {
        if (item->getItemType() && item->getItemType()->getId() == itemTypeId) {
            results.push_back(item);
        }
    }
    
    return results;
}

int InventoryManager::getItemCountByType(const UUID& itemTypeId) {
    return static_cast<int>(getItemsByType(itemTypeId).size());
}

// ========================================
// Legacy Item Management
// ========================================

// Item management (LEGACY)
std::shared_ptr<Item> InventoryManager::createItem(const std::string& name,
                                                   std::shared_ptr<Category> category,
                                                   int quantity,
                                                   const std::string& description) {
    auto item = std::make_shared<Item>(name, category, quantity, description);
    items_.push_back(item);
    
    // Log creation
    logActivity(ActivityType::CREATED, item, "Item created", "system");
    
    database_->saveItem(item);
    return item;
}

bool InventoryManager::deleteItem(const UUID& itemId) {
    auto it = std::find_if(items_.begin(), items_.end(),
        [&itemId](const std::shared_ptr<Item>& item) {
            return item->getId() == itemId;
        });
    
    if (it != items_.end()) {
        logActivity(ActivityType::DELETED, *it, "Item deleted", "system");
        
        // Remove from container if present
        if ((*it)->getCurrentContainer()) {
            (*it)->getCurrentContainer()->removeItem(itemId);
        }
        
        database_->deleteItem(itemId);
        items_.erase(it);
        return true;
    }
    
    return false;
}

std::shared_ptr<Item> InventoryManager::getItem(const UUID& itemId) {
    auto it = std::find_if(items_.begin(), items_.end(),
        [&itemId](const std::shared_ptr<Item>& item) {
            return item->getId() == itemId;
        });
    
    return (it != items_.end()) ? *it : nullptr;
}

std::vector<std::shared_ptr<Item>> InventoryManager::getAllItems() {
    return items_;
}

std::vector<std::shared_ptr<Item>> InventoryManager::searchItems(const std::string& query) {
    std::vector<std::shared_ptr<Item>> results;
    
    for (const auto& item : items_) {
        if (item->getName().find(query) != std::string::npos ||
            item->getDescription().find(query) != std::string::npos) {
            results.push_back(item);
        }
    }
    
    return results;
}

// Container management
std::shared_ptr<Container> InventoryManager::createContainer(const std::string& name,
                                                             ContainerType type,
                                                             const std::string& description) {
    auto container = std::make_shared<Container>(name, type, description);
    containers_.push_back(container);
    database_->saveContainer(container);
    return container;
}

bool InventoryManager::deleteContainer(const UUID& containerId) {
    auto it = std::find_if(containers_.begin(), containers_.end(),
        [&containerId](const std::shared_ptr<Container>& container) {
            return container->getId() == containerId;
        });
    
    if (it != containers_.end()) {
        // Remove from location if present
        if ((*it)->getLocation()) {
            (*it)->getLocation()->removeContainer(containerId);
        }
        
        database_->deleteContainer(containerId);
        containers_.erase(it);
        return true;
    }
    
    return false;
}

std::shared_ptr<Container> InventoryManager::getContainer(const UUID& containerId) {
    auto it = std::find_if(containers_.begin(), containers_.end(),
        [&containerId](const std::shared_ptr<Container>& container) {
            return container->getId() == containerId;
        });
    
    return (it != containers_.end()) ? *it : nullptr;
}

std::vector<std::shared_ptr<Container>> InventoryManager::getAllContainers() {
    return containers_;
}

// Location management
std::shared_ptr<Location> InventoryManager::createLocation(const std::string& name,
                                                          const std::string& address) {
    auto location = std::make_shared<Location>(name, address);
    locations_.push_back(location);
    database_->saveLocation(location);
    return location;
}

bool InventoryManager::deleteLocation(const UUID& locationId) {
    auto it = std::find_if(locations_.begin(), locations_.end(),
        [&locationId](const std::shared_ptr<Location>& location) {
            return location->getId() == locationId;
        });
    
    if (it != locations_.end()) {
        database_->deleteLocation(locationId);
        locations_.erase(it);
        return true;
    }
    
    return false;
}

std::shared_ptr<Location> InventoryManager::getLocation(const UUID& locationId) {
    auto it = std::find_if(locations_.begin(), locations_.end(),
        [&locationId](const std::shared_ptr<Location>& location) {
            return location->getId() == locationId;
        });
    
    return (it != locations_.end()) ? *it : nullptr;
}

std::vector<std::shared_ptr<Location>> InventoryManager::getAllLocations() {
    return locations_;
}

// Project management
std::shared_ptr<Project> InventoryManager::createProject(const std::string& name,
                                                        const std::string& description) {
    auto project = std::make_shared<Project>(name, description);
    projects_.push_back(project);
    database_->saveProject(project);
    return project;
}

bool InventoryManager::deleteProject(const UUID& projectId) {
    auto it = std::find_if(projects_.begin(), projects_.end(),
        [&projectId](const std::shared_ptr<Project>& project) {
            return project->getId() == projectId;
        });
    
    if (it != projects_.end()) {
        database_->deleteProject(projectId);
        projects_.erase(it);
        return true;
    }
    
    return false;
}

std::shared_ptr<Project> InventoryManager::getProject(const UUID& projectId) {
    auto it = std::find_if(projects_.begin(), projects_.end(),
        [&projectId](const std::shared_ptr<Project>& project) {
            return project->getId() == projectId;
        });
    
    return (it != projects_.end()) ? *it : nullptr;
}

std::vector<std::shared_ptr<Project>> InventoryManager::getAllProjects() {
    return projects_;
}

// Category management
std::shared_ptr<Category> InventoryManager::createCategory(const std::string& name,
                                                          const std::string& description) {
    auto category = std::make_shared<Category>(name, description);
    categories_.push_back(category);
    database_->saveCategory(category);
    return category;
}

bool InventoryManager::deleteCategory(const UUID& categoryId) {
    auto it = std::find_if(categories_.begin(), categories_.end(),
        [&categoryId](const std::shared_ptr<Category>& category) {
            return category->getId() == categoryId;
        });
    
    if (it != categories_.end()) {
        database_->deleteCategory(categoryId);
        categories_.erase(it);
        return true;
    }
    
    return false;
}

std::shared_ptr<Category> InventoryManager::getCategory(const UUID& categoryId) {
    auto it = std::find_if(categories_.begin(), categories_.end(),
        [&categoryId](const std::shared_ptr<Category>& category) {
            return category->getId() == categoryId;
        });
    
    return (it != categories_.end()) ? *it : nullptr;
}

std::vector<std::shared_ptr<Category>> InventoryManager::getAllCategories() {
    return categories_;
}

// Item operations
bool InventoryManager::moveItem(const UUID& itemId, const UUID& toContainerId) {
    auto item = getItem(itemId);
    auto toContainer = getContainer(toContainerId);
    
    if (!item || !toContainer) {
        return false;
    }
    
    auto fromContainer = item->getCurrentContainer();
    
    // Remove from old container
    if (fromContainer) {
        fromContainer->removeItem(itemId);
    }
    
    // Add to new container
    toContainer->addItem(item);
    
    // Log the move
    auto log = std::make_shared<ActivityLog>(ActivityType::MOVED, item, 
                                             "Item moved to " + toContainer->getName(), "system");
    log->setFromContainer(fromContainer);
    log->setToContainer(toContainer);
    item->addActivity(log);
    database_->saveActivityLog(log);
    database_->saveItem(item);
    
    return true;
}

bool InventoryManager::checkOutItem(const UUID& itemId, const std::string& userId) {
    auto item = getItem(itemId);
    if (!item) {
        return false;
    }
    
    item->checkOut();
    logActivity(ActivityType::CHECK_OUT, item, "Item checked out", userId);
    database_->saveItem(item);
    
    return true;
}

bool InventoryManager::checkInItem(const UUID& itemId, const std::string& userId) {
    auto item = getItem(itemId);
    if (!item) {
        return false;
    }
    
    item->checkIn();
    logActivity(ActivityType::CHECK_IN, item, "Item checked in", userId);
    database_->saveItem(item);
    
    return true;
}

bool InventoryManager::assignItemToProject(const UUID& itemId, const UUID& projectId) {
    auto item = getItem(itemId);
    auto project = getProject(projectId);
    
    if (!item || !project) {
        return false;
    }
    
    // If item is in a container, add that container to the project if not already there
    auto currentContainer = item->getCurrentContainer();
    if (currentContainer) {
        // Check if this container is already in the project
        auto projectContainers = project->getAllContainers();
        bool containerInProject = false;
        for (const auto& pc : projectContainers) {
            if (pc->getId() == currentContainer->getId()) {
                containerInProject = true;
                break;
            }
        }
        
        if (!containerInProject) {
            project->addContainer(currentContainer);
            database_->saveProject(project);
        }
    }
    
    auto log = std::make_shared<ActivityLog>(ActivityType::ASSIGNED_TO_PROJECT, item,
                                             "Item assigned to project: " + project->getName(), "system");
    log->setProject(project);
    item->addActivity(log);
    database_->saveActivityLog(log);
    database_->saveItem(item);
    
    return true;
}

bool InventoryManager::returnItemFromProject(const UUID& itemId) {
    auto item = getItem(itemId);
    if (!item) {
        return false;
    }
    
    logActivity(ActivityType::RETURNED_FROM_PROJECT, item, "Item returned from project", "system");
    database_->saveItem(item);
    
    return true;
}

// Activity tracking
std::vector<std::shared_ptr<ActivityLog>> InventoryManager::getItemHistory(const UUID& itemId) {
    auto item = getItem(itemId);
    if (!item) {
        return {};
    }
    
    return item->getActivityHistory();
}

std::vector<std::shared_ptr<ActivityLog>> InventoryManager::getRecentActivity(int limit) {
    return database_->loadRecentActivityLogs(limit);
}

// Search and query
std::shared_ptr<Item> InventoryManager::findItemByName(const std::string& name) {
    auto it = std::find_if(items_.begin(), items_.end(),
        [&name](const std::shared_ptr<Item>& item) {
            return item->getName() == name;
        });
    
    return (it != items_.end()) ? *it : nullptr;
}

std::vector<std::shared_ptr<Item>> InventoryManager::findItemsByCategory(const UUID& categoryId) {
    std::vector<std::shared_ptr<Item>> results;
    
    for (const auto& item : items_) {
        if (item->getCategory() && item->getCategory()->getId() == categoryId) {
            results.push_back(item);
        }
    }
    
    return results;
}

std::vector<std::shared_ptr<Item>> InventoryManager::findItemsInLocation(const UUID& locationId) {
    std::vector<std::shared_ptr<Item>> results;
    auto location = getLocation(locationId);
    
    if (!location) {
        return results;
    }
    
    // Find all containers at this location
    for (const auto& container : containers_) {
        if (container->getLocation() && container->getLocation()->getId() == locationId) {
            auto containerItems = container->getAllItems();
            results.insert(results.end(), containerItems.begin(), containerItems.end());
        }
    }
    
    return results;
}

std::vector<std::shared_ptr<Item>> InventoryManager::findItemsInProject(const UUID& projectId) {
    auto project = getProject(projectId);
    if (!project) {
        return {};
    }
    
    return project->getAllAllocatedItems();
}

// Helper methods
void InventoryManager::logActivity(ActivityType type, std::shared_ptr<Item> item,
                                   const std::string& description, const std::string& userId) {
    auto log = std::make_shared<ActivityLog>(type, item, description, userId);
    item->addActivity(log);
    database_->saveActivityLog(log);
}

bool InventoryManager::saveAll() {
    bool success = true;
    
    // Save ItemTypes first (they're referenced by Items)
    for (const auto& itemType : itemTypes_) {
        success &= database_->saveItemType(itemType);
    }
    
    for (const auto& item : items_) {
        success &= database_->saveItem(item);
    }
    
    for (const auto& container : containers_) {
        success &= database_->saveContainer(container);
    }
    
    for (const auto& location : locations_) {
        success &= database_->saveLocation(location);
    }
    
    for (const auto& project : projects_) {
        success &= database_->saveProject(project);
    }
    
    for (const auto& category : categories_) {
        success &= database_->saveCategory(category);
    }
    
    return success;
}

bool InventoryManager::loadAll() {
    // Load ItemTypes first (they're referenced by Items)
    itemTypes_ = database_->loadAllItemTypes();
    
    items_ = database_->loadAllItems();
    containers_ = database_->loadAllContainers();
    locations_ = database_->loadAllLocations();
    projects_ = database_->loadAllProjects();
    categories_ = database_->loadAllCategories();
    
    return true;
}
