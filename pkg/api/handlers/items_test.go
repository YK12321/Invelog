package handlers

import (
	"encoding/json"
	"fmt"
	"net/http"
	"net/http/httptest"
	"testing"

	"invelog/pkg/database"
	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
)

func setupItemsTestDB(b *testing.B) *Handler {
	gin.SetMode(gin.ReleaseMode)

	db, err := database.Connect(database.Config{
		Type:     "sqlite",
		Database: ":memory:",
	})
	if err != nil {
		b.Fatalf("failed to connect database: %v", err)
	}

	if err := database.Migrate(db); err != nil {
		b.Fatalf("failed to migrate database: %v", err)
	}

	h := NewHandler(db)

	// Seed data
	b.Log("Seeding 5000 Items for benchmark...")
	for i := 0; i < 5000; i++ {
		item := models.Item{
			Name:        fmt.Sprintf("Item %d", i),
			Description: fmt.Sprintf("Description %d", i),
			Quantity:    1,
		}
		if err := db.Create(&item).Error; err != nil {
			b.Fatalf("failed to seed data: %v", err)
		}
	}
	b.Log("Seeding complete.")

	return h
}

func BenchmarkListItems(b *testing.B) {
	h := setupItemsTestDB(b)
	router := gin.New()
	router.GET("/items", h.ListItems)

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		req, _ := http.NewRequest("GET", "/items", nil)
		w := httptest.NewRecorder()
		router.ServeHTTP(w, req)

		if w.Code != http.StatusOK {
			b.Fatalf("expected status 200, got %d", w.Code)
		}

		if i == 0 {
			var items []models.Item
			json.Unmarshal(w.Body.Bytes(), &items)
			b.Logf("Retrieved %d items on first request", len(items))
		}
	}
}
