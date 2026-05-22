#include "APIDatabase.h"
#include "InventoryManager.h"
#include <iostream>
#include <thread>
#include <chrono>

/**
 * @brief Example client application demonstrating database server usage
 * 
 * This demonstrates how to connect to the Invelog Database Server
 * from a client application using the APIDatabase class.
 */

void printSeparator() {
    std::cout << "\n================================================\n" << std::endl;
}

void demonstrateBasicConnection() {
    std::cout << "1. BASIC CONNECTION" << std::endl;
    std::cout << "-------------------" << std::endl;
    
    // Configure connection to server
    APIDatabase::APIConfig config;
    config.baseUrl = "http://localhost:8080";
    config.authType = APIDatabase::AuthType::API_KEY;
    config.authToken = "mySecretKey";
    config.timeout = 30;
    
    std::cout << "Connecting to: " << config.baseUrl << std::endl;
    std::cout << "Authentication: API Key" << std::endl;
    
    auto database = std::make_shared<APIDatabase>(config);
    
    if (database->initialize()) {
        std::cout << "✓ Successfully connected to database server!" << std::endl;
    } else {
        std::cout << "✗ Failed to connect to database server" << std::endl;
        std::cout << "  Make sure the server is running:" << std::endl;
        std::cout << "  invelog_server --local ./data --api-key mySecretKey" << std::endl;
    }
    
    database->shutdown();
}

void demonstrateInventoryOperations() {
    std::cout << "2. INVENTORY OPERATIONS VIA API" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    
    // Create API database connection
    APIDatabase::APIConfig config;
    config.baseUrl = "http://localhost:8080";
    config.authType = APIDatabase::AuthType::API_KEY;
    config.authToken = "mySecretKey";
    
    auto database = std::make_shared<APIDatabase>(config);
    database->initialize();
    
    // Use InventoryManager with remote database
    InventoryManager manager(database);
    manager.initialize();
    
    std::cout << "Creating inventory entities via API..." << std::endl;
    
    // Create category
    auto category = manager.createCategory("Remote Resistors");
    if (category) {
        std::cout << "✓ Created category: " << category->getName() << std::endl;
    }
    
    // Create location
    auto location = manager.createLocation("Remote Warehouse");
    if (location) {
        std::cout << "✓ Created location: " << location->getName() << std::endl;
    }
    
    // Create container
    auto container = manager.createContainer("Remote Storage Box");
    if (container) {
        std::cout << "✓ Created container: " << container->getName() << std::endl;
        location->addContainer(container);
    }
    
    // Create items
    for (int i = 1; i <= 3; ++i) {
        std::string name = "Resistor " + std::to_string(i) + "kΩ";
        auto item = manager.createItem(name, category, 50 + i * 10);
        if (item) {
            std::cout << "✓ Created item: " << item->getName() 
                      << " (Qty: " << item->getQuantity() << ")" << std::endl;
            manager.moveItem(item->getId(), container->getId());
        }
    }
    
    std::cout << "\nInventory created successfully on remote server!" << std::endl;
    
    manager.shutdown();
}

void demonstrateSearchAndQuery() {
    std::cout << "3. SEARCH AND QUERY OPERATIONS" << std::endl;
    std::cout << "-------------------------------" << std::endl;
    
    APIDatabase::APIConfig config;
    config.baseUrl = "http://localhost:8080";
    config.authType = APIDatabase::AuthType::API_KEY;
    config.authToken = "mySecretKey";
    
    auto database = std::make_shared<APIDatabase>(config);
    database->initialize();
    
    InventoryManager manager(database);
    manager.initialize();
    
    std::cout << "Searching for items via API..." << std::endl;
    
    // Search by name
    auto results = manager.searchItems("Resistor");
    std::cout << "\nFound " << results.size() << " items matching 'Resistor':" << std::endl;
    for (const auto& item : results) {
        std::cout << "  - " << item->getName() << " (Qty: " << item->getQuantity() << ")" << std::endl;
    }
    
    manager.shutdown();
}

void demonstrateProjectManagement() {
    std::cout << "4. PROJECT MANAGEMENT VIA API" << std::endl;
    std::cout << "------------------------------" << std::endl;
    
    APIDatabase::APIConfig config;
    config.baseUrl = "http://localhost:8080";
    config.authType = APIDatabase::AuthType::API_KEY;
    config.authToken = "mySecretKey";
    
    auto database = std::make_shared<APIDatabase>(config);
    database->initialize();
    
    InventoryManager manager(database);
    manager.initialize();
    
    // Create project
    auto project = manager.createProject("Remote Arduino Project", "LED controller");
    if (project) {
        std::cout << "✓ Created project: " << project->getName() << std::endl;
        
        // Get items and assign to project
        auto items = manager.searchItems("Resistor");
        if (!items.empty()) {
            auto item = items[0];
            if (manager.assignItemToProject(item->getId(), project->getId(), 5)) {
                std::cout << "✓ Assigned " << item->getName() << " to project" << std::endl;
            }
        }
        
        std::cout << "\nProject created and items allocated via API!" << std::endl;
    }
    
    manager.shutdown();
}

