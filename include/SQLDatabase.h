#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include "Database.h"
#include <string>
#include <map>
#include <mutex>

// Forward declaration for database connection
// This will be implemented with actual SQL library (e.g., libpq, MySQL Connector)
class SQLConnection;

// SQL database implementation supporting PostgreSQL, MySQL, SQLite
class SQLDatabase : public IDatabase {
public:
    enum class SQLType {
        POSTGRESQL,
        MYSQL,
        SQLITE,
        MSSQL
    };
    
    struct ConnectionConfig {
        SQLType type;
        std::string host;
        int port;
        std::string database;
        std::string username;
        std::string password;
        std::string connectionString; // For custom connection strings
        int maxConnections = 10;
        int connectionTimeout = 30; // seconds
        bool useSSL = false;
    };
    
    explicit SQLDatabase(const ConnectionConfig& config);
    ~SQLDatabase() override;
    
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
    
    // SQL-specific operations
    bool initializeSchema();
    bool migrateSchema(int fromVersion, int toVersion);
    int getSchemaVersion();
    bool executeQuery(const std::string& query);
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();
    
private:
    ConnectionConfig config_;
    SQLConnection* connection_;
    bool connected_;
    mutable std::mutex connectionMutex_;
    
    // Helper methods
    std::string getConnectionString() const;
    bool createTables();
    bool createIndexes();
    std::string escapeString(const std::string& str) const;
    std::string getSQLTypeString() const;
    
    // Query builders
    std::string buildInsertQuery(const std::string& table, const std::map<std::string, std::string>& values);
    std::string buildUpdateQuery(const std::string& table, const std::map<std::string, std::string>& values, const std::string& where);
    std::string buildSelectQuery(const std::string& table, const std::vector<std::string>& columns, const std::string& where = "");
    std::string buildDeleteQuery(const std::string& table, const std::string& where);
};

#endif // SQLDATABASE_H
