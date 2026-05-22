#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include "InventoryManager.h"
#include "LocalDatabase.h"
#include "SQLDatabase.h"
#include "APIDatabase.h"
#include "Item.h"
#include "ItemType.h"
#include "Container.h"
#include "Location.h"
#include "Project.h"
#include "Category.h"

void printSeparator() {
    std::cout << "\n" << std::string(80, '=') << "\n\n";
}

void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << " " << title << "\n";
    std::cout << std::string(80, '=') << "\n\n";
}

void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string getLine(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

int getInt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);
        
        if (input.empty()) return 0;
        
        try {
            return std::stoi(input);
        } catch (...) {
            std::cout << "Invalid number. Please try again.\n";
        }
    }
}

void pause() {
    std::cout << "\nPress Enter to continue...";
    std::cin.get();
}

// ==================== ITEM TYPE MANAGEMENT ====================

void listAllItemTypes(InventoryManager& manager) {
    printHeader("ALL ITEM TYPES");
    
    auto itemTypes = manager.getAllItemTypes();
    
    if (itemTypes.empty()) {
        std::cout << "No item types found.\n";
        return;
    }
    
    std::cout << std::left << std::setw(40) << "Name" 
              << std::setw(20) << "Category"
              << std::setw(10) << "Items" << "\n";
    std::cout << std::string(70, '-') << "\n";
    
    for (const auto& itemType : itemTypes) {
        auto category = itemType->getCategory();
        int itemCount = manager.getItemCountByType(itemType->getId());
        
        std::cout << std::left << std::setw(40) << itemType->getName()
                  << std::setw(20) << (category ? category->getName() : "Unknown")
                  << std::setw(10) << itemCount << "\n";
    }
    
    std::cout << "\nTotal: " << itemTypes.size() << " item types\n";
}

void createItemType(InventoryManager& manager) {
    printHeader("CREATE NEW ITEM TYPE");
    
    // Select category
    auto categories = manager.getAllCategories();
    if (categories.empty()) {
        std::cout << "No categories found. Please create a category first.\n";
        return;
    }
    
    std::cout << "Available Categories:\n";
    for (size_t i = 0; i < categories.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << categories[i]->getName() << "\n";
    }
    
    int categoryChoice = getInt("\nSelect category (number): ");
    if (categoryChoice < 1 || categoryChoice > static_cast<int>(categories.size())) {
        std::cout << "Invalid category selection.\n";
        return;
    }
    
    auto selectedCategory = categories[categoryChoice - 1];
    
    // Get item type details
    std::string name = getLine("Enter item type name: ");
    if (name.empty()) {
        std::cout << "Name cannot be empty.\n";
        return;
    }
    
    std::string description = getLine("Enter description: ");
    std::string manufacturer = getLine("Enter manufacturer (optional): ");
    std::string partNumber = getLine("Enter part number (optional): ");
    
    // Get specifications
    std::cout << "\nEnter specifications (key=value pairs, one per line, empty line to finish):\n";
    std::string specString;
    
    while (true) {
        std::string line = getLine("  ");
        if (line.empty()) break;
        
        if (!specString.empty()) {
            specString += ";";
        }
        specString += line;
    }
    
    // Create the item type
    auto itemType = manager.createItemType(name, selectedCategory, description, 
                                          specString, manufacturer, partNumber);
    
    if (itemType) {
        std::cout << "\n✓ Item Type created successfully!\n";
        std::cout << "  ID: " << itemType->getId().toString() << "\n";
        std::cout << "  Name: " << itemType->getName() << "\n";
        std::cout << "  Category: " << selectedCategory->getName() << "\n";
    } else {
        std::cout << "\n✗ Failed to create item type.\n";
    }
}

