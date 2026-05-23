package handlers

import (
	"net/http"
	"strconv"

	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
)

// @Summary List Activity Logs
// @Description Get all activity logs
// @Tags Activity Logs
// @Produce json
// @Param limit query int false "Limit (default 100, max 1000)"
// @Param offset query int false "Offset (default 0)"
// @Success 200 {array} models.ActivityLog
// @Router /activity-logs [get]
func (h *Handler) ListActivityLogs(c *gin.Context) {
	limitStr := c.DefaultQuery("limit", "100")
	offsetStr := c.DefaultQuery("offset", "0")

	limit, err := strconv.Atoi(limitStr)
	if err != nil || limit <= 0 {
		limit = 100
	}
	if limit > 1000 {
		limit = 1000
	}

	offset, err := strconv.Atoi(offsetStr)
	if err != nil || offset < 0 {
		offset = 0
	}

	var logs []models.ActivityLog
	h.DB.Order("created_at desc").Limit(limit).Offset(offset).Find(&logs)
	c.JSON(http.StatusOK, logs)
}
