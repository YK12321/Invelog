package handlers

import (
	"net/http"
	"strconv"

	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
)

// @Summary Create ItemType
// @Description Create a new item type
// @Tags ItemTypes
// @Accept json
// @Produce json
// @Param itemType body models.ItemType true "ItemType Data"
// @Success 201 {object} models.ItemType
// @Failure 400 {object} map[string]string
// @Router /item-types [post]
func (h *Handler) CreateItemType(c *gin.Context) {
	var itemType models.ItemType
	if err := c.ShouldBindJSON(&itemType); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	if err := h.DB.Create(&itemType).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to create item type"})
		return
	}

	h.LogActivity("CREATE", "ItemType", itemType.ID, "Created item type: "+itemType.Name)
	c.JSON(http.StatusCreated, itemType)
}

// @Summary List ItemTypes
// @Description Get all item types
// @Tags ItemTypes
// @Produce json
// @Param limit query int false "Limit the number of item types returned"
// @Param offset query int false "Offset for pagination (default 0)" default(0)
// @Param user_id query string false "User ID for user-specific settings"
// @Success 200 {object} models.PaginatedItemTypesResponse
// @Router /item-types [get]
func (h *Handler) ListItemTypes(c *gin.Context) {
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

	limit := h.Settings.ResolveLimit("ITEM_TYPES_DEFAULT_LIMIT", requestedLimit, userID)

	offset, err := strconv.Atoi(offsetStr)
	if err != nil || offset < 0 {
		offset = 0
	}

	var itemTypes []models.ItemType
	var total int64

	h.DB.Model(&models.ItemType{}).Count(&total)

	h.DB.Preload("Category").
		Limit(limit).
		Offset(offset).
		Find(&itemTypes)

	response := models.PaginatedResponse[models.ItemType]{
		Items:  itemTypes,
		Total:  total,
		Limit:  limit,
		Offset: offset,
	}

	c.JSON(http.StatusOK, response)
}

// @Summary Get ItemType
// @Description Get an item type by ID
// @Tags ItemTypes
// @Produce json
// @Param id path string true "ItemType ID"
// @Success 200 {object} models.ItemType
// @Failure 404 {object} map[string]string
// @Router /item-types/{id} [get]
func (h *Handler) GetItemType(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	var itemType models.ItemType
	if err := h.DB.Preload("Category").First(&itemType, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "ItemType not found"})
		return
	}

	c.JSON(http.StatusOK, itemType)
}

// @Summary Update ItemType
// @Description Update an item type by ID
// @Tags ItemTypes
// @Accept json
// @Produce json
// @Param id path string true "ItemType ID"
// @Param itemType body models.ItemType true "ItemType Data"
// @Success 200 {object} models.ItemType
// @Failure 400 {object} map[string]string
// @Failure 404 {object} map[string]string
// @Router /item-types/{id} [put]
func (h *Handler) UpdateItemType(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	var itemType models.ItemType
	if err := h.DB.First(&itemType, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "ItemType not found"})
		return
	}

	if err := c.ShouldBindJSON(&itemType); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}
	itemType.ID = id // Ensure ID cannot be changed

	if err := h.DB.Save(&itemType).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to update item type"})
		return
	}

	h.LogActivity("UPDATE", "ItemType", itemType.ID, "Updated item type: "+itemType.Name)
	c.JSON(http.StatusOK, itemType)
}

// @Summary Delete ItemType
// @Description Delete an item type by ID
// @Tags ItemTypes
// @Produce json
// @Param id path string true "ItemType ID"
// @Success 204
// @Failure 400 {object} map[string]string
// @Failure 404 {object} map[string]string
// @Router /item-types/{id} [delete]
func (h *Handler) DeleteItemType(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	if err := h.DB.Delete(&models.ItemType{}, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to delete item type"})
		return
	}

	h.LogActivity("DELETE", "ItemType", id, "Deleted item type")
	c.JSON(http.StatusNoContent, nil)
}
