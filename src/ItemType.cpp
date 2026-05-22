#include "ItemType.h"

ItemType::ItemType(const std::string& name,
                   std::shared_ptr<Category> category,
                   const std::string& description,
                   const std::string& specifications,
                   const std::string& manufacturer,
                   const std::string& partNumber)
    : id_(UUID::generate()),
      name_(name),
      category_(category),
      description_(description),
      specifications_(specifications),
      manufacturer_(manufacturer),
      partNumber_(partNumber) {
}

ItemType::ItemType(const UUID& id,
                   const std::string& name,
                   std::shared_ptr<Category> category,
                   const std::string& description,
                   const std::string& specifications,
                   const std::string& manufacturer,
                   const std::string& partNumber)
    : id_(id),
      name_(name),
      category_(category),
      description_(description),
      specifications_(specifications),
      manufacturer_(manufacturer),
      partNumber_(partNumber) {
}

UUID ItemType::getId() const {
    return id_;
}

std::string ItemType::getName() const {
    return name_;
}

std::string ItemType::getDescription() const {
    return description_;
}

std::string ItemType::getSpecifications() const {
    return specifications_;
}

std::string ItemType::getManufacturer() const {
    return manufacturer_;
}

std::string ItemType::getPartNumber() const {
    return partNumber_;
}

std::shared_ptr<Category> ItemType::getCategory() const {
    return category_;
}

void ItemType::setName(const std::string& name) {
    name_ = name;
}

void ItemType::setDescription(const std::string& description) {
    description_ = description;
}

void ItemType::setSpecifications(const std::string& specifications) {
    specifications_ = specifications;
}

void ItemType::setManufacturer(const std::string& manufacturer) {
    manufacturer_ = manufacturer;
}

void ItemType::setPartNumber(const std::string& partNumber) {
    partNumber_ = partNumber;
}

void ItemType::setCategory(std::shared_ptr<Category> category) {
    category_ = category;
}
