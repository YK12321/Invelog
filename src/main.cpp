#include <iostream>
#include <memory>
#include "InventoryManager.h"
#include "LocalDatabase.h"
#include "Item.h"
#include "Container.h"
#include "Location.h"
#include "Project.h"
#include "Category.h"

void printSeparator() {
    std::cout << "\n" << std::string(60, '=') << "\n\n";
}

void demonstrateBasicOperations(InventoryManager& manager) {
    std::cout << "=== DEMONSTRATION: Basic Inventory Operations ===\n\n";
    
    // 1. Create Categories
    std::cout << "1. Creating categories...\n";
    auto resistorCategory = manager.createCategory("Resistors", "Passive electronic components");
    auto capacitorCategory = manager.createCategory("Capacitors", "Energy storage components");
    auto icCategory = manager.createCategory("Integrated Circuits", "Active components");
    std::cout << "   ✓ Created 3 categories\n";
    
    printSeparator();
    
    // 2. Create Locations
    std::cout << "2. Creating locations...\n";
    auto mainWarehouse = manager.createLocation("Main Warehouse", "123 Tech Street");
    auto labLocation = manager.createLocation("Electronics Lab", "Building B, Room 101");
    std::cout << "   ✓ Created 2 locations\n";
    
    printSeparator();
    
    // 3. Create Containers
    std::cout << "3. Creating containers...\n";
    auto mainStorage = manager.createContainer("Main Storage", ContainerType::INVENTORY, 
                                              "Primary inventory storage");
    auto drawer1 = manager.createContainer("Drawer 1", ContainerType::SUBCONTAINER, 
                                          "Small components drawer");
    auto drawer2 = manager.createContainer("Drawer 2", ContainerType::SUBCONTAINER, 
                                          "IC storage drawer");
    
    // Set up container hierarchy
    mainWarehouse->addContainer(mainStorage);
    mainStorage->addSubcontainer(drawer1);
    mainStorage->addSubcontainer(drawer2);
    
    std::cout << "   ✓ Created 3 containers with hierarchy\n";
    std::cout << "   ✓ Main Storage -> Drawer 1, Drawer 2\n";
    
    printSeparator();
    
    // 4. Create Items
    std::cout << "4. Creating inventory items...\n";
    auto resistor1k = manager.createItem("Resistor 1kΩ", resistorCategory, 100, 
                                        "1/4W carbon film resistor");
    auto resistor10k = manager.createItem("Resistor 10kΩ", resistorCategory, 50, 
                                         "1/4W carbon film resistor");
    auto cap100nf = manager.createItem("Capacitor 100nF", capacitorCategory, 75, 
                                      "Ceramic capacitor 50V");
    auto atmega328 = manager.createItem("ATmega328P", icCategory, 10, 
                                       "8-bit AVR microcontroller");
    
    std::cout << "   ✓ Created 4 items:\n";
    std::cout << "     - " << resistor1k->getName() << " (Qty: " << resistor1k->getQuantity() << ")\n";
    std::cout << "     - " << resistor10k->getName() << " (Qty: " << resistor10k->getQuantity() << ")\n";
    std::cout << "     - " << cap100nf->getName() << " (Qty: " << cap100nf->getQuantity() << ")\n";
    std::cout << "     - " << atmega328->getName() << " (Qty: " << atmega328->getQuantity() << ")\n";
    
    printSeparator();
    
    // 5. Place items in containers
    std::cout << "5. Organizing items in containers...\n";
    manager.moveItem(resistor1k->getId(), drawer1->getId());
    manager.moveItem(resistor10k->getId(), drawer1->getId());
    manager.moveItem(cap100nf->getId(), drawer1->getId());
    manager.moveItem(atmega328->getId(), drawer2->getId());
    
    std::cout << "   ✓ Moved resistors and capacitors to Drawer 1\n";
    std::cout << "   ✓ Moved ATmega328P to Drawer 2\n";
    
    printSeparator();
    
    // 6. Display container contents
    std::cout << "6. Container Inventory:\n\n";
    std::cout << "   Drawer 1 contents:\n";
    for (const auto& item : drawer1->getAllItems()) {
        std::cout << "     - " << item->getName() << " (Qty: " << item->getQuantity() << ")\n";
    }
    
    std::cout << "\n   Drawer 2 contents:\n";
    for (const auto& item : drawer2->getAllItems()) {
        std::cout << "     - " << item->getName() << " (Qty: " << item->getQuantity() << ")\n";
    }
    
    printSeparator();
    
    // 7. Create a Project
    std::cout << "7. Creating a project...\n";
    auto arduinoProject = manager.createProject("Arduino LED Controller", 
                                               "Build an LED controller using Arduino");
    arduinoProject->setStatus(ProjectStatus::IN_PROGRESS);
    
    auto projectContainer = manager.createContainer("Arduino Project Box", 
                                                   ContainerType::PROJECT,
                                                   "Components for Arduino LED project");
    arduinoProject->addContainer(projectContainer);
    labLocation->addContainer(projectContainer);
    
    std::cout << "   ✓ Created project: " << arduinoProject->getName() << "\n";
    std::cout << "   ✓ Status: " << arduinoProject->getStatusString() << "\n";
    std::cout << "   ✓ Created project container\n";
    
    printSeparator();
    
    // 8. Allocate items to project
    std::cout << "8. Allocating components to project...\n";
    
    // Move some resistors to the project
    resistor1k->adjustQuantity(-10);  // Take 10 from inventory
    auto projectResistor = manager.createItem("Resistor 1kΩ", resistorCategory, 10, 
                                             "For Arduino project");
    manager.moveItem(projectResistor->getId(), projectContainer->getId());
    manager.assignItemToProject(projectResistor->getId(), arduinoProject->getId());
    
    // Move an ATmega to the project
    atmega328->adjustQuantity(-1);  // Take 1 from inventory
    auto projectMCU = manager.createItem("ATmega328P", icCategory, 1, 
                                        "MCU for Arduino project");
    manager.moveItem(projectMCU->getId(), projectContainer->getId());
    manager.assignItemToProject(projectMCU->getId(), arduinoProject->getId());
    
    std::cout << "   ✓ Allocated 10x Resistor 1kΩ to project\n";
    std::cout << "   ✓ Allocated 1x ATmega328P to project\n";
    std::cout << "   ✓ Updated main inventory quantities\n";
    
    printSeparator();
    
    // 9. Check-out/Check-in operations
    std::cout << "9. Simulating check-out/check-in...\n";
    manager.checkOutItem(projectResistor->getId(), "john_doe");
    std::cout << "   ✓ John Doe checked out resistors\n";
    
    manager.checkInItem(projectResistor->getId(), "john_doe");
    std::cout << "   ✓ John Doe checked in resistors\n";
    
    printSeparator();
    
    // 10. Search operations
    std::cout << "10. Search demonstrations:\n\n";
    
    std::cout << "   Searching for 'Resistor':\n";
    auto searchResults = manager.searchItems("Resistor");
    for (const auto& item : searchResults) {
        std::cout << "     - " << item->getName() << " (Qty: " << item->getQuantity() << ")\n";
    }
    
    std::cout << "\n   Items in Resistor category:\n";
    auto categoryItems = manager.findItemsByCategory(resistorCategory->getId());
    for (const auto& item : categoryItems) {
        std::cout << "     - " << item->getName() << " (Qty: " << item->getQuantity() << ")\n";
    }
    
    printSeparator();
    
    // 11. Activity History
    std::cout << "11. Activity history for " << projectResistor->getName() << ":\n\n";
    auto history = manager.getItemHistory(projectResistor->getId());
    for (const auto& activity : history) {
        std::cout << "   - " << activity->getTypeString() << ": " 
                  << activity->getDescription() << "\n";
        std::cout << "     User: " << activity->getUserId() << "\n\n";
    }
    
    printSeparator();
    
    // 12. Summary
    std::cout << "12. System Summary:\n\n";
    std::cout << "   Total Categories: " << manager.getAllCategories().size() << "\n";
    std::cout << "   Total Locations: " << manager.getAllLocations().size() << "\n";
    std::cout << "   Total Containers: " << manager.getAllContainers().size() << "\n";
    std::cout << "   Total Items: " << manager.getAllItems().size() << "\n";
    std::cout << "   Total Projects: " << manager.getAllProjects().size() << "\n";
    std::cout << "   Items in project: " << arduinoProject->getTotalItemCount() << "\n";
    
    printSeparator();
}

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    INVELOG DEMO                          ║\n";
    std::cout << "║         Inventory Management System for Electronics      ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════╝\n";
    
    printSeparator();
    
    try {
        // Initialize database and inventory manager
        std::cout << "Initializing system...\n";
        auto database = std::make_shared<LocalDatabase>("./invelog_data");
        InventoryManager manager(database);
        
        if (!manager.initialize()) {
            std::cerr << "Failed to initialize inventory manager\n";
            return 1;
        }
        
        std::cout << "✓ System initialized successfully\n";
        std::cout << "✓ Database connected: ./invelog_data\n";
        
        printSeparator();
        
        // Run demonstrations
        demonstrateBasicOperations(manager);
        
        // Shutdown
        std::cout << "Shutting down system...\n";
        if (manager.shutdown()) {
            std::cout << "✓ All data saved successfully\n";
            std::cout << "✓ Database disconnected\n";
        }
        
        printSeparator();
        
        std::cout << "Demo completed successfully!\n\n";
        
        std::cout << "Data has been saved to: ./invelog_data/\n";
        std::cout << "You can explore the saved files to see the persisted data.\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
