package handlers

import (
	"net/http"
	"net/http/httptest"
	"strconv"
	"testing"

	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
	"gorm.io/driver/sqlite"
	"gorm.io/gorm"
)

func BenchmarkListItems(b *testing.B) {
	db, err := gorm.Open(sqlite.Open("file::memory:?cache=shared"), &gorm.Config{})
	if err != nil {
		b.Fatalf("failed to connect database: %v", err)
	}

	db.AutoMigrate(&models.Item{}, &models.Category{}, &models.Container{})

	for i := 0; i < 5000; i++ {
		db.Create(&models.Item{Name: "Item " + strconv.Itoa(i)})
	}

	h := NewHandler(db)
	gin.SetMode(gin.ReleaseMode)
	r := gin.New()
	r.GET("/items", h.ListItems)

	b.ResetTimer()
	b.ReportAllocs()
	for i := 0; i < b.N; i++ {
		req, _ := http.NewRequest(http.MethodGet, "/items", nil)
		w := httptest.NewRecorder()
		r.ServeHTTP(w, req)
		if w.Code != http.StatusOK {
			b.Fatalf("expected status 200, got %d", w.Code)
		}
	}
}
