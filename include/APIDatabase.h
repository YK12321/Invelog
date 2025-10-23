#ifndef APIDATABASE_H
#define APIDATABASE_H

#include "Database.h"
#include <string>
#include <map>
#include <functional>
#include <mutex>

// Forward declaration for HTTP client
class HTTPClient;

// Custom API database implementation
// Communicates with external REST API for data storage
class APIDatabase : public IDatabase {
public:
    struct APIConfig {
        std::string baseUrl;              // Base API URL (e.g., "https://api.example.com/v1")
        std::string apiKey;               // API authentication key
        std::string apiSecret;            // API secret (if needed)
        std::string authToken;            // Bearer token (if using OAuth)
        
        // Authentication method
        enum class AuthMethod {
            NONE,
            API_KEY,
            BEARER_TOKEN,
            BASIC_AUTH,
            OAUTH2
        } authMethod = AuthMethod::API_KEY;
        
        // Request settings
        int timeoutSeconds = 30;
        int maxRetries = 3;
        bool verifySSL = true;
        
        // Custom headers
        std::map<std::string, std::string> customHeaders;
        
        // Rate limiting
        int maxRequestsPerMinute = 60;
        
        // Endpoints customization
        std::string itemsEndpoint = "/items";
        std::string containersEndpoint = "/containers";
        std::string locationsEndpoint = "/locations";
        std::string projectsEndpoint = "/projects";
        std::string categoriesEndpoint = "/categories";
        std::string activityLogsEndpoint = "/activity-logs";
    };
    
    explicit APIDatabase(const APIConfig& config);
    ~APIDatabase() override;
    
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
    
    // API-specific operations
    bool testConnection();
    std::string getAPIVersion();
    bool validateAPIKey();
    void setCustomHeader(const std::string& key, const std::string& value);
    void removeCustomHeader(const std::string& key);
    
    // Batch operations for efficiency
    bool saveBatch(const std::vector<std::shared_ptr<Item>>& items);
    bool deleteBatch(const std::vector<UUID>& ids, const std::string& entityType);
    
private:
    APIConfig config_;
    HTTPClient* httpClient_;
    bool connected_;
    mutable std::mutex apiMutex_;
    
    // Rate limiting
    std::chrono::steady_clock::time_point lastRequestTime_;
    int requestCount_;
    
    // Helper methods
    bool checkRateLimit();
    std::string buildAuthHeader() const;
    std::map<std::string, std::string> getDefaultHeaders() const;
    
    // HTTP operations
    std::string httpGet(const std::string& endpoint);
    std::string httpPost(const std::string& endpoint, const std::string& jsonData);
    std::string httpPut(const std::string& endpoint, const std::string& jsonData);
    bool httpDelete(const std::string& endpoint);
    
    // JSON serialization helpers
    std::string serializeItem(std::shared_ptr<Item> item);
    std::string serializeContainer(std::shared_ptr<Container> container);
    std::string serializeLocation(std::shared_ptr<Location> location);
    std::string serializeProject(std::shared_ptr<Project> project);
    std::string serializeCategory(std::shared_ptr<Category> category);
    std::string serializeActivityLog(std::shared_ptr<ActivityLog> log);
    
    // JSON deserialization helpers (to be implemented with JSON library)
    std::shared_ptr<Item> deserializeItem(const std::string& json);
    std::shared_ptr<Container> deserializeContainer(const std::string& json);
    std::shared_ptr<Location> deserializeLocation(const std::string& json);
    std::shared_ptr<Project> deserializeProject(const std::string& json);
    std::shared_ptr<Category> deserializeCategory(const std::string& json);
    std::shared_ptr<ActivityLog> deserializeActivityLog(const std::string& json);
    
    // Error handling
    void handleAPIError(int statusCode, const std::string& response);
    bool retryRequest(std::function<bool()> request);
};

#endif // APIDATABASE_H