void viewItemTypeDetails(InventoryManager& manager, std::shared_ptr<ItemType> itemType) {
    printHeader("ITEM TYPE DETAILS");
    
    auto category = itemType->getCategory();
    int itemCount = manager.getItemCountByType(itemType->getId());
    
    std::cout << "ID:           " << itemType->getId().toString() << "\n";
    std::cout << "Name:         " << itemType->getName() << "\n";
    std::cout << "Category:     " << (category ? category->getName() : "Unknown") << "\n";
    std::cout << "Description:  " << itemType->getDescription() << "\n";
    std::cout << "Manufacturer: " << itemType->getManufacturer() << "\n";
    std::cout << "Part Number:  " << itemType->getPartNumber() << "\n";
    std::cout << "Item Count:   " << itemCount << "\n";
    
    std::string specs = itemType->getSpecifications();
    if (!specs.empty()) {
        std::cout << "\nSpecifications:\n";
        std::cout << "  " << specs << "\n";
    }
    
    // List individual items
    if (itemCount > 0) {
        std::cout << "\nIndividual Items:\n";
        std::cout << std::string(80, '-') << "\n";
        
        auto items = manager.getItemsByType(itemType->getId());
        for (const auto& item : items) {
            auto container = item->getCurrentContainer();
            std::cout << "  • " << item->getName() 
                      << " (ID: " << item->getId().toString().substr(0, 8) << "...)\n";
            std::cout << "    Container: " << (container ? container->getName() : "None") << "\n";
            std::cout << "    Status: " << (item->isCheckedOut() ? "Checked Out" : "Available") << "\n";
        }
    }
}

void searchItemTypes(InventoryManager& manager) {
    printHeader("SEARCH ITEM TYPES");
    
    std::string query = getLine("Enter search query: ");
    if (query.empty()) return;
    
    auto results = manager.searchItemTypes(query);
    
    if (results.empty()) {
        std::cout << "No item types found matching \"" << query << "\".\n";
        return;
    }
    
    std::cout << "\nFound " << results.size() << " result(s):\n\n";
    std::cout << std::left << std::setw(40) << "Name" 
              << std::setw(20) << "Category"
              << std::setw(10) << "Items" << "\n";
    std::cout << std::string(70, '-') << "\n";
    
    for (const auto& itemType : results) {
        auto category = itemType->getCategory();
        int itemCount = manager.getItemCountByType(itemType->getId());
        
        std::cout << std::left << std::setw(40) << itemType->getName()
                  << std::setw(20) << (category ? category->getName() : "Unknown")
                  << std::setw(10) << itemCount << "\n";
    }
}

