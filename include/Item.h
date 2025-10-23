#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include "UUID.h"
#include "Category.h"

class Container;
class ActivityLog;

class Item {
public:
    Item(const std::string& name, 
         std::shared_ptr<Category> category,
         int quantity = 1,
         const std::string& description = "");
    
    // Constructor that allows specifying the UUID (for deserialization)
    Item(const UUID& id,
         const std::string& name,
         std::shared_ptr<Category> category,
         int quantity = 1,
         const std::string& description = "");
    
    UUID getId() const;
    std::string getName() const;
    std::string getDescription() const;
    std::shared_ptr<Category> getCategory() const;
    int getQuantity() const;
    std::shared_ptr<Container> getCurrentContainer() const;
    
    void setName(const std::string& name);
    void setDescription(const std::string& description);
    void setCategory(std::shared_ptr<Category> category);
    void setQuantity(int quantity);
    void adjustQuantity(int delta); // Add or remove quantity
    
    // Container management
    void setContainer(std::shared_ptr<Container> container);
    
    // Activity logging
    void addActivity(std::shared_ptr<ActivityLog> activity);
    std::vector<std::shared_ptr<ActivityLog>> getActivityHistory() const;
    
    // Check-in/Check-out tracking
    bool isCheckedOut() const;
    std::chrono::system_clock::time_point getLastCheckOutTime() const;
    
private:
    UUID id_;
    std::string name_;
    std::string description_;
    std::shared_ptr<Category> category_;
    int quantity_;
    std::shared_ptr<Container> currentContainer_;
    std::vector<std::shared_ptr<ActivityLog>> activityHistory_;
    bool checkedOut_;
    std::chrono::system_clock::time_point lastCheckOutTime_;
};

#endif // ITEM_H
