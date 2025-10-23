#include "Item.h"
#include "Container.h"
#include "ActivityLog.h"

Item::Item(const std::string& name,
           std::shared_ptr<Category> category,
           int quantity,
           const std::string& description)
    : id_(UUID::generate()),
      name_(name),
      description_(description),
      category_(category),
      quantity_(quantity),
      currentContainer_(nullptr),
      checkedOut_(false),
      lastCheckOutTime_(std::chrono::system_clock::now()) {
}

Item::Item(const UUID& id,
           const std::string& name,
           std::shared_ptr<Category> category,
           int quantity,
           const std::string& description)
    : id_(id),
      name_(name),
      description_(description),
      category_(category),
      quantity_(quantity),
      currentContainer_(nullptr),
      checkedOut_(false),
      lastCheckOutTime_(std::chrono::system_clock::now()) {
}

UUID Item::getId() const {
    return id_;
}

std::string Item::getName() const {
    return name_;
}

std::string Item::getDescription() const {
    return description_;
}

std::shared_ptr<Category> Item::getCategory() const {
    return category_;
}

int Item::getQuantity() const {
    return quantity_;
}

std::shared_ptr<Container> Item::getCurrentContainer() const {
    return currentContainer_;
}

void Item::setName(const std::string& name) {
    name_ = name;
}

void Item::setDescription(const std::string& description) {
    description_ = description;
}

void Item::setCategory(std::shared_ptr<Category> category) {
    category_ = category;
}

void Item::setQuantity(int quantity) {
    if (quantity >= 0) {
        quantity_ = quantity;
    }
}

void Item::adjustQuantity(int delta) {
    quantity_ += delta;
    if (quantity_ < 0) {
        quantity_ = 0;
    }
}

void Item::setContainer(std::shared_ptr<Container> container) {
    currentContainer_ = container;
}

void Item::addActivity(std::shared_ptr<ActivityLog> activity) {
    if (activity) {
        activityHistory_.push_back(activity);
    }
}

std::vector<std::shared_ptr<ActivityLog>> Item::getActivityHistory() const {
    return activityHistory_;
}

bool Item::isCheckedOut() const {
    return checkedOut_;
}

std::chrono::system_clock::time_point Item::getLastCheckOutTime() const {
    return lastCheckOutTime_;
}
