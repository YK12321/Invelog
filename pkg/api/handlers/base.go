package handlers

import (
	"net/http"

	"invelog/pkg/models"
	"invelog/pkg/settings"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
	"gorm.io/gorm"
)

type Handler struct {
	DB       *gorm.DB
	Settings *settings.SettingsManager
}

func NewHandler(db *gorm.DB) *Handler {
	return &Handler{
		DB:       db,
		Settings: settings.NewSettingsManager(db),
	}
}

// @Summary Health Check
// @Description Check if the API is running
// @Tags System
// @Produce json
// @Success 200 {object} map[string]string
// @Router /health [get]
func (h *Handler) HealthCheck(c *gin.Context) {
	c.JSON(http.StatusOK, gin.H{"status": "ok"})
}

// LogActivity helps log actions in the database
func (h *Handler) LogActivity(action, entityType string, entityID uuid.UUID, details string) {
	h.LogActivityDetailed(action, entityType, entityID, details, nil, nil, nil, 0)
}

// LogActivityDetailed helps log actions with full context in the database
func (h *Handler) LogActivityDetailed(action, entityType string, entityID uuid.UUID, details string, fromContainerID, toContainerID, projectID *uuid.UUID, quantityChange int) {
	log := models.ActivityLog{
		Action:          action,
		EntityType:      entityType,
		EntityID:        entityID,
		Details:         details,
		FromContainerID: fromContainerID,
		ToContainerID:   toContainerID,
		ProjectID:       projectID,
		QuantityChange:  quantityChange,
	}
	h.DB.Create(&log)
}
