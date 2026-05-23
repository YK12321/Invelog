package handlers

import (
	"net/http"
	"strconv"

	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
)

// @Summary List Activity Logs
// @Description Get all activity logs
// @Tags Activity Logs
// @Produce json
// @Param limit query int false "Limit the number of logs returned"
// @Param offset query int false "Offset for pagination (default 0)" default(0)
// @Param user_id query string false "User ID for user-specific settings"
// @Success 200 {object} models.PaginatedActivityLogsResponse
// @Router /activity-logs [get]
func (h *Handler) ListActivityLogs(c *gin.Context) {
	limitStr := c.Query("limit")
	offsetStr := c.DefaultQuery("offset", "0")
	userIDStr := c.Query("user_id")

	var userID *uuid.UUID
	if userIDStr != "" {
		if id, err := uuid.Parse(userIDStr); err == nil {
			userID = &id
		}
	}

	requestedLimit := 0
	if limitStr != "" {
		if l, err := strconv.Atoi(limitStr); err == nil {
			requestedLimit = l
		}
	}

	limit := h.Settings.ResolveLimit("ACTIVITY_LOGS_DEFAULT_LIMIT", requestedLimit, userID)

	offset, err := strconv.Atoi(offsetStr)
	if err != nil || offset < 0 {
		offset = 0
	}

	var logs []models.ActivityLog
	var total int64

	h.DB.Model(&models.ActivityLog{}).Count(&total)

	h.DB.Order("created_at desc").
		Limit(limit).
		Offset(offset).
		Find(&logs)

	response := models.PaginatedResponse[models.ActivityLog]{
		Items:  logs,
		Total:  total,
		Limit:  limit,
		Offset: offset,
	}

	c.JSON(http.StatusOK, response)
}
