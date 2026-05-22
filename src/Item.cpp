#include "Item.h"
#include "Container.h"
#include "Location.h"
#include "ActivityLog.h"

// New primary constructor - individual item tracking
Item::Item(std::shared_ptr<ItemType> itemType,
           const std::string& individualNotes,
           const std::string& serialNumber,
           const std::string& createdBy)
    : id_(UUID::generate()),
      itemType_(itemType),
      individualNotes_(individualNotes),
      serialNumber_(serialNumber),
      currentContainer_(nullptr),
      originLocation_(nullptr),
      createdBy_(createdBy),
      createdAt_(std::chrono::system_clock::now()),
      checkedOut_(false),
      lastCheckOutTime_(std::chrono::system_clock::now()),
      legacyQuantity_(1) {
}

// Constructor for deserialization
Item::Item(const UUID& id,
           std::shared_ptr<ItemType> itemType,
           const std::string& individualNotes,
           const std::string& serialNumber,
           const std::string& createdBy,
           std::chrono::system_clock::time_point createdAt)
    : id_(id),
      itemType_(itemType),
      individualNotes_(individualNotes),
      serialNumber_(serialNumber),
      currentContainer_(nullptr),
      originLocation_(nullptr),
      createdBy_(createdBy),
      createdAt_(createdAt),
      checkedOut_(false),
      lastCheckOutTime_(std::chrono::system_clock::now()),
      legacyQuantity_(1) {
}

// Legacy constructor for backward compatibility (DEPRECATED)
Item::Item(const std::string& name,
           std::shared_ptr<Category> category,
           int quantity,
           const std::string& description)
    : id_(UUID::generate()),
      itemType_(nullptr),
      individualNotes_(""),
      serialNumber_(""),
      currentContainer_(nullptr),
      originLocation_(nullptr),
      createdBy_("system"),
      createdAt_(std::chrono::system_clock::now()),
      checkedOut_(false),
      lastCheckOutTime_(std::chrono::system_clock::now()),
      legacyName_(name),
      legacyDescription_(description),
      legacyCategory_(category),
      legacyQuantity_(quantity) {
}

// Legacy constructor with UUID for deserialization (DEPRECATED)
Item::Item(const UUID& id,
           const std::string& name,
           std::shared_ptr<Category> category,
           int quantity,
           const std::string& description)
    : id_(id),
      itemType_(nullptr),
      individualNotes_(""),
      serialNumber_(""),
      currentContainer_(nullptr),
      originLocation_(nullptr),
      createdBy_("system"),
      createdAt_(std::chrono::system_clock::now()),
      checkedOut_(false),
      lastCheckOutTime_(std::chrono::system_clock::now()),
      legacyName_(name),
      legacyDescription_(description),
      legacyCategory_(category),
      legacyQuantity_(quantity) {
}

UUID Item::getId() const {
    return id_;
}

std::shared_ptr<ItemType> Item::getItemType() const {
    return itemType_;
}

std::string Item::getName() const {
    if (itemType_) {
        return itemType_->getName();
    }
    return legacyName_;  // Fallback for legacy items
}

std::string Item::getDescription() const {
    std::string desc;
    if (itemType_) {
        desc = itemType_->getDescription();
        if (!individualNotes_.empty()) {
            if (!desc.empty()) {
                desc += "\n";
            }
            desc += "Notes: " + individualNotes_;
        }
    } else {
        desc = legacyDescription_;  // Fallback for legacy items
    }
    return desc;
}

std::string Item::getIndividualNotes() const {
    return individualNotes_;
}

std::string Item::getSerialNumber() const {
    return serialNumber_;
}

std::shared_ptr<Category> Item::getCategory() const {
    if (itemType_) {
        return itemType_->getCategory();
    }
    return legacyCategory_;  // Fallback for legacy items
}

std::shared_ptr<Container> Item::getCurrentContainer() const {
    return currentContainer_;
}

std::shared_ptr<Location> Item::getOriginLocation() const {
    return originLocation_;
}

std::string Item::getCreatedBy() const {
    return createdBy_;
}

std::chrono::system_clock::time_point Item::getCreatedAt() const {
    return createdAt_;
}

void Item::setItemType(std::shared_ptr<ItemType> itemType) {
    itemType_ = itemType;
}

void Item::setIndividualNotes(const std::string& notes) {
    individualNotes_ = notes;
}

void Item::setSerialNumber(const std::string& serialNumber) {
    serialNumber_ = serialNumber;
}

// Legacy setters (DEPRECATED)
void Item::setName(const std::string& name) {
    if (!itemType_) {
        legacyName_ = name;
    }
    // For items with ItemType, name should be changed on the ItemType
}

void Item::setDescription(const std::string& description) {
    if (!itemType_) {
        legacyDescription_ = description;
    } else {
        // Treat as individual notes for new items
        individualNotes_ = description;
    }
}

void Item::setCategory(std::shared_ptr<Category> category) {
    if (!itemType_) {
        legacyCategory_ = category;
    }
    // For items with ItemType, category should be changed on the ItemType
}

void Item::setQuantity(int quantity) {
    // No-op for individual tracking, kept for compatibility
    legacyQuantity_ = 1;
}

void Item::adjustQuantity(int delta) {
    // No-op for individual tracking, kept for compatibility
}

void Item::setContainer(std::shared_ptr<Container> container) {
    currentContainer_ = container;
}

void Item::setOriginLocation(std::shared_ptr<Location> location) {
    originLocation_ = location;
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

void Item::checkOut() {
    checkedOut_ = true;
    lastCheckOutTime_ = std::chrono::system_clock::now();
}

void Item::checkIn() {
    checkedOut_ = false;
}
