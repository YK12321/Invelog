#ifndef CONTAINER_H
#define CONTAINER_H

#include <string>
#include <vector>
#include <memory>
#include "UUID.h"

class Item;
class Location;

enum class ContainerType {
    INVENTORY,      // Main storage container
    PROJECT,        // Project-specific container
    SUBCONTAINER    // Nested container
};

class Container : public std::enable_shared_from_this<Container> {
public:
    Container(const std::string& name, 
              ContainerType type = ContainerType::INVENTORY,
              const std::string& description = "");
    
    UUID getId() const;
    std::string getName() const;
    std::string getDescription() const;
    ContainerType getType() const;
    std::shared_ptr<Location> getLocation() const;
    std::shared_ptr<Container> getParentContainer() const;
    
    void setName(const std::string& name);
    void setDescription(const std::string& description);
    void setLocation(std::shared_ptr<Location> location);
    void setParentContainer(std::shared_ptr<Container> parent);
    
    // Item management
    void addItem(std::shared_ptr<Item> item);
    void removeItem(const UUID& itemId);
    std::shared_ptr<Item> getItem(const UUID& itemId) const;
    std::vector<std::shared_ptr<Item>> getAllItems() const;
    
    // Subcontainer management
    void addSubcontainer(std::shared_ptr<Container> subcontainer);
    void removeSubcontainer(const UUID& subcontainerId);
    std::shared_ptr<Container> getSubcontainer(const UUID& subcontainerId) const;
    std::vector<std::shared_ptr<Container>> getAllSubcontainers() const;
    
    // Search functionality
    std::vector<std::shared_ptr<Item>> findItemsByName(const std::string& name) const;
    std::vector<std::shared_ptr<Item>> findItemsByCategory(const UUID& categoryId) const;
    
private:
    UUID id_;
    std::string name_;
    std::string description_;
    ContainerType type_;
    std::shared_ptr<Location> location_;
    std::shared_ptr<Container> parentContainer_;
    std::vector<std::shared_ptr<Item>> items_;
    std::vector<std::shared_ptr<Container>> subcontainers_;
};

#endif // CONTAINER_H
