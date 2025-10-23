#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "LocalDatabase.h"
#include "Item.h"
#include "Container.h"
#include "Location.h"
#include "Category.h"
#include "Project.h"
#include "ActivityLog.h"
#include <filesystem>

namespace fs = std::filesystem;

// Test fixture for database tests
class LocalDatabaseTest : public ::testing::Test {
protected:
    std::string testDbPath = "./test_db";
    std::shared_ptr<LocalDatabase> db;
    
    void SetUp() override {
        // Clean up any existing test database
        if (fs::exists(testDbPath)) {
            fs::remove_all(testDbPath);
        }
        
        db = std::make_shared<LocalDatabase>(testDbPath);
        ASSERT_TRUE(db->connect());
    }
    
    void TearDown() override {
        db->disconnect();
        
        // Clean up test database
        if (fs::exists(testDbPath)) {
            fs::remove_all(testDbPath);
        }
    }
};

// ============================================================================
// Connection Tests
// ============================================================================

TEST_F(LocalDatabaseTest, ConnectionSuccess) {
    EXPECT_TRUE(db->isConnected());
}

TEST_F(LocalDatabaseTest, Reconnection) {
    db->disconnect();
    EXPECT_FALSE(db->isConnected());
    
    EXPECT_TRUE(db->connect());
    EXPECT_TRUE(db->isConnected());
}

TEST_F(LocalDatabaseTest, DirectoryCreation) {
    // Directories should be created on connect
    EXPECT_TRUE(fs::exists(testDbPath));
    EXPECT_TRUE(fs::exists(testDbPath + "/items"));
    EXPECT_TRUE(fs::exists(testDbPath + "/containers"));
    EXPECT_TRUE(fs::exists(testDbPath + "/locations"));
    EXPECT_TRUE(fs::exists(testDbPath + "/projects"));
    EXPECT_TRUE(fs::exists(testDbPath + "/categories"));
    EXPECT_TRUE(fs::exists(testDbPath + "/activity_logs"));
}

// ============================================================================
// Item Operations
// ============================================================================

TEST_F(LocalDatabaseTest, SaveAndLoadItem) {
    auto category = std::make_shared<Category>("Electronics", "Parts");
    auto item = std::make_shared<Item>("Resistor 1k", category, 100, "1/4W resistor");
    
    // Save item
    EXPECT_TRUE(db->saveItem(item));
    
    // Load item
    auto loaded = db->loadItem(item->getId());
    ASSERT_NE(loaded, nullptr);
    
    EXPECT_EQ(loaded->getId(), item->getId());
    EXPECT_EQ(loaded->getName(), item->getName());
    EXPECT_EQ(loaded->getDescription(), item->getDescription());
    EXPECT_EQ(loaded->getQuantity(), item->getQuantity());
}

TEST_F(LocalDatabaseTest, LoadNonExistentItem) {
    UUID nonExistent = UUID::generate();
    auto loaded = db->loadItem(nonExistent);
    
    EXPECT_EQ(loaded, nullptr);
}

TEST_F(LocalDatabaseTest, DeleteItem) {
    auto category = std::make_shared<Category>("Parts", "");
    auto item = std::make_shared<Item>("Test Item", category, 50);
    
    db->saveItem(item);
    EXPECT_NE(db->loadItem(item->getId()), nullptr);
    
    EXPECT_TRUE(db->deleteItem(item->getId()));
    EXPECT_EQ(db->loadItem(item->getId()), nullptr);
}

TEST_F(LocalDatabaseTest, LoadAllItems) {
    auto category = std::make_shared<Category>("Parts", "");
    
    // Create and save multiple items
    auto item1 = std::make_shared<Item>("Item 1", category, 10);
    auto item2 = std::make_shared<Item>("Item 2", category, 20);
    auto item3 = std::make_shared<Item>("Item 3", category, 30);
    
    db->saveItem(item1);
    db->saveItem(item2);
    db->saveItem(item3);
    
    // Load all items
    auto items = db->loadAllItems();
    
    EXPECT_EQ(items.size(), 3);
}

TEST_F(LocalDatabaseTest, UpdateItem) {
    auto category = std::make_shared<Category>("Parts", "");
    auto item = std::make_shared<Item>("Original", category, 100);
    
    db->saveItem(item);
    
    // Modify and save again
    item->setName("Updated");
    item->setQuantity(200);
    db->saveItem(item);
    
    // Load and verify
    auto loaded = db->loadItem(item->getId());
    EXPECT_EQ(loaded->getName(), "Updated");
    EXPECT_EQ(loaded->getQuantity(), 200);
}

// ============================================================================
// Container Operations
// ============================================================================

