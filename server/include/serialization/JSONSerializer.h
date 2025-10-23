#ifndef JSON_SERIALIZER_H
#define JSON_SERIALIZER_H

#include <string>
#include <vector>
#include <memory>

// Forward declarations
class Item;
class Container;
class Location;
class Project;
class Category;
class ActivityLog;

/**
 * @brief JSON Serializer
 * 
 * Converts inventory management entities to JSON strings.
 */
class JSONSerializer {
public:
    // Single entity serialization
    static std::string serialize(std::shared_ptr<Item> item);
    static std::string serialize(std::shared_ptr<Container> container);
    static std::string serialize(std::shared_ptr<Location> location);
    static std::string serialize(std::shared_ptr<Project> project);
    static std::string serialize(std::shared_ptr<Category> category);
    static std::string serialize(std::shared_ptr<ActivityLog> log);
    
    // Array serialization
    static std::string serialize(const std::vector<std::shared_ptr<Item>>& items);
    static std::string serialize(const std::vector<std::shared_ptr<Container>>& containers);
    static std::string serialize(const std::vector<std::shared_ptr<Location>>& locations);
    static std::string serialize(const std::vector<std::shared_ptr<Project>>& projects);
    static std::string serialize(const std::vector<std::shared_ptr<Category>>& categories);
    static std::string serialize(const std::vector<std::shared_ptr<ActivityLog>>& logs);
    
    // Error response serialization
    static std::string serializeError(const std::string& message);
    static std::string serializeSuccess(const std::string& message);
};

#endif // JSON_SERIALIZER_H