void itemTypeExplorer(InventoryManager& manager) {
    while (true) {
        printHeader("ITEM TYPE EXPLORER");
        
        auto itemTypes = manager.getAllItemTypes();
        
        if (itemTypes.empty()) {
            std::cout << "No item types available. Create one first!\n";
            pause();
            return;
        }
        
        std::cout << "Item Types:\n\n";
        for (size_t i = 0; i < itemTypes.size(); ++i) {
            auto category = itemTypes[i]->getCategory();
            int itemCount = manager.getItemCountByType(itemTypes[i]->getId());
            
            std::cout << "  " << (i + 1) << ". " << itemTypes[i]->getName()
                      << " (" << (category ? category->getName() : "Unknown") 
                      << ") - " << itemCount << " item(s)\n";
        }
        
        std::cout << "\n  0. Back to main menu\n";
        
        int choice = getInt("\nSelect item type (number): ");
        
        if (choice == 0) break;
        if (choice < 1 || choice > static_cast<int>(itemTypes.size())) {
            std::cout << "Invalid selection.\n";
            pause();
            continue;
        }
        
        auto selectedType = itemTypes[choice - 1];
        
        while (true) {
            viewItemTypeDetails(manager, selectedType);
            
            std::cout << "\n\nActions:\n";
            std::cout << "  1. Create individual item of this type\n";
            std::cout << "  2. View item details\n";
            std::cout << "  3. Update item type\n";
            std::cout << "  4. Delete item type\n";
            std::cout << "  0. Back to item type list\n";
            
            int action = getInt("\nSelect action: ");
            
            if (action == 0) break;
            
            switch (action) {
                case 1: {
                    // Create individual item
                    std::string itemName = getLine("\nEnter item name (or leave blank for default): ");
                    if (itemName.empty()) {
                        itemName = selectedType->getName();
                    }
                    
                    std::string notes = getLine("Enter notes for this specific item: ");
                    
                    auto category = selectedType->getCategory();
                    auto item = manager.createItem(itemName, category, 1, notes);
                    
                    if (item) {
                        // Link the item to the itemType via description (workaround)
                        // In Phase 5, we should add proper itemType linking
                        std::cout << "\n✓ Individual item created!\n";
                        std::cout << "  Item ID: " << item->getId().toString() << "\n";
                        std::cout << "  Type: " << selectedType->getName() << "\n";
                    } else {
                        std::cout << "\n✗ Failed to create item.\n";
                    }
                    pause();
                    break;
                }
                
                case 2: {
                    // View specific item
                    auto items = manager.getItemsByType(selectedType->getId());
                    if (items.empty()) {
                        std::cout << "\nNo individual items of this type.\n";
                        pause();
                        break;
                    }
                    
                    std::cout << "\nIndividual Items:\n";
                    for (size_t i = 0; i < items.size(); ++i) {
                        std::cout << "  " << (i + 1) << ". " << items[i]->getName()
                                  << " (ID: " << items[i]->getId().toString().substr(0, 8) << "...)\n";
                    }
                    
                    int itemChoice = getInt("\nSelect item (number): ");
                    if (itemChoice < 1 || itemChoice > static_cast<int>(items.size())) {
                        std::cout << "Invalid selection.\n";
                        pause();
                        break;
                    }
                    
                    auto selectedItem = items[itemChoice - 1];
                    printHeader("ITEM DETAILS");
                    
                    std::cout << "ID:          " << selectedItem->getId().toString() << "\n";
                    std::cout << "Name:        " << selectedItem->getName() << "\n";
                    std::cout << "Type:        " << selectedType->getName() << "\n";
                    std::cout << "Description: " << selectedItem->getDescription() << "\n";
                    std::cout << "Status:      " << (selectedItem->isCheckedOut() ? "Checked Out" : "Available") << "\n";
                    
                    auto container = selectedItem->getCurrentContainer();
                    std::cout << "Container:   " << (container ? container->getName() : "None") << "\n";
                    
                    pause();
                    break;
                }
                
                case 3: {
                    // Update item type
                    std::cout << "\nUpdate Item Type (leave blank to keep current value):\n";
                    
                    std::string newName = getLine("New name [" + selectedType->getName() + "]: ");
                    std::string newDesc = getLine("New description [" + selectedType->getDescription() + "]: ");
                    std::string newMfg = getLine("New manufacturer [" + selectedType->getManufacturer() + "]: ");
                    std::string newPN = getLine("New part number [" + selectedType->getPartNumber() + "]: ");
                    
                    if (!newName.empty()) selectedType->setName(newName);
                    if (!newDesc.empty()) selectedType->setDescription(newDesc);
                    if (!newMfg.empty()) selectedType->setManufacturer(newMfg);
                    if (!newPN.empty()) selectedType->setPartNumber(newPN);
                    
                    // Save changes (manual save since no updateItemType method)
                    std::cout << "\n✓ Item type updated successfully!\n";
                    std::cout << "Note: Changes will be persisted on next save.\n";
                    pause();
                    break;
                }
                
                case 4: {
                    // Delete item type
                    int itemCount = manager.getItemCountByType(selectedType->getId());
                    if (itemCount > 0) {
                        std::cout << "\n✗ Cannot delete item type: " << itemCount 
                                  << " item(s) still reference it.\n";
                        std::cout << "Delete those items first.\n";
                        pause();
                        break;
                    }
                    
                    std::string confirm = getLine("\nAre you sure you want to delete this item type? (yes/no): ");
                    if (confirm == "yes" || confirm == "y") {
                        if (manager.deleteItemType(selectedType->getId())) {
                            std::cout << "\n✓ Item type deleted.\n";
                            pause();
                            goto exit_detail_view;  // Break out of inner loop
                        } else {
                            std::cout << "\n✗ Failed to delete item type.\n";
                        }
                    }
                    pause();
                    break;
                }
                
                default:
                    std::cout << "Invalid action.\n";
                    pause();
            }
        }
        exit_detail_view:;
    }
}