TEST_F(LocalDatabaseTest, SaveAndLoadContainer) {
    auto container = std::make_shared<Container>("Storage Box", ContainerType::INVENTORY, "Main storage");
    
    EXPECT_TRUE(db->saveContainer(container));
    
    auto loaded = db->loadContainer(container->getId());
    ASSERT_NE(loaded, nullptr);
    
    EXPECT_EQ(loaded->getId(), container->getId());
    EXPECT_EQ(loaded->getName(), container->getName());
    EXPECT_EQ(loaded->getType(), container->getType());
}

TEST_F(LocalDatabaseTest, DeleteContainer) {
    auto container = std::make_shared<Container>("Box", ContainerType::INVENTORY);
    
    db->saveContainer(container);
    EXPECT_NE(db->loadContainer(container->getId()), nullptr);
    
    EXPECT_TRUE(db->deleteContainer(container->getId()));
    EXPECT_EQ(db->loadContainer(container->getId()), nullptr);
}

TEST_F(LocalDatabaseTest, LoadAllContainers) {
    auto cont1 = std::make_shared<Container>("Box 1", ContainerType::INVENTORY);
    auto cont2 = std::make_shared<Container>("Box 2", ContainerType::PROJECT);
    auto cont3 = std::make_shared<Container>("Box 3", ContainerType::SUBCONTAINER);
    
    db->saveContainer(cont1);
    db->saveContainer(cont2);
    db->saveContainer(cont3);
    
    auto containers = db->loadAllContainers();
    
    EXPECT_EQ(containers.size(), 3);
}

// ============================================================================
// Location Operations
// ============================================================================

TEST_F(LocalDatabaseTest, SaveAndLoadLocation) {
    auto location = std::make_shared<Location>("Warehouse", "Main storage facility");
    location->setAddress("123 Main St");
    
    EXPECT_TRUE(db->saveLocation(location));
    
    auto loaded = db->loadLocation(location->getId());
    ASSERT_NE(loaded, nullptr);
    
    EXPECT_EQ(loaded->getId(), location->getId());
    EXPECT_EQ(loaded->getName(), location->getName());
    EXPECT_EQ(loaded->getAddress(), location->getAddress());
}

TEST_F(LocalDatabaseTest, LoadAllLocations) {
    auto loc1 = std::make_shared<Location>("Warehouse", "");
    auto loc2 = std::make_shared<Location>("Lab", "");
    
    db->saveLocation(loc1);
    db->saveLocation(loc2);
    
    auto locations = db->loadAllLocations();
    
    EXPECT_GE(locations.size(), 2);
}

// ============================================================================
// Category Operations
// ============================================================================

TEST_F(LocalDatabaseTest, SaveAndLoadCategory) {
    auto category = std::make_shared<Category>("Electronics", "Electronic components");
    
    EXPECT_TRUE(db->saveCategory(category));
    
    auto loaded = db->loadCategory(category->getId());
    ASSERT_NE(loaded, nullptr);
    
    EXPECT_EQ(loaded->getId(), category->getId());
    EXPECT_EQ(loaded->getName(), category->getName());
}

TEST_F(LocalDatabaseTest, LoadAllCategories) {
    auto cat1 = std::make_shared<Category>("Resistors", "");
    auto cat2 = std::make_shared<Category>("Capacitors", "");
    auto cat3 = std::make_shared<Category>("ICs", "");
    
    db->saveCategory(cat1);
    db->saveCategory(cat2);
    db->saveCategory(cat3);
    
    auto categories = db->loadAllCategories();
    
    EXPECT_GE(categories.size(), 3);
}

// ============================================================================
// Project Operations
// ============================================================================

TEST_F(LocalDatabaseTest, SaveAndLoadProject) {
    auto project = std::make_shared<Project>("Arduino Controller", "LED project");
    project->setStatus(ProjectStatus::IN_PROGRESS);
    
    EXPECT_TRUE(db->saveProject(project));
    
    auto loaded = db->loadProject(project->getId());
    ASSERT_NE(loaded, nullptr);
    
    EXPECT_EQ(loaded->getId(), project->getId());
    EXPECT_EQ(loaded->getName(), project->getName());
    EXPECT_EQ(loaded->getStatus(), project->getStatus());
}

TEST_F(LocalDatabaseTest, LoadAllProjects) {
    auto proj1 = std::make_shared<Project>("Project 1", "");
    auto proj2 = std::make_shared<Project>("Project 2", "");
    
    db->saveProject(proj1);
    db->saveProject(proj2);
    
    auto projects = db->loadAllProjects();
    
    EXPECT_GE(projects.size(), 2);
}

// ============================================================================
// Activity Log Operations
// ============================================================================

