package handlers

import (
	"fmt"
	"net/http"
	"strconv"

	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
)

// @Summary List Activity Logs
// @Description Get all activity logs
// @Tags Activity Logs
// @Produce json
// @Param limit query int false "Limit (default 20, max 100)"
// @Param offset query int false "Offset (default 0)"
// @Success 200 {array} models.ActivityLog
// @Router /activity-logs [get]
func (h *Handler) ListActivityLogs(c *gin.Context) {
	limitStr := c.DefaultQuery("limit", "20")
	offsetStr := c.DefaultQuery("offset", "0")

	limit, err := strconv.Atoi(limitStr)
	if err != nil || limit <= 0 {
		limit = 20
	}
	if limit > 100 {
		limit = 100
	}

	offset, err := strconv.Atoi(offsetStr)
	if err != nil || offset < 0 {
		offset = 0
	}

	var logs []models.ActivityLog
	var total int64

	// Get total count
	h.DB.Model(&models.ActivityLog{}).Count(&total)

	// Get paginated results
	h.DB.Order("created_at desc").Limit(limit).Offset(offset).Find(&logs)

	c.Header("X-Total-Count", fmt.Sprintf("%d", total))
	c.Header("X-Limit", fmt.Sprintf("%d", limit))
	c.Header("X-Offset", fmt.Sprintf("%d", offset))

	c.JSON(http.StatusOK, logs)
}
