#ifndef ITEMTYPE_H
#define ITEMTYPE_H

#include <string>
#include <memory>
#include "UUID.h"
#include "Category.h"

/**
 * @brief ItemType defines the shared characteristics of a type of item
 * 
 * Examples: "HC-05 Bluetooth Module", "10kΩ Resistor", "Arduino Uno R3"
 * 
 * Multiple Item instances can reference the same ItemType, allowing for:
 * - Consistent naming and categorization
 * - Shared specifications/documentation
 * - Easy grouping and counting by type
 * - Individual tracking of each physical unit
 */
class ItemType {
public:
    ItemType(const std::string& name,
             std::shared_ptr<Category> category,
             const std::string& description = "",
             const std::string& specifications = "",
             const std::string& manufacturer = "",
             const std::string& partNumber = "");
    
    // Constructor for deserialization
    ItemType(const UUID& id,
             const std::string& name,
             std::shared_ptr<Category> category,
             const std::string& description = "",
             const std::string& specifications = "",
             const std::string& manufacturer = "",
             const std::string& partNumber = "");
    
    // Getters
    UUID getId() const;
    std::string getName() const;
    std::string getDescription() const;
    std::string getSpecifications() const;
    std::string getManufacturer() const;
    std::string getPartNumber() const;
    std::shared_ptr<Category> getCategory() const;
    
    // Setters
    void setName(const std::string& name);
    void setDescription(const std::string& description);
    void setSpecifications(const std::string& specifications);
    void setManufacturer(const std::string& manufacturer);
    void setPartNumber(const std::string& partNumber);
    void setCategory(std::shared_ptr<Category> category);
    
private:
    UUID id_;
    std::string name_;                  // e.g., "HC-05 Bluetooth Module"
    std::shared_ptr<Category> category_; // e.g., "Wireless Communication"
    std::string description_;            // General description of this type
    std::string specifications_;         // Technical specs (JSON, plain text, etc.)
    std::string manufacturer_;           // e.g., "Generic/OEM"
    std::string partNumber_;             // Manufacturer part number
};

#endif // ITEMTYPE_H