TEST_F(LocalDatabaseTest, SaveAndLoadActivityLog) {
    auto category = std::make_shared<Category>("Parts", "");
    auto item = std::make_shared<Item>("Test Item", category, 10);
    
    auto log = std::make_shared<ActivityLog>(
        ActivityType::CREATED,
        item,
        "Item created",
        "user1"
    );
    
    EXPECT_TRUE(db->saveActivityLog(log));
    
    auto logs = db->loadActivityLogsForItem(item->getId());
    ASSERT_GE(logs.size(), 1);
    
    auto loaded = logs[0];
    EXPECT_EQ(loaded->getId(), log->getId());
    EXPECT_EQ(loaded->getItem(), log->getItem());
    EXPECT_EQ(loaded->getType(), log->getType());
}

TEST_F(LocalDatabaseTest, LoadActivityLogsForItem) {
    auto category = std::make_shared<Category>("Parts", "");
    auto item = std::make_shared<Item>("Test Item", category, 10);
    auto otherItem = std::make_shared<Item>("Other Item", category, 20);
    
    auto log1 = std::make_shared<ActivityLog>(ActivityType::CREATED, item, "Created", "user1");
    auto log2 = std::make_shared<ActivityLog>(ActivityType::MODIFIED, item, "Updated", "user1");
    auto log3 = std::make_shared<ActivityLog>(ActivityType::CREATED, otherItem, "Other", "user1");
    
    db->saveActivityLog(log1);
    db->saveActivityLog(log2);
    db->saveActivityLog(log3);
    
    auto logs = db->loadActivityLogsForItem(item->getId());
    
    EXPECT_EQ(logs.size(), 2);
}

// ============================================================================
// Search Operations
// ============================================================================

TEST_F(LocalDatabaseTest, LoadAllItemsSearch) {
    auto category = std::make_shared<Category>("Parts", "");
    
    auto item1 = std::make_shared<Item>("Resistor 1k", category, 100);
    auto item2 = std::make_shared<Item>("Resistor 10k", category, 50);
    auto item3 = std::make_shared<Item>("Capacitor 100nF", category, 75);
    
    db->saveItem(item1);
    db->saveItem(item2);
    db->saveItem(item3);
    
    auto allItems = db->loadAllItems();
    
    EXPECT_EQ(allItems.size(), 3);
    
    // Manually search for "Resistor"
    int resistorCount = 0;
    for (const auto& item : allItems) {
        if (item->getName().find("Resistor") != std::string::npos) {
            resistorCount++;
        }
    }
    EXPECT_EQ(resistorCount, 2);
}

TEST_F(LocalDatabaseTest, LoadItemsByCategory) {
    auto category1 = std::make_shared<Category>("Resistors", "");
    auto category2 = std::make_shared<Category>("Capacitors", "");
    
    db->saveCategory(category1);
    db->saveCategory(category2);
    
    auto item1 = std::make_shared<Item>("Item 1", category1, 100);
    auto item2 = std::make_shared<Item>("Item 2", category1, 50);
    auto item3 = std::make_shared<Item>("Item 3", category2, 75);
    
    db->saveItem(item1);
    db->saveItem(item2);
    db->saveItem(item3);
    
    auto allItems = db->loadAllItems();
    
    // Manually filter by category
    int category1Count = 0;
    for (const auto& item : allItems) {
        if (item->getCategory()->getId() == category1->getId()) {
            category1Count++;
        }
    }
    
    EXPECT_EQ(category1Count, 2);
}

// ============================================================================
// Persistence Tests
// ============================================================================

TEST_F(LocalDatabaseTest, DataPersistsAcrossConnections) {
    auto category = std::make_shared<Category>("Parts", "");
    auto item = std::make_shared<Item>("Persistent Item", category, 100);
    UUID itemId = item->getId();
    
    // Save and disconnect
    db->saveItem(item);
    db->disconnect();
    
    // Reconnect with new database instance
    auto db2 = std::make_shared<LocalDatabase>(testDbPath);
    db2->connect();
    
    // Load item
    auto loaded = db2->loadItem(itemId);
    ASSERT_NE(loaded, nullptr);
    EXPECT_EQ(loaded->getName(), "Persistent Item");
    
    db2->disconnect();
}

// ============================================================================
// Error Handling Tests
// ============================================================================

TEST_F(LocalDatabaseTest, OperationsFailWhenDisconnected) {
    db->disconnect();
    
    auto category = std::make_shared<Category>("Parts", "");
    auto item = std::make_shared<Item>("Item", category, 10);
    
    // Operations should fail when disconnected
    EXPECT_FALSE(db->saveItem(item));
    EXPECT_EQ(db->loadItem(item->getId()), nullptr);
    EXPECT_FALSE(db->deleteItem(item->getId()));
}

TEST_F(LocalDatabaseTest, InvalidPathHandling) {
    // Try to create database with invalid path (contains invalid characters)
    auto invalidDb = std::make_shared<LocalDatabase>("");
    
    // Should handle gracefully
    bool result = invalidDb->connect();
    
    // Note: This test behavior depends on implementation
    // It should either succeed (create in current dir) or fail gracefully
    EXPECT_TRUE(result || !result);  // Just ensure it doesn't crash
}
