#include "APIDatabase.h"
#include "Item.h"
#include "Container.h"
#include "Location.h"
#include "Project.h"
#include "Category.h"
#include "ActivityLog.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

// Placeholder for actual HTTP client
// In production, use libcurl, cpp-httplib, or similar
class HTTPClient {
public:
    struct Response {
        int statusCode;
        std::string body;
        std::map<std::string, std::string> headers;
    };
    
    Response get(const std::string& url, const std::map<std::string, std::string>& headers) {
        std::cout << "HTTP GET: " << url << std::endl;
        return {200, "{\"success\": true}", {}};
    }
    
    Response post(const std::string& url, const std::string& data, 
                 const std::map<std::string, std::string>& headers) {
        std::cout << "HTTP POST: " << url << std::endl;
        return {201, "{\"success\": true}", {}};
    }
    
    Response put(const std::string& url, const std::string& data,
                const std::map<std::string, std::string>& headers) {
        std::cout << "HTTP PUT: " << url << std::endl;
        return {200, "{\"success\": true}", {}};
    }
    
    Response del(const std::string& url, const std::map<std::string, std::string>& headers) {
        std::cout << "HTTP DELETE: " << url << std::endl;
        return {204, "", {}};
    }
};

APIDatabase::APIDatabase(const APIConfig& config)
    : config_(config), httpClient_(nullptr), connected_(false),
      lastRequestTime_(std::chrono::steady_clock::now()), requestCount_(0) {
    httpClient_ = new HTTPClient();
}

APIDatabase::~APIDatabase() {
    disconnect();
    delete httpClient_;
}

bool APIDatabase::connect() {
    std::lock_guard<std::mutex> lock(apiMutex_);
    
    if (connected_) {
        return true;
    }
    
    std::cout << "Connecting to API: " << config_.baseUrl << std::endl;
    
    // Test the connection
    if (!testConnection()) {
        std::cerr << "Failed to connect to API" << std::endl;
        return false;
    }
    
    // Validate API key if required
    if (config_.authMethod != APIConfig::AuthMethod::NONE) {
        if (!validateAPIKey()) {
            std::cerr << "API authentication failed" << std::endl;
            return false;
        }
    }
    
    connected_ = true;
    std::cout << "Successfully connected to API" << std::endl;
    std::cout << "API Version: " << getAPIVersion() << std::endl;
    
    return true;
}

bool APIDatabase::disconnect() {
    std::lock_guard<std::mutex> lock(apiMutex_);
    
    if (!connected_) {
        return true;
    }
    
    connected_ = false;
    std::cout << "Disconnected from API" << std::endl;
    
    return true;
}

bool APIDatabase::isConnected() const {
    std::lock_guard<std::mutex> lock(apiMutex_);
    return connected_;
}

bool APIDatabase::testConnection() {
    try {
        std::string response = httpGet("/health");
        return !response.empty();
    } catch (const std::exception& e) {
        std::cerr << "Connection test failed: " << e.what() << std::endl;
        return false;
    }
}

std::string APIDatabase::getAPIVersion() {
    try {
        std::string response = httpGet("/version");
        // In production, parse JSON response
        return "1.0.0"; // Placeholder
    } catch (const std::exception& e) {
        return "unknown";
    }
}

bool APIDatabase::validateAPIKey() {
    try {
        std::string response = httpGet("/auth/validate");
        // In production, parse JSON response
        return true; // Placeholder
    } catch (const std::exception& e) {
        std::cerr << "API key validation failed: " << e.what() << std::endl;
        return false;
    }
}

void APIDatabase::setCustomHeader(const std::string& key, const std::string& value) {
    config_.customHeaders[key] = value;
}

void APIDatabase::removeCustomHeader(const std::string& key) {
    config_.customHeaders.erase(key);
}

