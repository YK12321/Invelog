package handlers

import (
	"net/http"

	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
)

// @Summary List Activity Logs
// @Description Get all activity logs
// @Tags Activity Logs
// @Produce json
// @Success 200 {array} models.ActivityLog
// @Router /activity-logs [get]
func (h *Handler) ListActivityLogs(c *gin.Context) {
	var logs []models.ActivityLog
	h.DB.Order("created_at desc").Find(&logs)
	c.JSON(http.StatusOK, logs)
}
