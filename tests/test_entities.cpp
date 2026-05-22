#include <gtest/gtest.h>
#include "Category.h"
#include "Location.h"
#include "Container.h"
#include "Item.h"
#include "Project.h"
#include "ActivityLog.h"

// ============================================================================
// Category Tests
// ============================================================================

TEST(CategoryTest, Creation) {
    auto category = std::make_shared<Category>("Resistors", "Electronic resistors");
    
    EXPECT_EQ(category->getName(), "Resistors");
    EXPECT_EQ(category->getDescription(), "Electronic resistors");
    EXPECT_FALSE(category->getId().toString().empty());
}

TEST(CategoryTest, SettersGetters) {
    auto category = std::make_shared<Category>("Initial", "Description");
    
    category->setName("Updated");
    category->setDescription("New description");
    
    EXPECT_EQ(category->getName(), "Updated");
    EXPECT_EQ(category->getDescription(), "New description");
}

TEST(CategoryTest, SubcategoryManagement) {
    auto parent = std::make_shared<Category>("Electronics", "All electronics");
    auto child = std::make_shared<Category>("Resistors", "Resistors only");
    
    parent->addSubcategory(child);
    
    auto subcats = parent->getSubcategories();
    EXPECT_EQ(subcats.size(), 1);
    EXPECT_EQ(subcats[0], child);
    
    parent->removeSubcategory(child->getId());
    subcats = parent->getSubcategories();
    EXPECT_EQ(subcats.size(), 0);
}

// ============================================================================
// Location Tests
// ============================================================================

TEST(LocationTest, Creation) {
    auto location = std::make_shared<Location>("Warehouse", "123 Main St");
    
    EXPECT_EQ(location->getName(), "Warehouse");
    EXPECT_EQ(location->getAddress(), "123 Main St");
    EXPECT_FALSE(location->getId().toString().empty());
}

TEST(LocationTest, SettersGetters) {
    auto location = std::make_shared<Location>("Lab", "456 Oak Ave");
    
    location->setName("Updated Lab");
    location->setAddress("789 Pine St");
    
    EXPECT_EQ(location->getName(), "Updated Lab");
    EXPECT_EQ(location->getAddress(), "789 Pine St");
}

TEST(LocationTest, ContainerManagement) {
    auto location = std::make_shared<Location>("Warehouse", "Storage");
    auto container = std::make_shared<Container>("Box1", ContainerType::INVENTORY, "Storage box");
    
    location->addContainer(container);
    
    auto containers = location->getAllContainers();
    EXPECT_EQ(containers.size(), 1);
    EXPECT_EQ(containers[0], container);
    
    auto retrieved = location->getContainer(container->getId());
    EXPECT_EQ(retrieved, container);
    
    location->removeContainer(container->getId());
    containers = location->getAllContainers();
    EXPECT_EQ(containers.size(), 0);
}

// ============================================================================
// Container Tests
// ============================================================================

TEST(ContainerTest, Creation) {
    auto container = std::make_shared<Container>("Box1", ContainerType::INVENTORY, "Storage");
    
    EXPECT_EQ(container->getName(), "Box1");
    EXPECT_EQ(container->getDescription(), "Storage");
    EXPECT_EQ(container->getType(), ContainerType::INVENTORY);
    EXPECT_FALSE(container->getId().toString().empty());
}

TEST(ContainerTest, TypeEnums) {
    auto inventory = std::make_shared<Container>("Inv", ContainerType::INVENTORY);
    auto project = std::make_shared<Container>("Proj", ContainerType::PROJECT);
    auto sub = std::make_shared<Container>("Sub", ContainerType::SUBCONTAINER);
    
    EXPECT_EQ(inventory->getType(), ContainerType::INVENTORY);
    EXPECT_EQ(project->getType(), ContainerType::PROJECT);
    EXPECT_EQ(sub->getType(), ContainerType::SUBCONTAINER);
}

TEST(ContainerTest, ItemManagement) {
    auto container = std::make_shared<Container>("Box", ContainerType::INVENTORY);
    auto category = std::make_shared<Category>("Parts", "");
    auto item = std::make_shared<Item>("Resistor", category, 100);
    
    container->addItem(item);
    
    auto items = container->getAllItems();
    EXPECT_EQ(items.size(), 1);
    EXPECT_EQ(items[0], item);
    
    auto retrieved = container->getItem(item->getId());
    EXPECT_EQ(retrieved, item);
    
    container->removeItem(item->getId());
    items = container->getAllItems();
    EXPECT_EQ(items.size(), 0);
}

TEST(ContainerTest, SubcontainerHierarchy) {
    auto parent = std::make_shared<Container>("Parent", ContainerType::INVENTORY);
    auto child = std::make_shared<Container>("Child", ContainerType::SUBCONTAINER);
    
    child->setParentContainer(parent);
    parent->addSubcontainer(child);
    
    EXPECT_EQ(child->getParentContainer(), parent);
    
    auto subs = parent->getAllSubcontainers();
    EXPECT_EQ(subs.size(), 1);
    EXPECT_EQ(subs[0], child);
}

