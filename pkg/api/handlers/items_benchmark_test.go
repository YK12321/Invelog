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

func setupBenchmarkDB() *gorm.DB {
	db, err := gorm.Open(sqlite.Open("file::memory:?cache=shared"), &gorm.Config{
		Logger: logger.Default.LogMode(logger.Silent),
	})
	if err != nil {
		panic(err)
	}
	db.AutoMigrate(&models.Item{}, &models.Category{}, &models.Container{})

	// Insert 1500 items to see the pagination effect (default limit 1000)
	var items []models.Item
	for i := 0; i < 1500; i++ {
		items = append(items, models.Item{Name: "Test Item", Quantity: 1})
	}
	db.Create(&items)
	return db
}

func BenchmarkListItems(b *testing.B) {
	gin.SetMode(gin.ReleaseMode)
	db := setupBenchmarkDB()
	handler := &Handler{DB: db}

	router := gin.New()
	router.GET("/items", handler.ListItems)

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		req, _ := http.NewRequest("GET", "/items", nil)
		w := httptest.NewRecorder()
		router.ServeHTTP(w, req)
	}
}