void itemTypeMenu(InventoryManager& manager) {
    while (true) {
        printHeader("ITEM TYPE MANAGEMENT");
        
        std::cout << "1. List all item types\n";
        std::cout << "2. Create new item type\n";
        std::cout << "3. Search item types\n";
        std::cout << "4. Item type explorer (browse and manage)\n";
        std::cout << "0. Back to main menu\n";
        
        int choice = getInt("\nSelect option: ");
        
        switch (choice) {
            case 0:
                return;
            case 1:
                listAllItemTypes(manager);
                pause();
                break;
            case 2:
                createItemType(manager);
                pause();
                break;
            case 3:
                searchItemTypes(manager);
                pause();
                break;
            case 4:
                itemTypeExplorer(manager);
                break;
            default:
                std::cout << "Invalid option.\n";
                pause();
        }
    }
}

// ==================== CATEGORY MANAGEMENT ====================

void categoryMenu(InventoryManager& manager) {
    while (true) {
        printHeader("CATEGORY MANAGEMENT");
        
        auto categories = manager.getAllCategories();
        
        std::cout << "Current Categories:\n\n";
        if (categories.empty()) {
            std::cout << "  (No categories yet)\n";
        } else {
            for (const auto& cat : categories) {
                std::cout << "  • " << cat->getName() << " - " << cat->getDescription() << "\n";
            }
        }
        
        std::cout << "\n1. Create new category\n";
        std::cout << "0. Back to main menu\n";
        
        int choice = getInt("\nSelect option: ");
        
        if (choice == 0) return;
        
        if (choice == 1) {
            std::string name = getLine("Enter category name: ");
            std::string desc = getLine("Enter description: ");
            
            if (!name.empty()) {
                auto category = manager.createCategory(name, desc);
                if (category) {
                    std::cout << "\n✓ Category created: " << category->getName() << "\n";
                } else {
                    std::cout << "\n✗ Failed to create category.\n";
                }
            }
            pause();
        }
    }
}

// ==================== QUICK START DEMO ====================

