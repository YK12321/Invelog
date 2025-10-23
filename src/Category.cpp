#include "Category.h"
#include <algorithm>

Category::Category(const std::string& name, const std::string& description)
    : id_(UUID::generate()), name_(name), description_(description) {
}

UUID Category::getId() const {
    return id_;
}

std::string Category::getName() const {
    return name_;
}

std::string Category::getDescription() const {
    return description_;
}

void Category::setName(const std::string& name) {
    name_ = name;
}

void Category::setDescription(const std::string& description) {
    description_ = description;
}

void Category::addSubcategory(std::shared_ptr<Category> subcategory) {
    if (subcategory) {
        // Check if subcategory already exists
        auto it = std::find_if(subcategories_.begin(), subcategories_.end(),
            [&subcategory](const std::shared_ptr<Category>& c) {
                return c->getId() == subcategory->getId();
            });
        
        if (it == subcategories_.end()) {
            subcategories_.push_back(subcategory);
        }
    }
}

void Category::removeSubcategory(const UUID& subcategoryId) {
    auto it = std::find_if(subcategories_.begin(), subcategories_.end(),
        [&subcategoryId](const std::shared_ptr<Category>& c) {
            return c->getId() == subcategoryId;
        });
    
    if (it != subcategories_.end()) {
        subcategories_.erase(it);
    }
}

std::vector<std::shared_ptr<Category>> Category::getSubcategories() const {
    return subcategories_;
}
