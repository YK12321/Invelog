#ifndef LOCATION_H
#define LOCATION_H

#include <string>
#include <vector>
#include <memory>
#include "UUID.h"

class Container;

class Location : public std::enable_shared_from_this<Location> {
public:
    Location(const std::string& name, const std::string& address = "");
    
    UUID getId() const;
    std::string getName() const;
    std::string getAddress() const;
    
    void setName(const std::string& name);
    void setAddress(const std::string& address);
    
    void addContainer(std::shared_ptr<Container> container);
    void removeContainer(const UUID& containerId);
    std::shared_ptr<Container> getContainer(const UUID& containerId) const;
    std::vector<std::shared_ptr<Container>> getAllContainers() const;
    
private:
    UUID id_;
    std::string name_;
    std::string address_;
    std::vector<std::shared_ptr<Container>> containers_;
};

#endif // LOCATION_H
