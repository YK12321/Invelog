package handlers

import (
	"net/http"
	"time"

	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
)

// @Summary Check out an item
// @Description Check out an item by marking it as checked out and logging the activity
// @Tags Items
// @Produce json
// @Param id path string true "Item ID"
// @Success 200 {object} models.Item
// @Failure 400 {object} map[string]string
// @Failure 404 {object} map[string]string
// @Router /items/{id}/checkout [post]
func (h *Handler) CheckOutItem(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Item ID"})
		return
	}

	var item models.Item
	if err := h.DB.First(&item, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Item not found"})
		return
	}

	if item.CheckedOut {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Item is already checked out"})
		return
	}

	now := time.Now()
	item.CheckedOut = true
	item.LastCheckOutTime = &now

	if err := h.DB.Save(&item).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to check out item"})
		return
	}

	h.LogActivityDetailed("CHECK_OUT", "Item", item.ID, "Item checked out", nil, nil, nil, 0)
	c.JSON(http.StatusOK, item)
}

// @Summary Check in an item
// @Description Check in an item by marking it as not checked out and logging the activity
// @Tags Items
// @Produce json
// @Param id path string true "Item ID"
// @Success 200 {object} models.Item
// @Failure 400 {object} map[string]string
// @Failure 404 {object} map[string]string
// @Router /items/{id}/checkin [post]
func (h *Handler) CheckInItem(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Item ID"})
		return
	}

	var item models.Item
	if err := h.DB.First(&item, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Item not found"})
		return
	}

	if !item.CheckedOut {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Item is not checked out"})
		return
	}

	item.CheckedOut = false

	if err := h.DB.Save(&item).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to check in item"})
		return
	}

	h.LogActivityDetailed("CHECK_IN", "Item", item.ID, "Item checked in", nil, nil, nil, 0)
	c.JSON(http.StatusOK, item)
}
