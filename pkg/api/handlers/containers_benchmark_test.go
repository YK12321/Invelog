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

func setupContainersBenchmarkDB(b *testing.B) *gorm.DB {
	db, err := gorm.Open(sqlite.Open("file::memory:?cache=shared"), &gorm.Config{
		Logger: logger.Default.LogMode(logger.Silent),
	})
	if err != nil {
		b.Fatalf("failed to open benchmark database: %v", err)
	}
	if err := db.AutoMigrate(&models.Container{}, &models.Location{}, &models.Project{}); err != nil {
		b.Fatalf("failed to migrate benchmark database: %v", err)
	}

	// Insert 1500 containers
	containers := make([]models.Container, 1500)
	for i := range containers {
		containers[i] = models.Container{Name: "Test Container"}
	}
	if err := db.Create(&containers).Error; err != nil {
		b.Fatalf("failed to seed benchmark containers: %v", err)
	}
	return db
}

func BenchmarkListContainers(b *testing.B) {
	gin.SetMode(gin.ReleaseMode)
	db := setupContainersBenchmarkDB(b)
	handler := &Handler{DB: db}

	router := gin.New()
	router.GET("/containers", handler.ListContainers)

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		req, _ := http.NewRequest("GET", "/containers", nil)
		w := httptest.NewRecorder()
		router.ServeHTTP(w, req)

		if w.Code != http.StatusOK {
			b.Fatalf("expected status 200, got %d", w.Code)
		}
	}
}
