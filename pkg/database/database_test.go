package database

import (
	"testing"
)

func TestConnectEmptyDatabaseName(t *testing.T) {
	tests := []struct {
		name     string
		dbType   string
	}{
		{"SQLite Empty DB Name", "sqlite"},
		{"Postgres Empty DB Name", "postgres"},
		{"MySQL Empty DB Name", "mysql"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			cfg := Config{
				Type:     tt.dbType,
				Database: "",
				Host:     "localhost",
				Port:     "5432",
				User:     "user",
				Password: "password",
				SSLMode:  "disable",
			}

			db, err := Connect(cfg)
			if err == nil {
				t.Errorf("Expected an error for empty database name with type %s, but got nil", tt.dbType)
			} else if err.Error() != "database name (DB_NAME) must be provided" {
				t.Errorf("Expected error message 'database name (DB_NAME) must be provided', got: %v", err)
			}

			if db != nil {
				t.Errorf("Expected nil db when error occurs, but got %v", db)
			}
		})
	}
}
