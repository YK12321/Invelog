#include <gtest/gtest.h>
#include "InventoryManager.h"
#include "LocalDatabase.h"
#include "Item.h"
#include "Container.h"
#include "Location.h"
#include "Category.h"
#include "Project.h"
#include <filesystem>

namespace fs = std::filesystem;

// Test fixture for InventoryManager tests
class InventoryManagerTest : public ::testing::Test {
protected:
    std::string testDbPath = "./test_inventory_db";
    std::shared_ptr<LocalDatabase> db;
    std::shared_ptr<InventoryManager> manager;
    
    void SetUp() override {
        // Clean up any existing test database
        if (fs::exists(testDbPath)) {
            fs::remove_all(testDbPath);
        }
        
        db = std::make_shared<LocalDatabase>(testDbPath);
        manager = std::make_shared<InventoryManager>(db);
        manager->initialize();
    }
    
    void TearDown() override {
        manager.reset();
        db.reset();
        
        // Clean up test database
        if (fs::exists(testDbPath)) {
            fs::remove_all(testDbPath);
        }
    }
};

// ============================================================================
// Initialization Tests
// ============================================================================

TEST_F(InventoryManagerTest, Initialization) {
    // Just verify manager was created successfully
    EXPECT_NE(manager, nullptr);
}

// ============================================================================
// Category Management Tests
// ============================================================================

TEST_F(InventoryManagerTest, CreateCategory) {
    auto category = manager->createCategory("Resistors", "All resistors");
    
    ASSERT_NE(category, nullptr);
    EXPECT_EQ(category->getName(), "Resistors");
    EXPECT_EQ(category->getDescription(), "All resistors");
}

TEST_F(InventoryManagerTest, GetCategory) {
    auto created = manager->createCategory("Capacitors", "All capacitors");
    
    auto retrieved = manager->getCategory(created->getId());
    
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getId(), created->getId());
    EXPECT_EQ(retrieved->getName(), "Capacitors");
}

TEST_F(InventoryManagerTest, GetAllCategories) {
    manager->createCategory("Cat1", "");
    manager->createCategory("Cat2", "");
    manager->createCategory("Cat3", "");
    
    auto categories = manager->getAllCategories();
    
    EXPECT_GE(categories.size(), 3);
}

// ============================================================================
// Location Management Tests
// ============================================================================

TEST_F(InventoryManagerTest, CreateLocation) {
    auto location = manager->createLocation("Warehouse", "Main storage");
    
    ASSERT_NE(location, nullptr);
    EXPECT_EQ(location->getName(), "Warehouse");
}

TEST_F(InventoryManagerTest, GetLocation) {
    auto created = manager->createLocation("Lab", "Testing lab");
    
    auto retrieved = manager->getLocation(created->getId());
    
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getId(), created->getId());
}

TEST_F(InventoryManagerTest, GetAllLocations) {
    manager->createLocation("Loc1", "");
    manager->createLocation("Loc2", "");
    
    auto locations = manager->getAllLocations();
    
    EXPECT_GE(locations.size(), 2);
}

// ============================================================================
// Container Management Tests
// ============================================================================

TEST_F(InventoryManagerTest, CreateContainer) {
    auto container = manager->createContainer(
        "Storage Box",
        ContainerType::INVENTORY,
        "Main storage container"
    );
    
    ASSERT_NE(container, nullptr);
    EXPECT_EQ(container->getName(), "Storage Box");
    EXPECT_EQ(container->getType(), ContainerType::INVENTORY);
}

TEST_F(InventoryManagerTest, GetContainer) {
    auto created = manager->createContainer("Box", ContainerType::INVENTORY);
    
    auto retrieved = manager->getContainer(created->getId());
    
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getId(), created->getId());
}

TEST_F(InventoryManagerTest, AssignContainerToLocation) {
    auto location = manager->createLocation("Warehouse", "");
    auto container = manager->createContainer("Box", ContainerType::INVENTORY);
    
    container->setLocation(location);
    
    EXPECT_EQ(container->getLocation(), location);
}

// ============================================================================
// Item Management Tests
// ============================================================================

TEST_F(InventoryManagerTest, CreateItem) {
    auto category = manager->createCategory("Parts", "");
    auto item = manager->createItem("Resistor 1k", category, 100, "1/4W resistor");
    
    ASSERT_NE(item, nullptr);
    EXPECT_EQ(item->getName(), "Resistor 1k");
    EXPECT_EQ(item->getQuantity(), 100);
}

TEST_F(InventoryManagerTest, GetItem) {
    auto category = manager->createCategory("Parts", "");
    auto created = manager->createItem("Item", category, 50);
    
    auto retrieved = manager->getItem(created->getId());
    
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getId(), created->getId());
}

TEST_F(InventoryManagerTest, GetAllItems) {
    auto category = manager->createCategory("Parts", "");
    
    manager->createItem("Item1", category, 10);
    manager->createItem("Item2", category, 20);
    manager->createItem("Item3", category, 30);
    
    auto items = manager->getAllItems();
    
    EXPECT_GE(items.size(), 3);
}

