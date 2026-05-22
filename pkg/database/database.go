package database

import (
	"fmt"
	"log"
	"os"

	"invelog/pkg/models"

	"gorm.io/driver/mysql"
	"gorm.io/driver/postgres"
	"gorm.io/driver/sqlite"
	"gorm.io/gorm"
	"gorm.io/gorm/logger"
)

type Config struct {
	Type     string // sqlite, postgres, mysql
	Host     string
	Port     string
	User     string
	Password string
	Database string
	SSLMode  string
}

func Connect(cfg Config) (*gorm.DB, error) {
	var dialector gorm.Dialector

	switch cfg.Type {
	case "sqlite":
		// Database acts as file path for sqlite
		dbPath := cfg.Database
		if dbPath == "" {
			dbPath = "invelog.db"
		}
		dialector = sqlite.Open(dbPath)
	case "postgres":
		dsn := fmt.Sprintf("host=%s user=%s password=%s dbname=%s port=%s sslmode=%s",
			cfg.Host, cfg.User, cfg.Password, cfg.Database, cfg.Port, cfg.SSLMode)
		dialector = postgres.Open(dsn)
	case "mysql":
		dsn := fmt.Sprintf("%s:%s@tcp(%s:%s)/%s?charset=utf8mb4&parseTime=True&loc=Local",
			cfg.User, cfg.Password, cfg.Host, cfg.Port, cfg.Database)
		dialector = mysql.Open(dsn)
	default:
		return nil, fmt.Errorf("unsupported database type: %s", cfg.Type)
	}

	// Adjust logger based on environment, use silent by default in production
	logLevel := logger.Warn
	if os.Getenv("DEBUG") == "true" {
		logLevel = logger.Info
	}

	db, err := gorm.Open(dialector, &gorm.Config{
		Logger: logger.Default.LogMode(logLevel),
	})
	if err != nil {
		return nil, err
	}

	return db, nil
}

func Migrate(db *gorm.DB) error {
	log.Println("Running database migrations...")
	err := db.AutoMigrate(
		&models.Category{},
		&models.Location{},
		&models.Project{},
		&models.Container{},
		&models.ItemType{},
		&models.Item{},
		&models.ActivityLog{},
	)
	if err != nil {
		return err
	}
	log.Println("Database migrations completed successfully.")
	return nil
}