TEST(ContainerTest, LocationAssociation) {
    auto container = std::make_shared<Container>("Box", ContainerType::INVENTORY);
    auto location = std::make_shared<Location>("Lab", "");
    
    container->setLocation(location);
    
    EXPECT_EQ(container->getLocation(), location);
}

TEST(ContainerTest, SearchFunctions) {
    auto container = std::make_shared<Container>("Box", ContainerType::INVENTORY);
    auto category = std::make_shared<Category>("Parts", "");
    auto item1 = std::make_shared<Item>("Resistor 1k", category, 100);
    auto item2 = std::make_shared<Item>("Resistor 10k", category, 50);
    auto item3 = std::make_shared<Item>("Capacitor", category, 75);
    
    container->addItem(item1);
    container->addItem(item2);
    container->addItem(item3);
    
    auto found = container->findItemsByName("Resistor");
    EXPECT_GE(found.size(), 2);
    
    auto byCategory = container->findItemsByCategory(category->getId());
    EXPECT_EQ(byCategory.size(), 3);
}

// ============================================================================
// Item Tests
// ============================================================================

TEST(ItemTest, Creation) {
    auto category = std::make_shared<Category>("Electronics", "");
    auto item = std::make_shared<Item>("Resistor 1k", category, 100, "1/4W resistor");
    
    EXPECT_EQ(item->getName(), "Resistor 1k");
    EXPECT_EQ(item->getDescription(), "1/4W resistor");
    EXPECT_EQ(item->getQuantity(), 100);
    EXPECT_EQ(item->getCategory(), category);
    EXPECT_FALSE(item->isCheckedOut());
}

TEST(ItemTest, UUIDConstructor) {
    auto category = std::make_shared<Category>("Electronics", "");
    UUID customId = UUID::generate();
    
    auto item = std::make_shared<Item>(customId, "Resistor", category, 50, "Test");
    
    EXPECT_EQ(item->getId(), customId);
    EXPECT_EQ(item->getName(), "Resistor");
    EXPECT_EQ(item->getQuantity(), 50);
}

TEST(ItemTest, QuantityManagement) {
    auto category = std::make_shared<Category>("Parts", "");
    auto item = std::make_shared<Item>("Item", category, 100);
    
    item->setQuantity(150);
    EXPECT_EQ(item->getQuantity(), 150);
    
    item->adjustQuantity(50);
    EXPECT_EQ(item->getQuantity(), 200);
    
    item->adjustQuantity(-75);
    EXPECT_EQ(item->getQuantity(), 125);
}

TEST(ItemTest, ContainerAssociation) {
    auto category = std::make_shared<Category>("Parts", "");
    auto item = std::make_shared<Item>("Item", category, 10);
    auto container = std::make_shared<Container>("Box", ContainerType::INVENTORY);
    
    item->setContainer(container);
    
    EXPECT_EQ(item->getCurrentContainer(), container);
}

TEST(ItemTest, ActivityTracking) {
    auto category = std::make_shared<Category>("Parts", "");
    auto item = std::make_shared<Item>("Item", category, 10);
    auto activity = std::make_shared<ActivityLog>(
        ActivityType::CREATED,
        item,
        "Item created",
        "user1"
    );
    
    item->addActivity(activity);
    
    auto logs = item->getActivityHistory();
    EXPECT_EQ(logs.size(), 1);
    EXPECT_EQ(logs[0], activity);
}

// ============================================================================
// Project Tests
// ============================================================================

TEST(ProjectTest, Creation) {
    auto project = std::make_shared<Project>("Arduino Project", "LED controller");
    
    EXPECT_EQ(project->getName(), "Arduino Project");
    EXPECT_EQ(project->getDescription(), "LED controller");
    EXPECT_EQ(project->getStatus(), ProjectStatus::PLANNED);
    EXPECT_FALSE(project->getId().toString().empty());
}

TEST(ProjectTest, StatusManagement) {
    auto project = std::make_shared<Project>("Project", "");
    
    EXPECT_EQ(project->getStatus(), ProjectStatus::PLANNED);
    
    project->setStatus(ProjectStatus::IN_PROGRESS);
    EXPECT_EQ(project->getStatus(), ProjectStatus::IN_PROGRESS);
    
    project->setStatus(ProjectStatus::COMPLETED);
    EXPECT_EQ(project->getStatus(), ProjectStatus::COMPLETED);
    
    // Test status string
    EXPECT_FALSE(project->getStatusString().empty());
}

TEST(ProjectTest, ContainerManagement) {
    auto project = std::make_shared<Project>("Project", "");
    auto container = std::make_shared<Container>("Box", ContainerType::PROJECT);
    
    project->addContainer(container);
    
    auto containers = project->getAllContainers();
    EXPECT_EQ(containers.size(), 1);
    EXPECT_EQ(containers[0], container);
    
    auto retrieved = project->getContainer(container->getId());
    EXPECT_EQ(retrieved, container);
    
    project->removeContainer(container->getId());
    containers = project->getAllContainers();
    EXPECT_EQ(containers.size(), 0);
}

