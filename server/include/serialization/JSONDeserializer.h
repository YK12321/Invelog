#ifndef JSON_DESERIALIZER_H
#define JSON_DESERIALIZER_H

#include <string>
#include <memory>

// Forward declarations
class Item;
class Container;
class Location;
class Project;
class Category;
class ActivityLog;

/**
 * @brief JSON Deserializer
 * 
 * Converts JSON strings to inventory management entities.
 */
class JSONDeserializer {
public:
    // Single entity deserialization
    static std::shared_ptr<Item> deserializeItem(const std::string& json);
    static std::shared_ptr<Container> deserializeContainer(const std::string& json);
    static std::shared_ptr<Location> deserializeLocation(const std::string& json);
    static std::shared_ptr<Project> deserializeProject(const std::string& json);
    static std::shared_ptr<Category> deserializeCategory(const std::string& json);
    static std::shared_ptr<ActivityLog> deserializeActivityLog(const std::string& json);
    
    // Validation
    static bool isValidJSON(const std::string& json);
};

#endif // JSON_DESERIALIZER_H
