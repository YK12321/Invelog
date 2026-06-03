package handlers

import (
	"net/http"
	"strconv"

	"invelog/pkg/dto"
	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
)

// @Summary Create Item
// @Description Create a new item
// @Tags Items
// @Accept json
// @Produce json
// @Param item body dto.CreateItemRequest true "Item Data"
// @Success 201 {object} models.Item
// @Failure 400 {object} map[string]string
// @Router /items [post]
func (h *Handler) CreateItem(c *gin.Context) {
	var req dto.CreateItemRequest
	if err := c.ShouldBindJSON(&req); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	item := models.Item{
		Name:             req.Name,
		Description:      req.Description,
		IndividualNotes:  req.IndividualNotes,
		SerialNumber:     req.SerialNumber,
		ItemTypeID:       req.ItemTypeID,
		CategoryID:       req.CategoryID,
		ContainerID:      req.ContainerID,
		OriginLocationID: req.OriginLocationID,
	}

	if req.Quantity != nil {
		item.Quantity = *req.Quantity
	} else {
		item.Quantity = 1
	}

	if err := h.DB.Create(&item).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to create item"})
		return
	}

	h.LogActivity("CREATE", "Item", item.ID, "Created item: "+item.Name)
	c.JSON(http.StatusCreated, item)
}

// @Summary List Items
// @Description Get all items with pagination
// @Tags Items
// @Produce json
// @Param limit query int false "Limit the number of items returned"
// @Param offset query int false "Offset for pagination (default 0)" default(0)
// @Param user_id query string false "User ID for user-specific settings"
// @Success 200 {object} models.PaginatedItemsResponse
// @Router /items [get]
func (h *Handler) ListItems(c *gin.Context) {
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

	limit := h.Settings.ResolveLimit("ITEMS_DEFAULT_LIMIT", requestedLimit, userID)

	offset, err := strconv.Atoi(offsetStr)
	if err != nil || offset < 0 {
		offset = 0
	}

	var items []models.Item
	var total int64

	if err := h.DB.Model(&models.Item{}).Count(&total).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to list items"})
		return
	}

	if err := h.DB.Preload("Category").Preload("Container").Order("created_at desc").Limit(limit).Offset(offset).Find(&items).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to list items"})
		return
	}

	response := models.PaginatedResponse[models.Item]{
		Items:  items,
		Total:  total,
		Limit:  limit,
		Offset: offset,
	}

	c.JSON(http.StatusOK, response)
}

// @Summary Get Item
// @Description Get an item by ID
// @Tags Items
// @Produce json
// @Param id path string true "Item ID"
// @Success 200 {object} models.Item
// @Failure 404 {object} map[string]string
// @Router /items/{id} [get]
func (h *Handler) GetItem(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	var item models.Item
	if err := h.DB.Preload("Category").Preload("Container").First(&item, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Item not found"})
		return
	}

	c.JSON(http.StatusOK, item)
}

type UpdateItemInput struct {
	Name             *string    `json:"name"`
	Description      *string    `json:"description"`
	Quantity         *int       `json:"quantity"`
	IndividualNotes  *string    `json:"individual_notes"`
	SerialNumber     *string    `json:"serial_number"`
	ItemTypeID       *uuid.UUID `json:"item_type_id"`
	CategoryID       *uuid.UUID `json:"category_id"`
	ContainerID      *uuid.UUID `json:"container_id"`
	OriginLocationID *uuid.UUID `json:"origin_location_id"`
}

// @Summary Update Item
// @Description Update an item by ID
// @Tags Items
// @Accept json
// @Produce json
// @Param id path string true "Item ID"
// @Param item body UpdateItemInput true "Item Update Data"
// @Success 200 {object} models.Item
// @Failure 400 {object} map[string]string
// @Failure 404 {object} map[string]string
// @Router /items/{id} [put]
func (h *Handler) UpdateItem(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	var item models.Item
	if err := h.DB.First(&item, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Item not found"})
		return
	}

	var input UpdateItemInput
	if err := c.ShouldBindJSON(&input); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	if input.Name != nil {
		item.Name = *input.Name
	}
	if input.Description != nil {
		item.Description = *input.Description
	}
	if input.Quantity != nil {
		item.Quantity = *input.Quantity
	}
	if input.IndividualNotes != nil {
		item.IndividualNotes = *input.IndividualNotes
	}
	if input.SerialNumber != nil {
		item.SerialNumber = *input.SerialNumber
	}
	if input.ItemTypeID != nil {
		item.ItemTypeID = input.ItemTypeID
	}
	if input.CategoryID != nil {
		item.CategoryID = input.CategoryID
	}
	if input.ContainerID != nil {
		item.ContainerID = input.ContainerID
	}
	if input.OriginLocationID != nil {
		item.OriginLocationID = input.OriginLocationID
	}

	if err := h.DB.Save(&item).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to update item"})
		return
	}

	h.LogActivity("UPDATE", "Item", item.ID, "Updated item: "+item.Name)
	c.JSON(http.StatusOK, item)
}

// @Summary Delete Item
// @Description Delete an item by ID
// @Tags Items
// @Produce json
// @Param id path string true "Item ID"
// @Success 204
// @Failure 400 {object} map[string]string
// @Failure 404 {object} map[string]string
// @Router /items/{id} [delete]
func (h *Handler) DeleteItem(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	if err := h.DB.Delete(&models.Item{}, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to delete item"})
		return
	}

	h.LogActivity("DELETE", "Item", id, "Deleted item")
	c.JSON(http.StatusNoContent, nil)
}

// @Summary Move Item
// @Description Move an item to a different container
// @Tags Items
// @Produce json
// @Param id path string true "Item ID"
// @Param container_id query string true "New Container ID"
// @Success 200 {object} models.Item
// @Failure 400 {object} map[string]string
// @Failure 404 {object} map[string]string
// @Router /items/{id}/move [post]
func (h *Handler) MoveItem(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Item ID"})
		return
	}

	containerIDStr := c.Query("container_id")
	containerID, err := uuid.Parse(containerIDStr)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Container ID"})
		return
	}

	var item models.Item
	if err := h.DB.First(&item, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Item not found"})
		return
	}

	var container models.Container
	if err := h.DB.First(&container, "id = ?", containerID).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Container not found"})
		return
	}

	item.ContainerID = &containerID
	if err := h.DB.Save(&item).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to move item"})
		return
	}

	h.LogActivity("MOVE", "Item", item.ID, "Moved item to container: "+containerID.String())
	c.JSON(http.StatusOK, item)
}