bool APIDatabase::checkRateLimit() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::minutes>(now - lastRequestTime_);
    
    if (elapsed.count() >= 1) {
        // Reset counter after a minute
        requestCount_ = 0;
        lastRequestTime_ = now;
    }
    
    if (requestCount_ >= config_.maxRequestsPerMinute) {
        std::cout << "Rate limit reached, waiting..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(60 - elapsed.count()));
        requestCount_ = 0;
        lastRequestTime_ = std::chrono::steady_clock::now();
    }
    
    requestCount_++;
    return true;
}

std::string APIDatabase::buildAuthHeader() const {
    switch (config_.authMethod) {
        case APIConfig::AuthMethod::API_KEY:
            return "X-API-Key: " + config_.apiKey;
        
        case APIConfig::AuthMethod::BEARER_TOKEN:
            return "Authorization: Bearer " + config_.authToken;
        
        case APIConfig::AuthMethod::BASIC_AUTH:
            // In production, base64 encode username:password
            return "Authorization: Basic " + config_.apiKey;
        
        case APIConfig::AuthMethod::OAUTH2:
            return "Authorization: Bearer " + config_.authToken;
        
        case APIConfig::AuthMethod::NONE:
        default:
            return "";
    }
}

std::map<std::string, std::string> APIDatabase::getDefaultHeaders() const {
    std::map<std::string, std::string> headers;
    
    headers["Content-Type"] = "application/json";
    headers["Accept"] = "application/json";
    headers["User-Agent"] = "Invelog/1.0";
    
    // Add authentication
    std::string authHeader = buildAuthHeader();
    if (!authHeader.empty()) {
        size_t colonPos = authHeader.find(':');
        if (colonPos != std::string::npos) {
            std::string key = authHeader.substr(0, colonPos);
            std::string value = authHeader.substr(colonPos + 2); // Skip ": "
            headers[key] = value;
        }
    }
    
    // Add custom headers
    for (const auto& pair : config_.customHeaders) {
        headers[pair.first] = pair.second;
    }
    
    return headers;
}

std::string APIDatabase::httpGet(const std::string& endpoint) {
    if (!checkRateLimit()) return "";
    
    std::string url = config_.baseUrl + endpoint;
    auto headers = getDefaultHeaders();
    
    auto response = httpClient_->get(url, headers);
    
    if (response.statusCode >= 200 && response.statusCode < 300) {
        return response.body;
    }
    
    handleAPIError(response.statusCode, response.body);
    return "";
}

std::string APIDatabase::httpPost(const std::string& endpoint, const std::string& jsonData) {
    if (!checkRateLimit()) return "";
    
    std::string url = config_.baseUrl + endpoint;
    auto headers = getDefaultHeaders();
    
    auto response = httpClient_->post(url, jsonData, headers);
    
    if (response.statusCode >= 200 && response.statusCode < 300) {
        return response.body;
    }
    
    handleAPIError(response.statusCode, response.body);
    return "";
}

std::string APIDatabase::httpPut(const std::string& endpoint, const std::string& jsonData) {
    if (!checkRateLimit()) return "";
    
    std::string url = config_.baseUrl + endpoint;
    auto headers = getDefaultHeaders();
    
    auto response = httpClient_->put(url, jsonData, headers);
    
    if (response.statusCode >= 200 && response.statusCode < 300) {
        return response.body;
    }
    
    handleAPIError(response.statusCode, response.body);
    return "";
}

bool APIDatabase::httpDelete(const std::string& endpoint) {
    if (!checkRateLimit()) return false;
    
    std::string url = config_.baseUrl + endpoint;
    auto headers = getDefaultHeaders();
    
    auto response = httpClient_->del(url, headers);
    
    if (response.statusCode >= 200 && response.statusCode < 300) {
        return true;
    }
    
    handleAPIError(response.statusCode, response.body);
    return false;
}

