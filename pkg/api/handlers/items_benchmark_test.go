package handlers

import (
	"net/http"
	"net/http/httptest"
	"testing"

	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
	"gorm.io/driver/sqlite"
	"gorm.io/gorm"
	"gorm.io/gorm/logger"
)

func setupBenchmarkDB(b *testing.B) *gorm.DB {
	db, err := gorm.Open(sqlite.Open("file::memory:?cache=shared"), &gorm.Config{
		Logger: logger.Default.LogMode(logger.Silent),
	})
	if err != nil {
		b.Fatalf("failed to open benchmark database: %v", err)
	}
	if err := db.AutoMigrate(&models.Item{}, &models.Category{}, &models.Container{}); err != nil {
		b.Fatalf("failed to migrate benchmark database: %v", err)
	}

	// Insert 1500 items to see the pagination effect (default limit 1000)
	items := make([]models.Item, 1500)
	for i := range items {
		items[i] = models.Item{Name: "Test Item", Quantity: 1}
	}
	if err := db.Create(&items).Error; err != nil {
		b.Fatalf("failed to seed benchmark items: %v", err)
	}
	return db
}

func BenchmarkListItems(b *testing.B) {
	gin.SetMode(gin.ReleaseMode)
	db := setupBenchmarkDB(b)
	handler := &Handler{DB: db}

	router := gin.New()
	router.GET("/items", handler.ListItems)

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		req, _ := http.NewRequest("GET", "/items", nil)
		w := httptest.NewRecorder()
		router.ServeHTTP(w, req)

		if w.Code != http.StatusOK {
			b.Fatalf("expected status 200, got %d", w.Code)
		}
	}
}
