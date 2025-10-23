#include "DatabaseAPIServer.h"
#include "ServerConfig.h"
#include "LocalDatabase.h"
#include "SQLDatabase.h"
#include <iostream>
#include <string>
#include <csignal>
#include <memory>

// Global server instance for signal handling
std::shared_ptr<DatabaseAPIServer> g_server;

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "\nShutting down server gracefully..." << std::endl;
        if (g_server) {
            g_server->stop();
        }
        exit(0);
    }
}

void printUsage(const char* programName) {
    std::cout << "Invelog Database Server v0.3.0" << std::endl;
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  --port <port>           Set server port (default: 8080)" << std::endl;
    std::cout << "  --api-key <key>         Set API key for authentication" << std::endl;
    std::cout << "  --no-auth               Disable authentication" << std::endl;
    std::cout << "  --cors                  Enable CORS support" << std::endl;
    std::cout << "  --max-request <size>    Set max request size in bytes (default: 10485760)" << std::endl;
    std::cout << "  --timeout <seconds>     Set request timeout in seconds (default: 300)" << std::endl;
    std::cout << "  --local <path>          Use local file-based database" << std::endl;
    std::cout << "  --postgres <conn>       Use PostgreSQL database (connection string)" << std::endl;
    std::cout << "  --mysql <conn>          Use MySQL database (connection string)" << std::endl;
    std::cout << "  --sqlite <path>         Use SQLite database" << std::endl;
    std::cout << "  --help                  Show this help message" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  # Local database with authentication and CORS" << std::endl;
    std::cout << "  " << programName << " --local ./data --port 8080 --api-key mySecretKey --cors" << std::endl;
    std::cout << "\n  # PostgreSQL database without authentication" << std::endl;
    std::cout << "  " << programName << " --postgres \"host=localhost dbname=invelog\" --no-auth" << std::endl;
    std::cout << "\n  # SQLite database on custom port" << std::endl;
    std::cout << "  " << programName << " --sqlite ./invelog.db --port 9000" << std::endl;
}

int main(int argc, char* argv[]) {
    // Default configuration
    ServerConfig config;
    config.port = 8080;
    config.apiKey = "";
    config.authRequired = true;
    config.enableCORS = false;
    config.maxRequestSize = 10485760; // 10 MB
    config.timeoutSeconds = 300; // 5 minutes
    
    std::string dbType = "local";
    std::string dbPath = "./data";
    std::string dbConnectionString;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        }
        else if (arg == "--port" && i + 1 < argc) {
            config.port = std::stoi(argv[++i]);
        }
        else if (arg == "--api-key" && i + 1 < argc) {
            config.apiKey = argv[++i];
            config.authRequired = true;
        }
        else if (arg == "--no-auth") {
            config.authRequired = false;
        }
        else if (arg == "--cors") {
            config.enableCORS = true;
        }
        else if (arg == "--max-request" && i + 1 < argc) {
            config.maxRequestSize = std::stoull(argv[++i]);
        }
        else if (arg == "--timeout" && i + 1 < argc) {
            config.timeoutSeconds = std::stoi(argv[++i]);
        }
        else if (arg == "--local" && i + 1 < argc) {
            dbType = "local";
            dbPath = argv[++i];
        }
        else if (arg == "--postgres" && i + 1 < argc) {
            dbType = "postgres";
            dbConnectionString = argv[++i];
        }
        else if (arg == "--mysql" && i + 1 < argc) {
            dbType = "mysql";
            dbConnectionString = argv[++i];
        }
        else if (arg == "--sqlite" && i + 1 < argc) {
            dbType = "sqlite";
            dbPath = argv[++i];
        }
        else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    // Create database instance based on configuration
    std::shared_ptr<IDatabase> database;
    
    try {
        if (dbType == "local") {
            std::cout << "Initializing local file-based database at: " << dbPath << std::endl;
            database = std::make_shared<LocalDatabase>(dbPath);
        }
        else if (dbType == "postgres") {
            std::cout << "Initializing PostgreSQL database..." << std::endl;
            SQLDatabase::ConnectionConfig sqlConfig;
            sqlConfig.type = SQLDatabase::SQLType::POSTGRESQL;
            // TODO: Parse connection string properly
            sqlConfig.host = "localhost";
            sqlConfig.database = "invelog";
            database = std::make_shared<SQLDatabase>(sqlConfig);
        }
        else if (dbType == "mysql") {
            std::cout << "Initializing MySQL database..." << std::endl;
            SQLDatabase::ConnectionConfig sqlConfig;
            sqlConfig.type = SQLDatabase::SQLType::MYSQL;
            sqlConfig.host = "localhost";
            sqlConfig.database = "invelog";
            database = std::make_shared<SQLDatabase>(sqlConfig);
        }
        else if (dbType == "sqlite") {
            std::cout << "Initializing SQLite database at: " << dbPath << std::endl;
            SQLDatabase::ConnectionConfig sqlConfig;
            sqlConfig.type = SQLDatabase::SQLType::SQLITE;
            sqlConfig.database = dbPath;
            database = std::make_shared<SQLDatabase>(sqlConfig);
        }
        else {
            std::cerr << "Unknown database type: " << dbType << std::endl;
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error initializing database: " << e.what() << std::endl;
        return 1;
    }
    
    // Validate API key if authentication is required
    if (config.authRequired && config.apiKey.empty()) {
        std::cout << "WARNING: Authentication enabled but no API key set!" << std::endl;
        std::cout << "Use --api-key <key> to set an API key or --no-auth to disable" << std::endl;
        std::cout << "Continuing without authentication..." << std::endl;
        config.authRequired = false;
    }
    
    // Create and configure server
    g_server = std::make_shared<DatabaseAPIServer>(database, config);
    
    // Set up signal handlers for graceful shutdown
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // Start server
    std::cout << "\n========================================" << std::endl;
    std::cout << "  Invelog Database Server" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Version: 0.3.0 (Modular Architecture)" << std::endl;
    std::cout << "Port: " << config.port << std::endl;
    std::cout << "Database: " << dbType << std::endl;
    std::cout << "Authentication: " << (config.authRequired ? "Enabled" : "Disabled") << std::endl;
    std::cout << "CORS: " << (config.enableCORS ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Max Request Size: " << config.maxRequestSize << " bytes" << std::endl;
    std::cout << "Timeout: " << config.timeoutSeconds << " seconds" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    try {
        g_server->start();
        
        std::cout << "\nServer is running. Press Ctrl+C to stop." << std::endl;
        
        // Keep server running
        while (g_server->isRunning()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "Server stopped." << std::endl;
    
    return 0;
}
