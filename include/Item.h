#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include "UUID.h"
#include "Category.h"
#include "ItemType.h"

class Container;
class Location;
class ActivityLog;

/**
 * @brief Item represents a single physical unit of inventory
 * 
 * Each Item instance represents ONE physical unit with:
 * - Unique UUID for individual tracking
 * - Reference to ItemType for shared properties
 * - Individual notes/condition (e.g., "small defect on pin 3")
 * - Complete activity history specific to this unit
 * - Current location and status
 * 
 * Example: Three HC-05 modules = Three Item instances, all referencing
 *          the same ItemType, but each with unique UUID and history.
 */
class Item {
public:
    // Create new item instance of a given type
    Item(std::shared_ptr<ItemType> itemType,
         const std::string& individualNotes = "",
         const std::string& serialNumber = "",
         const std::string& createdBy = "system");
    
    // Constructor for deserialization
    Item(const UUID& id,
         std::shared_ptr<ItemType> itemType,
         const std::string& individualNotes = "",
         const std::string& serialNumber = "",
         const std::string& createdBy = "system",
         std::chrono::system_clock::time_point createdAt = std::chrono::system_clock::now());
    
    // Legacy constructor for backward compatibility (DEPRECATED)
    Item(const std::string& name, 
         std::shared_ptr<Category> category,
         int quantity = 1,
         const std::string& description = "");
    
    // Legacy constructor with UUID for deserialization (DEPRECATED)
    Item(const UUID& id,
         const std::string& name,
         std::shared_ptr<Category> category,
         int quantity = 1,
         const std::string& description = "");
    
    // Getters
    UUID getId() const;
    std::shared_ptr<ItemType> getItemType() const;
    std::string getName() const;  // Returns itemType->getName()
    std::string getDescription() const;  // Returns type description + individual notes
    std::string getIndividualNotes() const;
    std::string getSerialNumber() const;
    std::shared_ptr<Category> getCategory() const;  // Returns itemType->getCategory()
    std::shared_ptr<Container> getCurrentContainer() const;
    std::shared_ptr<Location> getOriginLocation() const;
    std::string getCreatedBy() const;
    std::chrono::system_clock::time_point getCreatedAt() const;
    
    // Quantity is always 1 for individual tracking, but kept for compatibility
    int getQuantity() const { return 1; }
    
    // Setters
    void setItemType(std::shared_ptr<ItemType> itemType);
    void setIndividualNotes(const std::string& notes);
    void setSerialNumber(const std::string& serialNumber);
    
    // Legacy setters (DEPRECATED - modify ItemType instead)
    void setName(const std::string& name);
    void setDescription(const std::string& description);
    void setCategory(std::shared_ptr<Category> category);
    void setQuantity(int quantity);  // No-op, kept for compatibility
    void adjustQuantity(int delta);  // No-op, kept for compatibility
    
    // Container management
    void setContainer(std::shared_ptr<Container> container);
    void setOriginLocation(std::shared_ptr<Location> location);
    
    // Activity logging
    void addActivity(std::shared_ptr<ActivityLog> activity);
    std::vector<std::shared_ptr<ActivityLog>> getActivityHistory() const;
    
    // Check-in/Check-out tracking
    bool isCheckedOut() const;
    std::chrono::system_clock::time_point getLastCheckOutTime() const;
    void checkOut();
    void checkIn();
    
private:
    UUID id_;
    std::shared_ptr<ItemType> itemType_;        // Type definition (shared)
    std::string individualNotes_;               // Instance-specific notes
    std::string serialNumber_;                  // Serial/asset number if applicable
    std::shared_ptr<Container> currentContainer_;
    std::shared_ptr<Location> originLocation_;  // Where it was originally created/received
    std::string createdBy_;                     // User who created/added this item
    std::chrono::system_clock::time_point createdAt_;
    std::vector<std::shared_ptr<ActivityLog>> activityHistory_;
    bool checkedOut_;
    std::chrono::system_clock::time_point lastCheckOutTime_;
    
    // Legacy fields for backward compatibility (DEPRECATED)
    std::string legacyName_;
    std::string legacyDescription_;
    std::shared_ptr<Category> legacyCategory_;
    int legacyQuantity_;
};

#endif // ITEM_H
