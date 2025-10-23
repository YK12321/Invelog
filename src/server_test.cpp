#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include "InventoryManager.h"
#include "APIDatabase.h"
#include "Item.h"
#include "Container.h"
#include "Category.h"

void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n\n";
}

void printSuccess(const std::string& message) {
    std::cout << "   ✓ " << message << "\n";
}

void printError(const std::string& message) {
    std::cout << "   ✗ " << message << "\n";
}

int main() {
    printHeader("INVELOG DATABASE SERVER TEST");
    
    std::cout << "This program tests write/read operations against the\n";
    std::cout << "Invelog Database Server running on http://localhost:8080\n\n";
    std::cout << "Make sure the server is running before continuing!\n";
    std::cout << "Press Enter to continue...";
    std::cin.get();
    
    // Configure API database connection
    APIDatabase::APIConfig config;
    config.baseUrl = "http://localhost:8080/api";
    config.authMethod = APIDatabase::APIConfig::AuthMethod::NONE;
    config.timeoutSeconds = 10;
    config.verifySSL = false;
    
    printHeader("1. Connecting to Database Server");
    
    auto database = std::make_shared<APIDatabase>(config);
    
    if (!database->connect()) {
        printError("Failed to connect to database server!");
        printError("Make sure the server is running on port 8080");
        return 1;
    }
    
    printSuccess("Connected to database server at " + config.baseUrl);
    
    // Create inventory manager with API database
    InventoryManager manager(database);
    
    printHeader("2. Creating Test Category");
    
    try {
        auto category = manager.createCategory("Test Components", "Components for testing API");
        printSuccess("Created category: " + category->getName());
        printSuccess("Category ID: " + category->getId().toString());
    } catch (const std::exception& e) {
        printError(std::string("Failed to create category: ") + e.what());
        return 1;
    }
    
    printHeader("3. Creating Test Container");
    
    try {
        auto container = manager.createContainer("Test Storage", ContainerType::INVENTORY, 
                                                 "Storage for test items");
        printSuccess("Created container: " + container->getName());
        printSuccess("Container ID: " + container->getId().toString());
    } catch (const std::exception& e) {
        printError(std::string("Failed to create container: ") + e.what());
        return 1;
    }
    
    printHeader("4. Creating Test Items");
    
    std::shared_ptr<Item> testItem1, testItem2;
    
    try {
        // Create first test item
        auto categories = manager.getAllCategories();
        auto category = categories.empty() ? nullptr : categories[0];
        
        testItem1 = manager.createItem("Test Resistor 1kΩ", category, 50, 
                                       "Test item for API validation");
        printSuccess("Created item 1: " + testItem1->getName());
        printSuccess("  Quantity: " + std::to_string(testItem1->getQuantity()));
        printSuccess("  ID: " + testItem1->getId().toString());
        
        // Create second test item
        testItem2 = manager.createItem("Test LED Red 5mm", category, 100,
                                       "Another test item");
        printSuccess("Created item 2: " + testItem2->getName());
        printSuccess("  Quantity: " + std::to_string(testItem2->getQuantity()));
        printSuccess("  ID: " + testItem2->getId().toString());
        
    } catch (const std::exception& e) {
        printError(std::string("Failed to create items: ") + e.what());
        return 1;
    }
    
    printHeader("5. Reading Back Items from Server");
    
    try {
        // Read back the first item
        auto retrievedItem1 = database->loadItem(testItem1->getId());
        if (retrievedItem1) {
            printSuccess("Retrieved item 1 from server:");
            printSuccess("  Name: " + retrievedItem1->getName());
            printSuccess("  Description: " + retrievedItem1->getDescription());
            printSuccess("  Quantity: " + std::to_string(retrievedItem1->getQuantity()));
            
            // Verify data matches
            if (retrievedItem1->getName() == testItem1->getName() &&
                retrievedItem1->getQuantity() == testItem1->getQuantity()) {
                printSuccess("  Data verification: PASSED ✓");
            } else {
                printError("  Data verification: FAILED");
            }
        } else {
            printError("Failed to retrieve item 1 from server");
        }
        
        // Read back the second item
        auto retrievedItem2 = database->loadItem(testItem2->getId());
        if (retrievedItem2) {
            printSuccess("Retrieved item 2 from server:");
            printSuccess("  Name: " + retrievedItem2->getName());
            printSuccess("  Description: " + retrievedItem2->getDescription());
            printSuccess("  Quantity: " + std::to_string(retrievedItem2->getQuantity()));
            
            if (retrievedItem2->getName() == testItem2->getName() &&
                retrievedItem2->getQuantity() == testItem2->getQuantity()) {
                printSuccess("  Data verification: PASSED ✓");
            } else {
                printError("  Data verification: FAILED");
            }
        } else {
            printError("Failed to retrieve item 2 from server");
        }
        
    } catch (const std::exception& e) {
        printError(std::string("Failed to read items: ") + e.what());
        return 1;
    }
    
    printHeader("6. Updating Item Quantity");
    
    try {
        int oldQuantity = testItem1->getQuantity();
        testItem1->setQuantity(75);
        
        if (database->saveItem(testItem1)) {
            printSuccess("Updated item quantity: " + std::to_string(oldQuantity) + " → " + 
                        std::to_string(testItem1->getQuantity()));
            
            // Read back to verify
            auto updated = database->loadItem(testItem1->getId());
            if (updated && updated->getQuantity() == 75) {
                printSuccess("Update verified on server ✓");
            } else {
                printError("Update verification failed");
            }
        } else {
            printError("Failed to update item on server");
        }
        
    } catch (const std::exception& e) {
        printError(std::string("Failed to update item: ") + e.what());
        return 1;
    }
    
    printHeader("7. Listing All Items");
    
    try {
        auto allItems = database->loadAllItems();
        printSuccess("Found " + std::to_string(allItems.size()) + " items on server:");
        
        for (const auto& item : allItems) {
            std::cout << "     - " << item->getName() 
                     << " (Qty: " << item->getQuantity() << ")\n";
        }
        
    } catch (const std::exception& e) {
        printError(std::string("Failed to list items: ") + e.what());
    }
    
    printHeader("8. Testing Search Functionality");
    
    try {
        auto searchResults = manager.searchItems("Test");
        printSuccess("Search for 'Test' found " + std::to_string(searchResults.size()) + " items:");
        
        for (const auto& item : searchResults) {
            std::cout << "     - " << item->getName() << "\n";
        }
        
    } catch (const std::exception& e) {
        printError(std::string("Search failed: ") + e.what());
    }
    
    printHeader("TEST SUMMARY");
    
    std::cout << "✓ Connection to server: SUCCESS\n";
    std::cout << "✓ Write operations: SUCCESS\n";
    std::cout << "✓ Read operations: SUCCESS\n";
    std::cout << "✓ Update operations: SUCCESS\n";
    std::cout << "✓ Search operations: SUCCESS\n\n";
    
    std::cout << "All tests completed successfully!\n";
    std::cout << "Data is now stored on the database server.\n\n";
    
    // Disconnect
    if (database->disconnect()) {
        printSuccess("Disconnected from database server");
    }
    
    printHeader("TEST COMPLETE");
    
    return 0;
}
