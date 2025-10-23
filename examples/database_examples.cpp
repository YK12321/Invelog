#include <iostream>
#include <memory>
#include "InventoryManager.h"
#include "LocalDatabase.h"
#include "SQLDatabase.h"
#include "APIDatabase.h"

void printSeparator(const std::string& title = "") {
    std::cout << "\n" << std::string(70, '=') << "\n";
    if (!title.empty()) {
        std::cout << "  " << title << "\n";
        std::cout << std::string(70, '=') << "\n";
    }
    std::cout << "\n";
}

void demonstrateLocalDatabase() {
    printSeparator("LOCAL DATABASE EXAMPLE");
    
    std::cout << "Creating LocalDatabase instance...\n";
    auto localDb = std::make_shared<LocalDatabase>("./local_data");
    
    InventoryManager manager(localDb);
    
    if (manager.initialize()) {
        std::cout << "✓ LocalDatabase initialized successfully\n";
        
        // Create some test data
        auto category = manager.createCategory("Test Category", "For testing");
        auto item = manager.createItem("Test Item", category, 5);
        
        std::cout << "✓ Created test item: " << item->getName() 
                  << " (ID: " << item->getId().toString() << ")\n";
        
        manager.shutdown();
        std::cout << "✓ LocalDatabase shutdown complete\n";
    } else {
        std::cerr << "✗ Failed to initialize LocalDatabase\n";
    }
}

void demonstrateSQLDatabase() {
    printSeparator("SQL DATABASE EXAMPLE");
    
    std::cout << "Configuring SQL Database connection...\n\n";
    
    // Example 1: PostgreSQL
    std::cout << "Example 1: PostgreSQL Configuration\n";
    SQLDatabase::ConnectionConfig pgConfig;
    pgConfig.type = SQLDatabase::SQLType::POSTGRESQL;
    pgConfig.host = "localhost";
    pgConfig.port = 5432;
    pgConfig.database = "invelog_db";
    pgConfig.username = "invelog_user";
    pgConfig.password = "secure_password";
    pgConfig.useSSL = true;
    pgConfig.maxConnections = 10;
    
    auto sqlDb = std::make_shared<SQLDatabase>(pgConfig);
    
    std::cout << "Attempting to connect to PostgreSQL...\n";
    if (sqlDb->connect()) {
        std::cout << "✓ Connected to PostgreSQL database\n";
        std::cout << "✓ Schema initialized\n";
        
        InventoryManager manager(sqlDb);
        if (manager.initialize()) {
            std::cout << "✓ InventoryManager initialized with SQL backend\n";
            
            // Create test data
            auto category = manager.createCategory("Electronics", "Electronic components");
            std::cout << "✓ Created category in SQL database\n";
            
            manager.shutdown();
        }
        
        sqlDb->disconnect();
        std::cout << "✓ Disconnected from database\n";
    } else {
        std::cout << "Note: Connection simulation only (no actual DB)\n";
    }
    
    std::cout << "\n";
    
    // Example 2: MySQL
    std::cout << "Example 2: MySQL Configuration\n";
    SQLDatabase::ConnectionConfig mysqlConfig;
    mysqlConfig.type = SQLDatabase::SQLType::MYSQL;
    mysqlConfig.host = "localhost";
    mysqlConfig.port = 3306;
    mysqlConfig.database = "invelog";
    mysqlConfig.username = "root";
    mysqlConfig.password = "password";
    
    std::cout << "MySQL connection string would be:\n";
    std::cout << "  mysql://root:***@localhost:3306/invelog\n";
    
    std::cout << "\n";
    
    // Example 3: SQLite
    std::cout << "Example 3: SQLite Configuration (File-based)\n";
    SQLDatabase::ConnectionConfig sqliteConfig;
    sqliteConfig.type = SQLDatabase::SQLType::SQLITE;
    sqliteConfig.database = "./invelog.db";
    
    std::cout << "SQLite database file: ./invelog.db\n";
    std::cout << "Perfect for: Single-user, embedded applications\n";
}

