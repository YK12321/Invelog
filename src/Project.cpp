#include "Project.h"
#include "Container.h"
#include "Item.h"
#include <algorithm>

Project::Project(const std::string& name, const std::string& description)
    : id_(UUID::generate()),
      name_(name),
      description_(description),
      status_(ProjectStatus::PLANNED),
      createdDate_(std::chrono::system_clock::now()),
      startDate_(std::chrono::system_clock::now()),
      endDate_(std::chrono::system_clock::now()) {
}

UUID Project::getId() const {
    return id_;
}

std::string Project::getName() const {
    return name_;
}

std::string Project::getDescription() const {
    return description_;
}

ProjectStatus Project::getStatus() const {
    return status_;
}

std::chrono::system_clock::time_point Project::getCreatedDate() const {
    return createdDate_;
}

std::chrono::system_clock::time_point Project::getStartDate() const {
    return startDate_;
}

std::chrono::system_clock::time_point Project::getEndDate() const {
    return endDate_;
}

void Project::setName(const std::string& name) {
    name_ = name;
}

void Project::setDescription(const std::string& description) {
    description_ = description;
}

void Project::setStatus(ProjectStatus status) {
    status_ = status;
}

void Project::setStartDate(const std::chrono::system_clock::time_point& date) {
    startDate_ = date;
}

void Project::setEndDate(const std::chrono::system_clock::time_point& date) {
    endDate_ = date;
}

void Project::addContainer(std::shared_ptr<Container> container) {
    if (container) {
        // Check if container already exists
        auto it = std::find_if(containers_.begin(), containers_.end(),
            [&container](const std::shared_ptr<Container>& c) {
                return c->getId() == container->getId();
            });
        
        if (it == containers_.end()) {
            containers_.push_back(container);
        }
    }
}

void Project::removeContainer(const UUID& containerId) {
    auto it = std::find_if(containers_.begin(), containers_.end(),
        [&containerId](const std::shared_ptr<Container>& c) {
            return c->getId() == containerId;
        });
    
    if (it != containers_.end()) {
        containers_.erase(it);
    }
}

std::shared_ptr<Container> Project::getContainer(const UUID& containerId) const {
    auto it = std::find_if(containers_.begin(), containers_.end(),
        [&containerId](const std::shared_ptr<Container>& c) {
            return c->getId() == containerId;
        });
    
    return (it != containers_.end()) ? *it : nullptr;
}

std::vector<std::shared_ptr<Container>> Project::getAllContainers() const {
    return containers_;
}

std::vector<std::shared_ptr<Item>> Project::getAllAllocatedItems() const {
    std::vector<std::shared_ptr<Item>> allItems;
    
    for (const auto& container : containers_) {
        auto containerItems = container->getAllItems();
        allItems.insert(allItems.end(), containerItems.begin(), containerItems.end());
        
        // Also get items from subcontainers
        auto subcontainers = container->getAllSubcontainers();
        for (const auto& subcontainer : subcontainers) {
            auto subItems = subcontainer->getAllItems();
            allItems.insert(allItems.end(), subItems.begin(), subItems.end());
        }
    }
    
    return allItems;
}

int Project::getTotalItemCount() const {
    int count = 0;
    
    for (const auto& container : containers_) {
        count += container->getAllItems().size();
        
        // Count items in subcontainers
        auto subcontainers = container->getAllSubcontainers();
        for (const auto& subcontainer : subcontainers) {
            count += subcontainer->getAllItems().size();
        }
    }
    
    return count;
}

std::string Project::getStatusString() const {
    switch (status_) {
        case ProjectStatus::PLANNED:
            return "PLANNED";
        case ProjectStatus::IN_PROGRESS:
            return "IN_PROGRESS";
        case ProjectStatus::ON_HOLD:
            return "ON_HOLD";
        case ProjectStatus::COMPLETED:
            return "COMPLETED";
        case ProjectStatus::CANCELLED:
            return "CANCELLED";
        default:
            return "UNKNOWN";
    }
}