void APIDatabase::handleAPIError(int statusCode, const std::string& response) {
    std::cerr << "API Error (Status " << statusCode << "): " << response << std::endl;
    
    switch (statusCode) {
        case 400:
            std::cerr << "Bad Request - Check request format" << std::endl;
            break;
        case 401:
            std::cerr << "Unauthorized - Check API credentials" << std::endl;
            break;
        case 403:
            std::cerr << "Forbidden - Insufficient permissions" << std::endl;
            break;
        case 404:
            std::cerr << "Not Found - Resource doesn't exist" << std::endl;
            break;
        case 429:
            std::cerr << "Rate Limit Exceeded" << std::endl;
            break;
        case 500:
            std::cerr << "Internal Server Error" << std::endl;
            break;
        case 503:
            std::cerr << "Service Unavailable" << std::endl;
            break;
    }
}

bool APIDatabase::retryRequest(std::function<bool()> request) {
    for (int attempt = 0; attempt < config_.maxRetries; ++attempt) {
        if (request()) {
            return true;
        }
        
        if (attempt < config_.maxRetries - 1) {
            std::cout << "Retrying request (attempt " << (attempt + 2) << ")" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1 << attempt)); // Exponential backoff
        }
    }
    
    return false;
}

// Serialization helpers (basic implementations - use JSON library in production)
std::string APIDatabase::serializeItem(std::shared_ptr<Item> item) {
    std::stringstream ss;
    ss << "{"
       << "\"id\":\"" << item->getId().toString() << "\","
       << "\"name\":\"" << item->getName() << "\","
       << "\"description\":\"" << item->getDescription() << "\","
       << "\"quantity\":" << item->getQuantity();
    
    if (item->getCategory()) {
        ss << ",\"category_id\":\"" << item->getCategory()->getId().toString() << "\"";
    }
    
    ss << "}";
    return ss.str();
}

std::string APIDatabase::serializeContainer(std::shared_ptr<Container> container) {
    std::stringstream ss;
    ss << "{"
       << "\"id\":\"" << container->getId().toString() << "\","
       << "\"name\":\"" << container->getName() << "\","
       << "\"description\":\"" << container->getDescription() << "\","
       << "\"type\":" << static_cast<int>(container->getType())
       << "}";
    return ss.str();
}

std::string APIDatabase::serializeLocation(std::shared_ptr<Location> location) {
    std::stringstream ss;
    ss << "{"
       << "\"id\":\"" << location->getId().toString() << "\","
       << "\"name\":\"" << location->getName() << "\","
       << "\"address\":\"" << location->getAddress() << "\""
       << "}";
    return ss.str();
}

std::string APIDatabase::serializeProject(std::shared_ptr<Project> project) {
    std::stringstream ss;
    ss << "{"
       << "\"id\":\"" << project->getId().toString() << "\","
       << "\"name\":\"" << project->getName() << "\","
       << "\"description\":\"" << project->getDescription() << "\","
       << "\"status\":" << static_cast<int>(project->getStatus())
       << "}";
    return ss.str();
}

std::string APIDatabase::serializeCategory(std::shared_ptr<Category> category) {
    std::stringstream ss;
    ss << "{"
       << "\"id\":\"" << category->getId().toString() << "\","
       << "\"name\":\"" << category->getName() << "\","
       << "\"description\":\"" << category->getDescription() << "\""
       << "}";
    return ss.str();
}

std::string APIDatabase::serializeActivityLog(std::shared_ptr<ActivityLog> log) {
    std::stringstream ss;
    ss << "{"
       << "\"id\":\"" << log->getId().toString() << "\","
       << "\"type\":\"" << log->getTypeString() << "\","
       << "\"description\":\"" << log->getDescription() << "\","
       << "\"user_id\":\"" << log->getUserId() << "\"";
    
    if (log->getItem()) {
        ss << ",\"item_id\":\"" << log->getItem()->getId().toString() << "\"";
    }
    
    ss << "}";
    return ss.str();
}

// Item operations
bool APIDatabase::saveItem(std::shared_ptr<Item> item) {
    if (!isConnected() || !item) return false;
    
    std::string json = serializeItem(item);
    std::string endpoint = config_.itemsEndpoint + "/" + item->getId().toString();
    
    return retryRequest([this, &endpoint, &json]() {
        std::string response = httpPut(endpoint, json);
        return !response.empty();
    });
}