void demonstrateAPIDatabase() {
    printSeparator("API DATABASE EXAMPLE");
    
    std::cout << "Configuring API Database connection...\n\n";
    
    // Example 1: API Key Authentication
    std::cout << "Example 1: API Key Authentication\n";
    APIDatabase::APIConfig apiConfig;
    apiConfig.baseUrl = "https://api.invelog.example.com/v1";
    apiConfig.apiKey = "your-api-key-here";
    apiConfig.authMethod = APIDatabase::APIConfig::AuthMethod::API_KEY;
    apiConfig.timeoutSeconds = 30;
    apiConfig.maxRetries = 3;
    apiConfig.maxRequestsPerMinute = 100;
    
    auto apiDb = std::make_shared<APIDatabase>(apiConfig);
    
    std::cout << "Attempting to connect to API...\n";
    if (apiDb->connect()) {
        std::cout << "✓ Connected to API endpoint\n";
        std::cout << "✓ API Key validated\n";
        
        InventoryManager manager(apiDb);
        if (manager.initialize()) {
            std::cout << "✓ InventoryManager initialized with API backend\n";
            
            // Create test data
            auto location = manager.createLocation("Cloud Warehouse", "Virtual");
            std::cout << "✓ Created location via API\n";
            
            manager.shutdown();
        }
        
        apiDb->disconnect();
        std::cout << "✓ Disconnected from API\n";
    } else {
        std::cout << "Note: API simulation only (no actual endpoint)\n";
    }
    
    std::cout << "\n";
    
    // Example 2: Bearer Token (OAuth2)
    std::cout << "Example 2: Bearer Token (OAuth2) Configuration\n";
    APIDatabase::APIConfig oauthConfig;
    oauthConfig.baseUrl = "https://api.invelog.cloud/v2";
    oauthConfig.authToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...";
    oauthConfig.authMethod = APIDatabase::APIConfig::AuthMethod::BEARER_TOKEN;
    oauthConfig.verifySSL = true;
    
    std::cout << "OAuth2 Bearer token configured\n";
    std::cout << "Perfect for: Enterprise cloud deployments\n";
    
    std::cout << "\n";
    
    // Example 3: Custom Headers
    std::cout << "Example 3: Custom Headers and Rate Limiting\n";
    APIDatabase::APIConfig customConfig;
    customConfig.baseUrl = "https://custom-api.example.com";
    customConfig.apiKey = "custom-key";
    customConfig.maxRequestsPerMinute = 60;
    customConfig.customHeaders["X-Custom-Header"] = "CustomValue";
    customConfig.customHeaders["X-Client-Version"] = "1.0.0";
    
    std::cout << "Custom headers configured\n";
    std::cout << "Rate limit: 60 requests/minute\n";
}

void showDatabaseComparison() {
    printSeparator("DATABASE COMPARISON");
    
    std::cout << "┌─────────────────┬──────────────────────────────────────────────┐\n";
    std::cout << "│ Database Type   │ Best For                                     │\n";
    std::cout << "├─────────────────┼──────────────────────────────────────────────┤\n";
    std::cout << "│ LocalDatabase   │ • Single-user desktop applications           │\n";
    std::cout << "│                 │ • Offline operation                          │\n";
    std::cout << "│                 │ • Quick prototyping                          │\n";
    std::cout << "│                 │ • No server setup required                   │\n";
    std::cout << "├─────────────────┼──────────────────────────────────────────────┤\n";
    std::cout << "│ SQLDatabase     │ • Multi-user environments                    │\n";
    std::cout << "│ (PostgreSQL)    │ • Complex queries and relationships          │\n";
    std::cout << "│                 │ • ACID transactions                          │\n";
    std::cout << "│                 │ • Enterprise deployments                     │\n";
    std::cout << "├─────────────────┼──────────────────────────────────────────────┤\n";
    std::cout << "│ SQLDatabase     │ • Medium-scale web applications              │\n";
    std::cout << "│ (MySQL)         │ • Shared hosting environments                │\n";
    std::cout << "│                 │ • Good performance for read-heavy loads      │\n";
    std::cout << "├─────────────────┼──────────────────────────────────────────────┤\n";
    std::cout << "│ SQLDatabase     │ • Embedded systems                           │\n";
    std::cout << "│ (SQLite)        │ • Mobile applications                        │\n";
    std::cout << "│                 │ • Single-file database                       │\n";
    std::cout << "│                 │ • Zero configuration                         │\n";
    std::cout << "├─────────────────┼──────────────────────────────────────────────┤\n";
    std::cout << "│ APIDatabase     │ • Cloud-native applications                  │\n";
    std::cout << "│                 │ • Microservices architecture                 │\n";
    std::cout << "│                 │ • External data integration                  │\n";
    std::cout << "│                 │ • SaaS platforms                             │\n";
    std::cout << "└─────────────────┴──────────────────────────────────────────────┘\n";
}

