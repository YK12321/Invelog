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
    
    // Update existing entities from JSON
    static void updateItem(std::shared_ptr<Item> item, const std::string& json);
    static void updateContainer(std::shared_ptr<Container> container, const std::string& json);
    static void updateLocation(std::shared_ptr<Location> location, const std::string& json);
    static void updateProject(std::shared_ptr<Project> project, const std::string& json);
    static void updateCategory(std::shared_ptr<Category> category, const std::string& json);
    
    // Validation
    static bool isValidJSON(const std::string& json);
};

#endif // JSON_DESERIALIZER_H