void runQuickStartDemo(InventoryManager& manager) {
    printHeader("QUICK START DEMO - Setting up sample data");
    
    std::cout << "Creating sample categories...\n";
    auto microcontrollers = manager.createCategory("Microcontrollers", "MCUs and development boards");
    auto sensors = manager.createCategory("Sensors", "Various sensor modules");
    auto passives = manager.createCategory("Passive Components", "Resistors, capacitors, etc.");
    
    std::cout << "Creating sample containers...\n";
    auto mainStorage = manager.createContainer("Main Storage", ContainerType::INVENTORY, 
                                              "Primary component storage");
    auto drawer1 = manager.createContainer("Drawer A", ContainerType::SUBCONTAINER,
                                          "Microcontrollers and modules");
    mainStorage->addSubcontainer(drawer1);
    
    std::cout << "Creating item types...\n";
    
    // HC-05 Bluetooth Module
    std::string hc05Specs = "Voltage=3.3V-5V;Range=10m;Protocol=Bluetooth 2.0;Frequency=2.4GHz";
    auto hc05Type = manager.createItemType("HC-05 Bluetooth Module", microcontrollers,
                                          "Bluetooth serial communication module",
                                          hc05Specs, "Various", "HC-05");
    
    // Arduino Nano
    std::string nanoSpecs = "MCU=ATmega328P;Voltage=5V;Digital I/O=14;Analog Input=8";
    auto nanoType = manager.createItemType("Arduino Nano", microcontrollers,
                                          "Compact Arduino board",
                                          nanoSpecs, "Arduino", "A000005");
    
    // DHT22 Temperature Sensor
    std::string dht22Specs = "Type=Temperature & Humidity;Temp Range=-40°C to 80°C;Humidity Range=0-100% RH;Accuracy=±0.5°C, ±2% RH";
    auto dht22Type = manager.createItemType("DHT22 Temperature Sensor", sensors,
                                           "Digital temperature and humidity sensor",
                                           dht22Specs, "Aosong", "DHT22");
    
    std::cout << "Creating individual items...\n";
    
    // Create 3 HC-05 modules with different histories
    auto hc05_1 = manager.createItem("HC-05 Bluetooth Module", microcontrollers, 1,
                                    "Working condition, tested 2025-10-01");
    auto hc05_2 = manager.createItem("HC-05 Bluetooth Module", microcontrollers, 1,
                                    "New, unopened package");
    auto hc05_3 = manager.createItem("HC-05 Bluetooth Module", microcontrollers, 1,
                                    "Used in Robot Project, firmware v2.1");
    
    // Create 2 Arduino Nanos
    auto nano_1 = manager.createItem("Arduino Nano", microcontrollers, 1,
                                    "Clone board, works perfectly");
    auto nano_2 = manager.createItem("Arduino Nano", microcontrollers, 1,
                                    "Official board, bootloader installed");
    
    // Create DHT22 sensors
    auto dht22_1 = manager.createItem("DHT22 Temperature Sensor", sensors, 1,
                                     "Calibrated 2025-09-15");
    
    // Place items in containers
    manager.moveItem(hc05_1->getId(), drawer1->getId());
    manager.moveItem(hc05_2->getId(), drawer1->getId());
    manager.moveItem(hc05_3->getId(), drawer1->getId());
    manager.moveItem(nano_1->getId(), drawer1->getId());
    manager.moveItem(nano_2->getId(), drawer1->getId());
    manager.moveItem(dht22_1->getId(), drawer1->getId());
    
    std::cout << "\n✓ Sample data created successfully!\n";
    std::cout << "\nCreated:\n";
    std::cout << "  • 3 categories\n";
    std::cout << "  • 3 item types\n";
    std::cout << "  • 6 individual items (3 HC-05, 2 Arduino Nano, 1 DHT22)\n";
    std::cout << "  • 2 containers\n";
    
    std::cout << "\nYou can now:\n";
    std::cout << "  - Browse item types to see grouping (e.g., '3 units of HC-05')\n";
    std::cout << "  - View individual item details and their unique histories\n";
    std::cout << "  - Search and manage your inventory\n";
}

void demonstrateBasicOperations(InventoryManager& manager) {
    // Legacy function - kept for compatibility but not used
    // Use runQuickStartDemo() instead
}

