#include "Container.h"
#include "Item.h"
#include "Location.h"
#include "Category.h"
#include <algorithm>

Container::Container(const std::string& name,
                     ContainerType type,
                     const std::string& description)
    : id_(UUID::generate()),
      name_(name),
      description_(description),
      type_(type),
      location_(nullptr),
      parentContainer_(nullptr) {
}

UUID Container::getId() const {
    return id_;
}

std::string Container::getName() const {
    return name_;
}

std::string Container::getDescription() const {
    return description_;
}

ContainerType Container::getType() const {
    return type_;
}

std::shared_ptr<Location> Container::getLocation() const {
    return location_;
}

std::shared_ptr<Container> Container::getParentContainer() const {
    return parentContainer_;
}

void Container::setName(const std::string& name) {
    name_ = name;
}

void Container::setDescription(const std::string& description) {
    description_ = description;
}

void Container::setLocation(std::shared_ptr<Location> location) {
    location_ = location;
}

void Container::setParentContainer(std::shared_ptr<Container> parent) {
    parentContainer_ = parent;
}

void Container::addItem(std::shared_ptr<Item> item) {
    if (item) {
        // Check if item already exists
        auto it = std::find_if(items_.begin(), items_.end(),
            [&item](const std::shared_ptr<Item>& i) {
                return i->getId() == item->getId();
            });
        
        if (it == items_.end()) {
            items_.push_back(item);
            item->setContainer(shared_from_this());
        }
    }
}

void Container::removeItem(const UUID& itemId) {
    auto it = std::find_if(items_.begin(), items_.end(),
        [&itemId](const std::shared_ptr<Item>& i) {
            return i->getId() == itemId;
        });
    
    if (it != items_.end()) {
        (*it)->setContainer(nullptr);
        items_.erase(it);
    }
}

std::shared_ptr<Item> Container::getItem(const UUID& itemId) const {
    auto it = std::find_if(items_.begin(), items_.end(),
        [&itemId](const std::shared_ptr<Item>& i) {
            return i->getId() == itemId;
        });
    
    return (it != items_.end()) ? *it : nullptr;
}

std::vector<std::shared_ptr<Item>> Container::getAllItems() const {
    return items_;
}

void Container::addSubcontainer(std::shared_ptr<Container> subcontainer) {
    if (subcontainer) {
        // Check if subcontainer already exists
        auto it = std::find_if(subcontainers_.begin(), subcontainers_.end(),
            [&subcontainer](const std::shared_ptr<Container>& c) {
                return c->getId() == subcontainer->getId();
            });
        
        if (it == subcontainers_.end()) {
            subcontainers_.push_back(subcontainer);
            subcontainer->setParentContainer(shared_from_this());
        }
    }
}

void Container::removeSubcontainer(const UUID& subcontainerId) {
    auto it = std::find_if(subcontainers_.begin(), subcontainers_.end(),
        [&subcontainerId](const std::shared_ptr<Container>& c) {
            return c->getId() == subcontainerId;
        });
    
    if (it != subcontainers_.end()) {
        (*it)->setParentContainer(nullptr);
        subcontainers_.erase(it);
    }
}

std::shared_ptr<Container> Container::getSubcontainer(const UUID& subcontainerId) const {
    auto it = std::find_if(subcontainers_.begin(), subcontainers_.end(),
        [&subcontainerId](const std::shared_ptr<Container>& c) {
            return c->getId() == subcontainerId;
        });
    
    return (it != subcontainers_.end()) ? *it : nullptr;
}

std::vector<std::shared_ptr<Container>> Container::getAllSubcontainers() const {
    return subcontainers_;
}

std::vector<std::shared_ptr<Item>> Container::findItemsByName(const std::string& name) const {
    std::vector<std::shared_ptr<Item>> results;
    
    for (const auto& item : items_) {
        if (item->getName().find(name) != std::string::npos) {
            results.push_back(item);
        }
    }
    
    // Search in subcontainers recursively
    for (const auto& subcontainer : subcontainers_) {
        auto subResults = subcontainer->findItemsByName(name);
        results.insert(results.end(), subResults.begin(), subResults.end());
    }
    
    return results;
}

std::vector<std::shared_ptr<Item>> Container::findItemsByCategory(const UUID& categoryId) const {
    std::vector<std::shared_ptr<Item>> results;
    
    for (const auto& item : items_) {
        if (item->getCategory() && item->getCategory()->getId() == categoryId) {
            results.push_back(item);
        }
    }
    
    // Search in subcontainers recursively
    for (const auto& subcontainer : subcontainers_) {
        auto subResults = subcontainer->findItemsByCategory(categoryId);
        results.insert(results.end(), subResults.begin(), subResults.end());
    }
    
    return results;
}
