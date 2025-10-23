#include "ActivityLog.h"
#include "Item.h"
#include "Container.h"
#include "Project.h"

ActivityLog::ActivityLog(ActivityType type,
                         std::shared_ptr<Item> item,
                         const std::string& description,
                         const std::string& userId)
    : id_(UUID::generate()),
      type_(type),
      description_(description),
      timestamp_(std::chrono::system_clock::now()),
      userId_(userId),
      item_(item),
      fromContainer_(nullptr),
      toContainer_(nullptr),
      project_(nullptr),
      quantityChange_(0) {
}

UUID ActivityLog::getId() const {
    return id_;
}

ActivityType ActivityLog::getType() const {
    return type_;
}

std::string ActivityLog::getDescription() const {
    return description_;
}

std::chrono::system_clock::time_point ActivityLog::getTimestamp() const {
    return timestamp_;
}

std::string ActivityLog::getUserId() const {
    return userId_;
}

std::shared_ptr<Item> ActivityLog::getItem() const {
    return item_;
}

std::shared_ptr<Container> ActivityLog::getFromContainer() const {
    return fromContainer_;
}

std::shared_ptr<Container> ActivityLog::getToContainer() const {
    return toContainer_;
}

std::shared_ptr<Project> ActivityLog::getProject() const {
    return project_;
}

int ActivityLog::getQuantityChange() const {
    return quantityChange_;
}

void ActivityLog::setFromContainer(std::shared_ptr<Container> container) {
    fromContainer_ = container;
}

void ActivityLog::setToContainer(std::shared_ptr<Container> container) {
    toContainer_ = container;
}

void ActivityLog::setProject(std::shared_ptr<Project> project) {
    project_ = project;
}

void ActivityLog::setQuantityChange(int change) {
    quantityChange_ = change;
}

std::string ActivityLog::getTypeString() const {
    switch (type_) {
        case ActivityType::CHECK_IN:
            return "CHECK_IN";
        case ActivityType::CHECK_OUT:
            return "CHECK_OUT";
        case ActivityType::MOVED:
            return "MOVED";
        case ActivityType::QUANTITY_ADJUSTED:
            return "QUANTITY_ADJUSTED";
        case ActivityType::CREATED:
            return "CREATED";
        case ActivityType::MODIFIED:
            return "MODIFIED";
        case ActivityType::DELETED:
            return "DELETED";
        case ActivityType::ASSIGNED_TO_PROJECT:
            return "ASSIGNED_TO_PROJECT";
        case ActivityType::RETURNED_FROM_PROJECT:
            return "RETURNED_FROM_PROJECT";
        default:
            return "UNKNOWN";
    }
}