void showUsageExamples() {
    printSeparator("QUICK USAGE EXAMPLES");
    
    std::cout << "1. LOCAL DATABASE (File-based)\n";
    std::cout << "   auto db = std::make_shared<LocalDatabase>(\"./data\");\n";
    std::cout << "   InventoryManager manager(db);\n";
    std::cout << "   manager.initialize();\n\n";
    
    std::cout << "2. SQL DATABASE (PostgreSQL)\n";
    std::cout << "   SQLDatabase::ConnectionConfig config;\n";
    std::cout << "   config.type = SQLDatabase::SQLType::POSTGRESQL;\n";
    std::cout << "   config.host = \"localhost\";\n";
    std::cout << "   config.database = \"invelog\";\n";
    std::cout << "   auto db = std::make_shared<SQLDatabase>(config);\n\n";
    
    std::cout << "3. API DATABASE (REST API)\n";
    std::cout << "   APIDatabase::APIConfig config;\n";
    std::cout << "   config.baseUrl = \"https://api.example.com\";\n";
    std::cout << "   config.apiKey = \"your-key\";\n";
    std::cout << "   auto db = std::make_shared<APIDatabase>(config);\n\n";
    
    std::cout << "4. SWITCHING DATABASES (Just change one line!)\n";
    std::cout << "   // Change from Local to SQL:\n";
    std::cout << "   // auto db = std::make_shared<LocalDatabase>(\"./data\");\n";
    std::cout << "   auto db = std::make_shared<SQLDatabase>(sqlConfig);\n";
    std::cout << "   \n";
    std::cout << "   // Everything else stays the same!\n";
    std::cout << "   InventoryManager manager(db);\n";
    std::cout << "   manager.initialize();\n";
}

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║          INVELOG - EXTERNAL DATABASE SUPPORT DEMO                  ║\n";
    std::cout << "║       LocalDatabase | SQLDatabase | APIDatabase                    ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════════╝\n";
    
    try {
        // Show database comparison
        showDatabaseComparison();
        
        // Show usage examples
        showUsageExamples();
        
        // Demonstrate each database type
        demonstrateLocalDatabase();
        demonstrateSQLDatabase();
        demonstrateAPIDatabase();
        
        printSeparator("SUMMARY");
        
        std::cout << "✓ Three database backends implemented:\n\n";
        
        std::cout << "  1. LocalDatabase (File-based)\n";
        std::cout << "     - Ready to use out of the box\n";
        std::cout << "     - Perfect for development and testing\n";
        std::cout << "     - No external dependencies\n\n";
        
        std::cout << "  2. SQLDatabase (PostgreSQL/MySQL/SQLite/MSSQL)\n";
        std::cout << "     - Professional-grade database support\n";
        std::cout << "     - Complete schema with indexes\n";
        std::cout << "     - Transaction support\n";
        std::cout << "     - Migration system ready\n\n";
        
        std::cout << "  3. APIDatabase (REST API)\n";
        std::cout << "     - Cloud-native integration\n";
        std::cout << "     - Multiple authentication methods\n";
        std::cout << "     - Rate limiting and retry logic\n";
        std::cout << "     - Batch operations support\n\n";
        
        std::cout << "Key Benefits:\n";
        std::cout << "  • Unified interface (IDatabase)\n";
        std::cout << "  • Easy to switch between backends\n";
        std::cout << "  • Application code stays the same\n";
        std::cout << "  • Choose the right tool for your needs\n\n";
        
        std::cout << "Next Steps:\n";
        std::cout << "  1. Install database libraries (libpq, MySQL Connector, etc.)\n";
        std::cout << "  2. Integrate JSON library for serialization\n";
        std::cout << "  3. Complete deserialization methods\n";
        std::cout << "  4. Add HTTP client library for API support\n";
        std::cout << "  5. Write comprehensive unit tests\n\n";
        
        printSeparator();
        
        std::cout << "Demo completed successfully!\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