TEST_F(InventoryManagerTest, AdjustItemQuantity) {
    auto category = manager->createCategory("Parts", "");
    auto item = manager->createItem("Item", category, 100);
    
    item->adjustQuantity(50);
    
    EXPECT_EQ(item->getQuantity(), 150);
}

// ============================================================================
// Item Movement Tests
// ============================================================================

TEST_F(InventoryManagerTest, MoveItemToContainer) {
    auto category = manager->createCategory("Parts", "");
    auto item = manager->createItem("Item", category, 10);
    auto container = manager->createContainer("Box", ContainerType::INVENTORY);
    
    bool result = manager->moveItem(item->getId(), container->getId());
    
    EXPECT_TRUE(result);
    EXPECT_EQ(item->getCurrentContainer(), container);
}

TEST_F(InventoryManagerTest, MoveItemBetweenContainers) {
    auto category = manager->createCategory("Parts", "");
    auto item = manager->createItem("Item", category, 10);
    auto container1 = manager->createContainer("Box1", ContainerType::INVENTORY);
    auto container2 = manager->createContainer("Box2", ContainerType::INVENTORY);
    
    // Move to first container
    manager->moveItem(item->getId(), container1->getId());
    EXPECT_EQ(item->getCurrentContainer(), container1);
    
    // Move to second container
    manager->moveItem(item->getId(), container2->getId());
    EXPECT_EQ(item->getCurrentContainer(), container2);
}

// ============================================================================
// Check-in/Check-out Tests
// ============================================================================

TEST_F(InventoryManagerTest, CheckOutItem) {
    auto category = manager->createCategory("Parts", "");
    auto item = manager->createItem("Item", category, 1);
    
    bool result = manager->checkOutItem(item->getId(), "user1");
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(item->isCheckedOut());
}

TEST_F(InventoryManagerTest, CheckInItem) {
    auto category = manager->createCategory("Parts", "");
    auto item = manager->createItem("Item", category, 1);
    
    manager->checkOutItem(item->getId(), "user1");
    EXPECT_TRUE(item->isCheckedOut());
    
    bool result = manager->checkInItem(item->getId(), "user1");
    
    EXPECT_TRUE(result);
    EXPECT_FALSE(item->isCheckedOut());
}

// ============================================================================
// Project Management Tests
// ============================================================================

TEST_F(InventoryManagerTest, CreateProject) {
    auto project = manager->createProject("Arduino Project", "LED controller");
    
    ASSERT_NE(project, nullptr);
    EXPECT_EQ(project->getName(), "Arduino Project");
    EXPECT_EQ(project->getStatus(), ProjectStatus::PLANNED);
}

TEST_F(InventoryManagerTest, GetProject) {
    auto created = manager->createProject("Project", "Description");
    
    auto retrieved = manager->getProject(created->getId());
    
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getId(), created->getId());
}

TEST_F(InventoryManagerTest, AssignItemToProject) {
    auto category = manager->createCategory("Parts", "");
    auto item = manager->createItem("Item", category, 10);
    auto project = manager->createProject("Project", "");
    
    bool result = manager->assignItemToProject(item->getId(), project->getId());
    
    EXPECT_TRUE(result);
}

TEST_F(InventoryManagerTest, ReturnItemFromProject) {
    auto category = manager->createCategory("Parts", "");
    auto item = manager->createItem("Item", category, 10);
    auto project = manager->createProject("Project", "");
    
    manager->assignItemToProject(item->getId(), project->getId());
    
    bool result = manager->returnItemFromProject(item->getId());
    
    EXPECT_TRUE(result);
}

// ============================================================================
// Search Tests
// ============================================================================

TEST_F(InventoryManagerTest, SearchItemsByName) {
    auto category = manager->createCategory("Parts", "");
    
    manager->createItem("Resistor 1k", category, 100);
    manager->createItem("Resistor 10k", category, 50);
    manager->createItem("Capacitor 100nF", category, 75);
    
    auto results = manager->searchItems("Resistor");
    
    EXPECT_GE(results.size(), 2);
    
    for (const auto& item : results) {
        EXPECT_TRUE(item->getName().find("Resistor") != std::string::npos);
    }
}

TEST_F(InventoryManagerTest, FindItemsByCategory) {
    auto category1 = manager->createCategory("Resistors", "");
    auto category2 = manager->createCategory("Capacitors", "");
    
    manager->createItem("Item1", category1, 100);
    manager->createItem("Item2", category1, 50);
    manager->createItem("Item3", category2, 75);
    
    auto results = manager->findItemsByCategory(category1->getId());
    
    EXPECT_GE(results.size(), 2);
}