std::shared_ptr<Item> APIDatabase::loadItem(const UUID& id) {
    if (!isConnected()) return nullptr;
    
    std::string endpoint = config_.itemsEndpoint + "/" + id.toString();
    std::string response = httpGet(endpoint);
    
    if (response.empty()) return nullptr;
    
    return deserializeItem(response);
}

bool APIDatabase::deleteItem(const UUID& id) {
    if (!isConnected()) return false;
    
    std::string endpoint = config_.itemsEndpoint + "/" + id.toString();
    return httpDelete(endpoint);
}

std::vector<std::shared_ptr<Item>> APIDatabase::loadAllItems() {
    std::vector<std::shared_ptr<Item>> items;
    if (!isConnected()) return items;
    
    std::string response = httpGet(config_.itemsEndpoint);
    // In production, parse JSON array and deserialize each item
    
    return items;
}

// Container operations
bool APIDatabase::saveContainer(std::shared_ptr<Container> container) {
    if (!isConnected() || !container) return false;
    
    std::string json = serializeContainer(container);
    std::string endpoint = config_.containersEndpoint + "/" + container->getId().toString();
    
    return retryRequest([this, &endpoint, &json]() {
        std::string response = httpPut(endpoint, json);
        return !response.empty();
    });
}

std::shared_ptr<Container> APIDatabase::loadContainer(const UUID& id) {
    if (!isConnected()) return nullptr;
    
    std::string endpoint = config_.containersEndpoint + "/" + id.toString();
    std::string response = httpGet(endpoint);
    
    if (response.empty()) return nullptr;
    
    return deserializeContainer(response);
}

bool APIDatabase::deleteContainer(const UUID& id) {
    if (!isConnected()) return false;
    
    std::string endpoint = config_.containersEndpoint + "/" + id.toString();
    return httpDelete(endpoint);
}

std::vector<std::shared_ptr<Container>> APIDatabase::loadAllContainers() {
    std::vector<std::shared_ptr<Container>> containers;
    if (!isConnected()) return containers;
    
    std::string response = httpGet(config_.containersEndpoint);
    return containers;
}

// Implement similar patterns for Location, Project, Category, ActivityLog...
bool APIDatabase::saveLocation(std::shared_ptr<Location> location) {
    if (!isConnected() || !location) return false;
    std::string json = serializeLocation(location);
    std::string endpoint = config_.locationsEndpoint + "/" + location->getId().toString();
    std::string response = httpPut(endpoint, json);
    return !response.empty();
}

std::shared_ptr<Location> APIDatabase::loadLocation(const UUID& id) {
    if (!isConnected()) return nullptr;
    std::string endpoint = config_.locationsEndpoint + "/" + id.toString();
    std::string response = httpGet(endpoint);
    return response.empty() ? nullptr : deserializeLocation(response);
}

bool APIDatabase::deleteLocation(const UUID& id) {
    if (!isConnected()) return false;
    return httpDelete(config_.locationsEndpoint + "/" + id.toString());
}

std::vector<std::shared_ptr<Location>> APIDatabase::loadAllLocations() {
    std::vector<std::shared_ptr<Location>> locations;
    if (!isConnected()) return locations;
    httpGet(config_.locationsEndpoint);
    return locations;
}

bool APIDatabase::saveProject(std::shared_ptr<Project> project) {
    if (!isConnected() || !project) return false;
    std::string json = serializeProject(project);
    std::string response = httpPut(config_.projectsEndpoint + "/" + project->getId().toString(), json);
    return !response.empty();
}

std::shared_ptr<Project> APIDatabase::loadProject(const UUID& id) {
    if (!isConnected()) return nullptr;
    std::string response = httpGet(config_.projectsEndpoint + "/" + id.toString());
    return response.empty() ? nullptr : deserializeProject(response);
}

bool APIDatabase::deleteProject(const UUID& id) {
    return isConnected() && httpDelete(config_.projectsEndpoint + "/" + id.toString());
}

