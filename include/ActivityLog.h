#ifndef ACTIVITYLOG_H
#define ACTIVITYLOG_H

#include <string>
#include <chrono>
#include <memory>
#include "UUID.h"

class Item;
class Container;
class Project;

enum class ActivityType {
    CHECK_IN,
    CHECK_OUT,
    MOVED,
    QUANTITY_ADJUSTED,
    CREATED,
    MODIFIED,
    DELETED,
    ASSIGNED_TO_PROJECT,
    RETURNED_FROM_PROJECT
};

class ActivityLog {
public:
    ActivityLog(ActivityType type,
                std::shared_ptr<Item> item,
                const std::string& description = "",
                const std::string& userId = "system");
    
    UUID getId() const;
    ActivityType getType() const;
    std::string getDescription() const;
    std::chrono::system_clock::time_point getTimestamp() const;
    std::string getUserId() const;
    std::shared_ptr<Item> getItem() const;
    std::shared_ptr<Container> getFromContainer() const;
    std::shared_ptr<Container> getToContainer() const;
    std::shared_ptr<Project> getProject() const;
    int getQuantityChange() const;
    
    void setFromContainer(std::shared_ptr<Container> container);
    void setToContainer(std::shared_ptr<Container> container);
    void setProject(std::shared_ptr<Project> project);
    void setQuantityChange(int change);
    
    std::string getTypeString() const;
    
private:
    UUID id_;
    ActivityType type_;
    std::string description_;
    std::chrono::system_clock::time_point timestamp_;
    std::string userId_;
    std::shared_ptr<Item> item_;
    std::shared_ptr<Container> fromContainer_;
    std::shared_ptr<Container> toContainer_;
    std::shared_ptr<Project> project_;
    int quantityChange_;
};

#endif // ACTIVITYLOG_H
