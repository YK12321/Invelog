#ifndef LOCALDATABASE_H
#define LOCALDATABASE_H

#include "Database.h"
#include <string>
#include <map>

// Local file-based database implementation
class LocalDatabase : public IDatabase {
public:
    explicit LocalDatabase(const std::string& dataDirectory);
    ~LocalDatabase() override;
    
    bool connect() override;
    bool disconnect() override;
    bool isConnected() const override;
    
    // Item operations
    bool saveItem(std::shared_ptr<Item> item) override;
    std::shared_ptr<Item> loadItem(const UUID& id) override;
    bool deleteItem(const UUID& id) override;
    std::vector<std::shared_ptr<Item>> loadAllItems() override;
    
    // Container operations
    bool saveContainer(std::shared_ptr<Container> container) override;
    std::shared_ptr<Container> loadContainer(const UUID& id) override;
    bool deleteContainer(const UUID& id) override;
    std::vector<std::shared_ptr<Container>> loadAllContainers() override;
    
    // Location operations
    bool saveLocation(std::shared_ptr<Location> location) override;
    std::shared_ptr<Location> loadLocation(const UUID& id) override;
    bool deleteLocation(const UUID& id) override;
    std::vector<std::shared_ptr<Location>> loadAllLocations() override;
    
    // Project operations
    bool saveProject(std::shared_ptr<Project> project) override;
    std::shared_ptr<Project> loadProject(const UUID& id) override;
    bool deleteProject(const UUID& id) override;
    std::vector<std::shared_ptr<Project>> loadAllProjects() override;
    
    // Category operations
    bool saveCategory(std::shared_ptr<Category> category) override;
    std::shared_ptr<Category> loadCategory(const UUID& id) override;
    bool deleteCategory(const UUID& id) override;
    std::vector<std::shared_ptr<Category>> loadAllCategories() override;
    
    // Activity log operations
    bool saveActivityLog(std::shared_ptr<ActivityLog> log) override;
    std::vector<std::shared_ptr<ActivityLog>> loadActivityLogsForItem(const UUID& itemId) override;
    std::vector<std::shared_ptr<ActivityLog>> loadRecentActivityLogs(int limit) override;
    
private:
    std::string dataDirectory_;
    bool connected_;
    
    // Helper methods
    bool ensureDirectoryExists(const std::string& path);
    std::string getFilePath(const std::string& type, const UUID& id) const;
    bool fileExists(const std::string& path) const;
};

#endif // LOCALDATABASE_H