std::vector<std::shared_ptr<Project>> APIDatabase::loadAllProjects() {
    std::vector<std::shared_ptr<Project>> projects;
    if (!isConnected()) return projects;
    httpGet(config_.projectsEndpoint);
    return projects;
}

bool APIDatabase::saveCategory(std::shared_ptr<Category> category) {
    if (!isConnected() || !category) return false;
    std::string json = serializeCategory(category);
    std::string response = httpPut(config_.categoriesEndpoint + "/" + category->getId().toString(), json);
    return !response.empty();
}

std::shared_ptr<Category> APIDatabase::loadCategory(const UUID& id) {
    if (!isConnected()) return nullptr;
    std::string response = httpGet(config_.categoriesEndpoint + "/" + id.toString());
    return response.empty() ? nullptr : deserializeCategory(response);
}

bool APIDatabase::deleteCategory(const UUID& id) {
    return isConnected() && httpDelete(config_.categoriesEndpoint + "/" + id.toString());
}

std::vector<std::shared_ptr<Category>> APIDatabase::loadAllCategories() {
    std::vector<std::shared_ptr<Category>> categories;
    if (!isConnected()) return categories;
    httpGet(config_.categoriesEndpoint);
    return categories;
}

bool APIDatabase::saveActivityLog(std::shared_ptr<ActivityLog> log) {
    if (!isConnected() || !log) return false;
    std::string json = serializeActivityLog(log);
    std::string response = httpPost(config_.activityLogsEndpoint, json);
    return !response.empty();
}

std::vector<std::shared_ptr<ActivityLog>> APIDatabase::loadActivityLogsForItem(const UUID& itemId) {
    std::vector<std::shared_ptr<ActivityLog>> logs;
    if (!isConnected()) return logs;
    httpGet(config_.activityLogsEndpoint + "?item_id=" + itemId.toString());
    return logs;
}

std::vector<std::shared_ptr<ActivityLog>> APIDatabase::loadRecentActivityLogs(int limit) {
    std::vector<std::shared_ptr<ActivityLog>> logs;
    if (!isConnected()) return logs;
    httpGet(config_.activityLogsEndpoint + "?limit=" + std::to_string(limit));
    return logs;
}

// Batch operations
bool APIDatabase::saveBatch(const std::vector<std::shared_ptr<Item>>& items) {
    if (!isConnected() || items.empty()) return false;
    
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < items.size(); ++i) {
        if (i > 0) ss << ",";
        ss << serializeItem(items[i]);
    }
    ss << "]";
    
    std::string response = httpPost(config_.itemsEndpoint + "/batch", ss.str());
    return !response.empty();
}

bool APIDatabase::deleteBatch(const std::vector<UUID>& ids, const std::string& entityType) {
    if (!isConnected() || ids.empty()) return false;
    
    std::stringstream ss;
    ss << "{\"ids\":[";
    for (size_t i = 0; i < ids.size(); ++i) {
        if (i > 0) ss << ",";
        ss << "\"" << ids[i].toString() << "\"";
    }
    ss << "]}";
    
    std::string endpoint = "/" + entityType + "/batch";
    return httpDelete(endpoint);
}

// Deserialization placeholders (implement with JSON library in production)
std::shared_ptr<Item> APIDatabase::deserializeItem(const std::string& json) {
    std::cerr << "Item deserialization not implemented (requires JSON library)" << std::endl;
    return nullptr;
}

std::shared_ptr<Container> APIDatabase::deserializeContainer(const std::string& json) {
    return nullptr;
}

std::shared_ptr<Location> APIDatabase::deserializeLocation(const std::string& json) {
    return nullptr;
}

std::shared_ptr<Project> APIDatabase::deserializeProject(const std::string& json) {
    return nullptr;
}

std::shared_ptr<Category> APIDatabase::deserializeCategory(const std::string& json) {
    return nullptr;
}

std::shared_ptr<ActivityLog> APIDatabase::deserializeActivityLog(const std::string& json) {
    return nullptr;
}
