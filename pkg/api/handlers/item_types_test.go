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

func setupTestDB(b *testing.B) *Handler {
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
	b.Log("Seeding 5000 ItemTypes for benchmark...")
	for i := 0; i < 5000; i++ {
		itemType := models.ItemType{
			Name:         fmt.Sprintf("Component %d", i),
			Manufacturer: "TestCorp",
			PartNumber:   fmt.Sprintf("TC-%d", i),
		}
		if err := db.Create(&itemType).Error; err != nil {
			b.Fatalf("failed to seed data: %v", err)
		}
	}
	b.Log("Seeding complete.")

	return h
}

func BenchmarkListItemTypes(b *testing.B) {
	h := setupTestDB(b)
	router := gin.New()
	router.GET("/item-types", h.ListItemTypes)

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		req, _ := http.NewRequest("GET", "/item-types", nil)
		w := httptest.NewRecorder()
		router.ServeHTTP(w, req)

		if w.Code != http.StatusOK {
			b.Fatalf("expected status 200, got %d", w.Code)
		}

		// Just to verify it's working
		if i == 0 {
			var resp models.PaginatedResponse[models.ItemType]
			json.Unmarshal(w.Body.Bytes(), &resp)
			b.Logf("Retrieved %d items on first request (Total: %d)", len(resp.Items), resp.Total)
		}
	}
}