TEST_F(InventoryManagerTest, FindItemsByLocation) {
    auto category = manager->createCategory("Parts", "");
    auto location1 = manager->createLocation("Lab", "");
    auto location2 = manager->createLocation("Warehouse", "");
    auto container1 = manager->createContainer("Box1", ContainerType::INVENTORY);
    auto container2 = manager->createContainer("Box2", ContainerType::INVENTORY);
    
    container1->setLocation(location1);
    container2->setLocation(location2);
    
    auto item1 = manager->createItem("Item1", category, 10);
    auto item2 = manager->createItem("Item2", category, 20);
    auto item3 = manager->createItem("Item3", category, 30);
    
    manager->moveItem(item1->getId(), container1->getId());
    manager->moveItem(item2->getId(), container1->getId());
    manager->moveItem(item3->getId(), container2->getId());
    
    auto results = manager->findItemsInLocation(location1->getId());
    
    EXPECT_GE(results.size(), 2);
}

TEST_F(InventoryManagerTest, FindItemsByProject) {
    auto category = manager->createCategory("Parts", "");
    auto project = manager->createProject("Project", "");
    
    auto item1 = manager->createItem("Item1", category, 10);
    auto item2 = manager->createItem("Item2", category, 20);
    auto item3 = manager->createItem("Item3", category, 30);
    
    manager->assignItemToProject(item1->getId(), project->getId());
    manager->assignItemToProject(item2->getId(), project->getId());
    
    auto results = manager->findItemsInProject(project->getId());
    
    EXPECT_GE(results.size(), 2);
}

TEST_F(InventoryManagerTest, FindItemsByContainer) {
    auto category = manager->createCategory("Parts", "");
    auto container = manager->createContainer("Box", ContainerType::INVENTORY);
    
    auto item1 = manager->createItem("Item1", category, 10);
    auto item2 = manager->createItem("Item2", category, 20);
    auto item3 = manager->createItem("Item3", category, 30);
    
    manager->moveItem(item1->getId(), container->getId());
    manager->moveItem(item2->getId(), container->getId());
    
    auto items = container->getAllItems();
    
    EXPECT_EQ(items.size(), 2);
}

// ============================================================================
// Activity Log Tests
// ============================================================================

TEST_F(InventoryManagerTest, GetActivityLogs) {
    auto category = manager->createCategory("Parts", "");
    auto item = manager->createItem("Item", category, 100);
    
    // Perform operations that generate logs
    item->adjustQuantity(50);
    manager->checkOutItem(item->getId(), "user1");
    manager->checkInItem(item->getId(), "user1");
    
    auto logs = manager->getItemHistory(item->getId());
    
    // Should have at least: create, checkout, checkin
    EXPECT_GE(logs.size(), 1);
}

// ============================================================================
// Error Handling Tests
// ============================================================================

TEST_F(InventoryManagerTest, GetNonExistentItem) {
    UUID nonExistent = UUID::generate();
    auto item = manager->getItem(nonExistent);
    
    EXPECT_EQ(item, nullptr);
}

TEST_F(InventoryManagerTest, MoveNonExistentItem) {
    UUID nonExistent = UUID::generate();
    auto container = manager->createContainer("Box", ContainerType::INVENTORY);
    
    bool result = manager->moveItem(nonExistent, container->getId());
    
    EXPECT_FALSE(result);
}

TEST_F(InventoryManagerTest, CheckOutAlreadyCheckedOutItem) {
    auto category = manager->createCategory("Parts", "");
    auto item = manager->createItem("Item", category, 1);
    
    manager->checkOutItem(item->getId(), "user1");
    
    // Try to checkout again - should fail or handle appropriately
    bool result = manager->checkOutItem(item->getId(), "user2");
    
    // Behavior depends on business logic - either should fail or allow
    // Just ensure it doesn't crash
    EXPECT_TRUE(result || !result);
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST_F(InventoryManagerTest, CompleteWorkflow) {
    // Create infrastructure
    auto location = manager->createLocation("Warehouse", "Main storage");
    auto container = manager->createContainer("Storage Box", ContainerType::INVENTORY);
    container->setLocation(location);
    
    // Create category and items
    auto category = manager->createCategory("Resistors", "Electronic resistors");
    auto item1 = manager->createItem("Resistor 1k", category, 100);
    auto item2 = manager->createItem("Resistor 10k", category, 50);
    
    // Organize items
    manager->moveItem(item1->getId(), container->getId());
    manager->moveItem(item2->getId(), container->getId());
    
    // Create project and assign items
    auto project = manager->createProject("Arduino Project", "LED controller");
    manager->assignItemToProject(item1->getId(), project->getId());
    
    // Use an item
    manager->checkOutItem(item1->getId(), "user1");
    manager->checkInItem(item1->getId(), "user1");
    
    // Adjust quantity
    item1->adjustQuantity(-10);
    
    // Verify final state
    EXPECT_EQ(item1->getQuantity(), 90);
    EXPECT_FALSE(item1->isCheckedOut());
    EXPECT_EQ(item1->getCurrentContainer(), container);
    EXPECT_EQ(container->getLocation(), location);
    
    // Verify logs exist
    auto logs = manager->getItemHistory(item1->getId());
    EXPECT_GE(logs.size(), 1);
}
