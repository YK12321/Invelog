#ifndef PROJECT_H
#define PROJECT_H

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include "UUID.h"

class Container;
class Item;

enum class ProjectStatus {
    PLANNED,
    IN_PROGRESS,
    ON_HOLD,
    COMPLETED,
    CANCELLED
};

class Project {
public:
    Project(const std::string& name, 
            const std::string& description = "");
    
    UUID getId() const;
    std::string getName() const;
    std::string getDescription() const;
    ProjectStatus getStatus() const;
    std::chrono::system_clock::time_point getCreatedDate() const;
    std::chrono::system_clock::time_point getStartDate() const;
    std::chrono::system_clock::time_point getEndDate() const;
    
    void setName(const std::string& name);
    void setDescription(const std::string& description);
    void setStatus(ProjectStatus status);
    void setStartDate(const std::chrono::system_clock::time_point& date);
    void setEndDate(const std::chrono::system_clock::time_point& date);
    
    // Project-specific container management
    void addContainer(std::shared_ptr<Container> container);
    void removeContainer(const UUID& containerId);
    std::shared_ptr<Container> getContainer(const UUID& containerId) const;
    std::vector<std::shared_ptr<Container>> getAllContainers() const;
    
    // Item allocation tracking
    std::vector<std::shared_ptr<Item>> getAllAllocatedItems() const;
    int getTotalItemCount() const;
    
    std::string getStatusString() const;
    
private:
    UUID id_;
    std::string name_;
    std::string description_;
    ProjectStatus status_;
    std::chrono::system_clock::time_point createdDate_;
    std::chrono::system_clock::time_point startDate_;
    std::chrono::system_clock::time_point endDate_;
    std::vector<std::shared_ptr<Container>> containers_;
};

#endif // PROJECT_H
