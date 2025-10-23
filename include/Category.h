#ifndef CATEGORY_H
#define CATEGORY_H

#include <string>
#include <vector>
#include <memory>
#include "UUID.h"

class Category {
public:
    explicit Category(const std::string& name, const std::string& description = "");
    
    UUID getId() const;
    std::string getName() const;
    std::string getDescription() const;
    
    void setName(const std::string& name);
    void setDescription(const std::string& description);
    
    // Support for subcategories
    void addSubcategory(std::shared_ptr<Category> subcategory);
    void removeSubcategory(const UUID& subcategoryId);
    std::vector<std::shared_ptr<Category>> getSubcategories() const;
    
private:
    UUID id_;
    std::string name_;
    std::string description_;
    std::vector<std::shared_ptr<Category>> subcategories_;
};

#endif // CATEGORY_H