TEST(ProjectTest, ItemTracking) {
    auto project = std::make_shared<Project>("Project", "");
    auto category = std::make_shared<Category>("Parts", "");
    auto container = std::make_shared<Container>("Box", ContainerType::PROJECT);
    
    project->addContainer(container);
    
    auto item1 = std::make_shared<Item>("Item1", category, 10);
    auto item2 = std::make_shared<Item>("Item2", category, 20);
    
    container->addItem(item1);
    container->addItem(item2);
    
    auto items = project->getAllAllocatedItems();
    EXPECT_EQ(items.size(), 2);
    
    int totalCount = project->getTotalItemCount();
    EXPECT_EQ(totalCount, 30);  // 10 + 20
}

TEST(ProjectTest, DateTracking) {
    auto project = std::make_shared<Project>("Project", "");
    
    // Created date should be set
    auto created = project->getCreatedDate();
    EXPECT_NE(created.time_since_epoch().count(), 0);
    
    auto startDate = std::chrono::system_clock::now() + std::chrono::hours(24);
    project->setStartDate(startDate);
    EXPECT_EQ(project->getStartDate(), startDate);
    
    auto endDate = std::chrono::system_clock::now() + std::chrono::hours(48);
    project->setEndDate(endDate);
    EXPECT_EQ(project->getEndDate(), endDate);
}

// ============================================================================
// ActivityLog Tests
// ============================================================================

TEST(ActivityLogTest, Creation) {
    auto category = std::make_shared<Category>("Parts", "");
    auto item = std::make_shared<Item>("Test Item", category, 10);
    
    auto log = std::make_shared<ActivityLog>(
        ActivityType::CREATED,
        item,
        "Item was created",
        "user1"
    );
    
    EXPECT_EQ(log->getItem(), item);
    EXPECT_EQ(log->getType(), ActivityType::CREATED);
    EXPECT_EQ(log->getUserId(), "user1");
    EXPECT_EQ(log->getDescription(), "Item was created");
    EXPECT_NE(log->getTimestamp().time_since_epoch().count(), 0);
}

TEST(ActivityLogTest, AllActivityTypes) {
    auto category = std::make_shared<Category>("Parts", "");
    auto item = std::make_shared<Item>("Item", category, 10);
    
    // Test all activity types
    std::vector<ActivityType> types = {
        ActivityType::CHECK_IN,
        ActivityType::CHECK_OUT,
        ActivityType::MOVED,
        ActivityType::QUANTITY_ADJUSTED,
        ActivityType::CREATED,
        ActivityType::MODIFIED,
        ActivityType::DELETED,
        ActivityType::ASSIGNED_TO_PROJECT,
        ActivityType::RETURNED_FROM_PROJECT
    };
    
    for (auto type : types) {
        auto log = std::make_shared<ActivityLog>(type, item, "Test", "user1");
        EXPECT_EQ(log->getType(), type);
        EXPECT_FALSE(log->getTypeString().empty());
    }
}

TEST(ActivityLogTest, ContainerTracking) {
    auto category = std::make_shared<Category>("Parts", "");
    auto item = std::make_shared<Item>("Item", category, 10);
    auto fromContainer = std::make_shared<Container>("Box1", ContainerType::INVENTORY);
    auto toContainer = std::make_shared<Container>("Box2", ContainerType::INVENTORY);
    
    auto log = std::make_shared<ActivityLog>(
        ActivityType::MOVED,
        item,
        "Moved item",
        "user1"
    );
    
    log->setFromContainer(fromContainer);
    log->setToContainer(toContainer);
    
    EXPECT_EQ(log->getFromContainer(), fromContainer);
    EXPECT_EQ(log->getToContainer(), toContainer);
}

TEST(ActivityLogTest, QuantityTracking) {
    auto category = std::make_shared<Category>("Parts", "");
    auto item = std::make_shared<Item>("Item", category, 100);
    
    auto log = std::make_shared<ActivityLog>(
        ActivityType::QUANTITY_ADJUSTED,
        item,
        "Adjusted quantity",
        "user1"
    );
    
    log->setQuantityChange(50);
    
    EXPECT_EQ(log->getQuantityChange(), 50);
}

TEST(ActivityLogTest, ProjectTracking) {
    auto category = std::make_shared<Category>("Parts", "");
    auto item = std::make_shared<Item>("Item", category, 10);
    auto project = std::make_shared<Project>("Project", "");
    
    auto log = std::make_shared<ActivityLog>(
        ActivityType::ASSIGNED_TO_PROJECT,
        item,
        "Assigned to project",
        "user1"
    );
    
    log->setProject(project);
    
    EXPECT_EQ(log->getProject(), project);
}
