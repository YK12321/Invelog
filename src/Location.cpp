#include "Location.h"
#include "Container.h"
#include <algorithm>

Location::Location(const std::string& name, const std::string& address)
    : id_(UUID::generate()), name_(name), address_(address) {
}

UUID Location::getId() const {
    return id_;
}

std::string Location::getName() const {
    return name_;
}

std::string Location::getAddress() const {
    return address_;
}

void Location::setName(const std::string& name) {
    name_ = name;
}

void Location::setAddress(const std::string& address) {
    address_ = address;
}

void Location::addContainer(std::shared_ptr<Container> container) {
    if (container) {
        // Check if container already exists
        auto it = std::find_if(containers_.begin(), containers_.end(),
            [&container](const std::shared_ptr<Container>& c) {
                return c->getId() == container->getId();
            });
        
        if (it == containers_.end()) {
            containers_.push_back(container);
            container->setLocation(shared_from_this());
        }
    }
}

void Location::removeContainer(const UUID& containerId) {
    auto it = std::find_if(containers_.begin(), containers_.end(),
        [&containerId](const std::shared_ptr<Container>& c) {
            return c->getId() == containerId;
        });
    
    if (it != containers_.end()) {
        (*it)->setLocation(nullptr);
        containers_.erase(it);
    }
}

std::shared_ptr<Container> Location::getContainer(const UUID& containerId) const {
    auto it = std::find_if(containers_.begin(), containers_.end(),
        [&containerId](const std::shared_ptr<Container>& c) {
            return c->getId() == containerId;
        });
    
    return (it != containers_.end()) ? *it : nullptr;
}

std::vector<std::shared_ptr<Container>> Location::getAllContainers() const {
    return containers_;
}
