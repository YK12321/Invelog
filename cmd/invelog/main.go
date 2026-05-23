package main

import (
	"log"
	"os"

	"invelog/pkg/api/router"
	"invelog/pkg/database"

	// Swagger documentation
	_ "invelog/docs"
)

// @title Invelog API
// @version 1.0
// @description A comprehensive inventory management system backend for electronic components.
// @termsOfService http://swagger.io/terms/

// @contact.name API Support
// @contact.url http://www.swagger.io/support
// @contact.email support@swagger.io

// @license.name MIT
// @license.url https://opensource.org/licenses/MIT

// @host localhost:8080
// @BasePath /api/v1
func main() {
	// 1. Initialize Database
	dbConfig := database.Config{
		Type:     getEnv("DB_TYPE", "sqlite"),
		Database: getEnv("DB_NAME", ""),
		Host:     getEnv("DB_HOST", "localhost"),
		Port:     getEnv("DB_PORT", "5432"),
		User:     getEnv("DB_USER", "postgres"),
		Password: getEnv("DB_PASSWORD", "postgres"),
		SSLMode:  getEnv("DB_SSLMODE", "disable"),
	}

	db, err := database.Connect(dbConfig)
	if err != nil {
		log.Fatalf("Failed to connect to database: %v", err)
	}

	// 2. Run Migrations
	if err := database.Migrate(db); err != nil {
		log.Fatalf("Failed to run database migrations: %v", err)
	}

	// 3. Setup Router
	r := router.SetupRouter(db)

	// 4. Start Server
	port := getEnv("PORT", "8080")
	log.Printf("Starting Invelog server on port %s", port)
	if err := r.Run(":" + port); err != nil {
		log.Fatalf("Failed to start server: %v", err)
	}
}

// Helper to get env variable with fallback
func getEnv(key, fallback string) string {
	if value, exists := os.LookupEnv(key); exists {
		return value
	}
	return fallback
}