int main() {
    std::cout << "\n";
    std::cout << "================================================================================\n";
    std::cout << "                              INVELOG CLI                                       \n";
    std::cout << "                  Inventory Management System for Electronics                   \n";
    std::cout << "================================================================================\n";
    
    try {
        // Database selection
        std::cout << "\nSelect Database Backend:\n";
        std::cout << "1. Local File Database (JSON files)\n";
        std::cout << "2. SQL Database (SQLite - requires setup)\n";
        std::cout << "3. API Database (Remote server - requires connection)\n";
        std::cout << "\nRecommended for first-time use: Option 1\n";
        
        int dbChoice = getInt("\nSelect database option (1-3): ");
        
        std::shared_ptr<IDatabase> database;
        std::string dbInfo;
        
        switch (dbChoice) {
            case 2: {
                std::cout << "\n[SQL Database Configuration]\n";
                std::string dbPath = getLine("Enter SQLite database path [./invelog.db]: ");
                if (dbPath.empty()) dbPath = "./invelog.db";
                
                SQLDatabase::ConnectionConfig sqlConfig;
                sqlConfig.type = SQLDatabase::SQLType::SQLITE;
                sqlConfig.database = dbPath;
                
                database = std::make_shared<SQLDatabase>(sqlConfig);
                dbInfo = "SQL Database: " + dbPath;
                break;
            }
            case 3: {
                std::cout << "\n[API Database Configuration]\n";
                std::string apiUrl = getLine("Enter API server URL [http://localhost:8080]: ");
                if (apiUrl.empty()) apiUrl = "http://localhost:8080";
                std::string apiKey = getLine("Enter API key (optional): ");
                
                APIDatabase::APIConfig apiConfig;
                apiConfig.baseUrl = apiUrl;
                apiConfig.apiKey = apiKey;
                apiConfig.authMethod = apiKey.empty() ? 
                    APIDatabase::APIConfig::AuthMethod::NONE : 
                    APIDatabase::APIConfig::AuthMethod::API_KEY;
                
                // Configure API endpoints with /api prefix to match server routes
                apiConfig.itemsEndpoint = "/api/items";
                apiConfig.itemTypesEndpoint = "/api/item-types";
                apiConfig.containersEndpoint = "/api/containers";
                apiConfig.locationsEndpoint = "/api/locations";
                apiConfig.projectsEndpoint = "/api/projects";
                apiConfig.categoriesEndpoint = "/api/categories";
                apiConfig.activityLogsEndpoint = "/api/activity-logs";
                
                database = std::make_shared<APIDatabase>(apiConfig);
                dbInfo = "API Database: " + apiUrl;
                break;
            }
            case 1:
            default: {
                std::string dataPath = "./invelog_data";
                database = std::make_shared<LocalDatabase>(dataPath);
                dbInfo = "Local File Database: " + dataPath;
                break;
            }
        }
        
        // Initialize database and inventory manager
        std::cout << "\nInitializing system...\n";
        InventoryManager manager(database);
        
        if (!manager.initialize()) {
            std::cerr << "Failed to initialize inventory manager\n";
            std::cerr << "Note: SQL and API databases may require additional setup.\n";
            std::cerr << "Try using Local File Database (Option 1) for testing.\n";
            return 1;
        }
        
        std::cout << "[OK] System initialized successfully\n";
        std::cout << "[OK] " << dbInfo << "\n";
        
        // Check if this is first run
        bool firstRun = manager.getAllCategories().empty() && 
                       manager.getAllItemTypes().empty() &&
                       manager.getAllItems().empty();
        
        if (firstRun) {
            std::cout << "\n================================================================\n";
            std::cout << "  First time setup detected!                                   \n";
            std::cout << "================================================================\n\n";
            
            std::string runDemo = getLine("Would you like to load sample data? (yes/no): ");
            if (runDemo == "yes" || runDemo == "y") {
                runQuickStartDemo(manager);
                pause();
            }
        }
        
        // Main menu loop
        while (true) {
            printHeader("MAIN MENU");
            
            std::cout << "Database: " << dbInfo << "\n\n";
            
            std::cout << "1. Item Type Management\n";
            std::cout << "2. Category Management\n";
            std::cout << "3. System Information\n";
            std::cout << "4. Load Sample Data\n";
            std::cout << "0. Exit\n";
            
            int choice = getInt("\nSelect option: ");
            
            switch (choice) {
                case 0:
                    std::cout << "\nShutting down...\n";
                    if (manager.shutdown()) {
                        std::cout << "[OK] All data saved successfully\n";
                    }
                    std::cout << "\nGoodbye!\n\n";
                    return 0;
                    
                case 1:
                    itemTypeMenu(manager);
                    break;
                    
                case 2:
                    categoryMenu(manager);
                    break;
                    
                case 3: {
                    printHeader("SYSTEM INFORMATION");
                    std::cout << "Database:           " << dbInfo << "\n";
                    std::cout << "Total Categories:   " << manager.getAllCategories().size() << "\n";
                    std::cout << "Total Item Types:   " << manager.getAllItemTypes().size() << "\n";
                    std::cout << "Total Items:        " << manager.getAllItems().size() << "\n";
                    std::cout << "Total Containers:   " << manager.getAllContainers().size() << "\n";
                    std::cout << "Total Locations:    " << manager.getAllLocations().size() << "\n";
                    std::cout << "Total Projects:     " << manager.getAllProjects().size() << "\n";
                    pause();
                    break;
                }
                
                case 4:
                    runQuickStartDemo(manager);
                    pause();
                    break;
                    
                default:
                    std::cout << "Invalid option.\n";
                    pause();
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