void demonstrateMultipleClients() {
    std::cout << "5. MULTIPLE CLIENT CONNECTIONS" << std::endl;
    std::cout << "-------------------------------" << std::endl;
    std::cout << "Simulating multiple clients connecting to same server..." << std::endl;
    
    // Client 1: Creating items
    std::thread client1([]{
        APIDatabase::APIConfig config;
        config.baseUrl = "http://localhost:8080";
        config.authType = APIDatabase::AuthType::API_KEY;
        config.authToken = "mySecretKey";
        
        auto database = std::make_shared<APIDatabase>(config);
        database->initialize();
        
        InventoryManager manager(database);
        manager.initialize();
        
        auto category = manager.createCategory("Client1 Category");
        auto item = manager.createItem("Client1 Item", category, 100);
        
        std::cout << "[Client 1] ✓ Created item: " << (item ? item->getName() : "Failed") << std::endl;
        
        manager.shutdown();
    });
    
    // Client 2: Querying items
    std::thread client2([]{
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        APIDatabase::APIConfig config;
        config.baseUrl = "http://localhost:8080";
        config.authType = APIDatabase::AuthType::API_KEY;
        config.authToken = "mySecretKey";
        
        auto database = std::make_shared<APIDatabase>(config);
        database->initialize();
        
        InventoryManager manager(database);
        manager.initialize();
        
        auto items = database->loadAllItems();
        std::cout << "[Client 2] ✓ Found " << items.size() << " total items" << std::endl;
        
        manager.shutdown();
    });
    
    client1.join();
    client2.join();
    
    std::cout << "\nMultiple clients successfully interacted with server!" << std::endl;
}

void demonstrateErrorHandling() {
    std::cout << "6. ERROR HANDLING" << std::endl;
    std::cout << "-----------------" << std::endl;
    
    // Try to connect with wrong API key
    std::cout << "Attempting connection with invalid API key..." << std::endl;
    
    APIDatabase::APIConfig config;
    config.baseUrl = "http://localhost:8080";
    config.authType = APIDatabase::AuthType::API_KEY;
    config.authToken = "wrongKey";
    
    auto database = std::make_shared<APIDatabase>(config);
    
    if (!database->initialize()) {
        std::cout << "✓ Correctly rejected invalid authentication" << std::endl;
    }
    
    // Try to connect to wrong URL
    std::cout << "\nAttempting connection to non-existent server..." << std::endl;
    config.baseUrl = "http://localhost:9999";
    config.authToken = "mySecretKey";
    
    auto database2 = std::make_shared<APIDatabase>(config);
    
    if (!database2->initialize()) {
        std::cout << "✓ Correctly handled connection failure" << std::endl;
    }
}

void demonstrateDifferentAuthMethods() {
    std::cout << "7. DIFFERENT AUTHENTICATION METHODS" << std::endl;
    std::cout << "------------------------------------" << std::endl;
    
    // Method 1: API Key
    std::cout << "Method 1: API Key in custom header" << std::endl;
    APIDatabase::APIConfig config1;
    config1.baseUrl = "http://localhost:8080";
    config1.authType = APIDatabase::AuthType::API_KEY;
    config1.authToken = "mySecretKey";
    std::cout << "  Header: X-API-Key: mySecretKey" << std::endl;
    
    // Method 2: Bearer Token
    std::cout << "\nMethod 2: Bearer Token" << std::endl;
    APIDatabase::APIConfig config2;
    config2.baseUrl = "http://localhost:8080";
    config2.authType = APIDatabase::AuthType::BEARER_TOKEN;
    config2.authToken = "mySecretKey";
    std::cout << "  Header: Authorization: Bearer mySecretKey" << std::endl;
    
    // Method 3: No Authentication
    std::cout << "\nMethod 3: No Authentication (for local development)" << std::endl;
    APIDatabase::APIConfig config3;
    config3.baseUrl = "http://localhost:8080";
    config3.authType = APIDatabase::AuthType::NONE;
    std::cout << "  Server started with: --no-auth" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  Invelog Database Server Client Demo  " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\nThis demo requires the database server to be running." << std::endl;
    std::cout << "Start the server in another terminal:" << std::endl;
    std::cout << "  invelog_server --local ./data --api-key mySecretKey\n" << std::endl;
    
    std::cout << "Press Enter to continue...";
    std::cin.get();
    
    try {
        printSeparator();
        demonstrateBasicConnection();
        
        printSeparator();
        demonstrateInventoryOperations();
        
        printSeparator();
        demonstrateSearchAndQuery();
        
        printSeparator();
        demonstrateProjectManagement();
        
        printSeparator();
        demonstrateMultipleClients();
        
        printSeparator();
        demonstrateErrorHandling();
        
        printSeparator();
        demonstrateDifferentAuthMethods();
        
        printSeparator();
        std::cout << "Demo completed successfully!" << std::endl;
        std::cout << "\nNext steps:" << std::endl;
        std::cout << "1. Check the server console for API request logs" << std::endl;
        std::cout << "2. View the data directory to see stored items" << std::endl;
        std::cout << "3. Try connecting from multiple clients simultaneously" << std::endl;
        std::cout << "4. Test with different database backends (SQL, etc.)" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
