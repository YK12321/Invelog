package handlers

import (
	"net/http"
	"strconv"

	"invelog/pkg/dto"
	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
)

// @Summary Create ItemType
// @Description Create a new item type
// @Tags ItemTypes
// @Accept json
// @Produce json
// @Param itemType body dto.CreateItemTypeRequest true "ItemType Data"
// @Success 201 {object} models.ItemType
// @Failure 400 {object} map[string]string
// @Router /item-types [post]
func (h *Handler) CreateItemType(c *gin.Context) {
	var req dto.CreateItemTypeRequest
	if err := c.ShouldBindJSON(&req); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	itemType := models.ItemType{
		Name:           req.Name,
		Description:    req.Description,
		Specifications: req.Specifications,
		Manufacturer:   req.Manufacturer,
		PartNumber:     req.PartNumber,
		CategoryID:     req.CategoryID,
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
// @Param limit query int false "Limit (default 100, max 1000)"
// @Param offset query int false "Offset (default 0)"
// @Success 200 {array} models.ItemType
// @Router /item-types [get]
func (h *Handler) ListItemTypes(c *gin.Context) {
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

	var itemTypes []models.ItemType
	h.DB.Preload("Category").Limit(limit).Offset(offset).Find(&itemTypes)
	c.JSON(http.StatusOK, itemTypes)
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
// @Param itemType body dto.UpdateItemTypeInput true "ItemType Data"
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

	var input dto.UpdateItemTypeInput
	if err := c.ShouldBindJSON(&input); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	updates := map[string]interface{}{}
	if input.Name != nil {
		updates["name"] = *input.Name
		itemType.Name = *input.Name
	}
	if input.Description != nil {
		updates["description"] = *input.Description
		itemType.Description = *input.Description
	}
	if input.Specifications != nil {
		updates["specifications"] = *input.Specifications
		itemType.Specifications = *input.Specifications
	}
	if input.Manufacturer != nil {
		updates["manufacturer"] = *input.Manufacturer
		itemType.Manufacturer = *input.Manufacturer
	}
	if input.PartNumber != nil {
		updates["part_number"] = *input.PartNumber
		itemType.PartNumber = *input.PartNumber
	}
	if input.CategoryID != nil {
		updates["category_id"] = input.CategoryID
		itemType.CategoryID = input.CategoryID
	}

	if len(updates) == 0 {
		c.JSON(http.StatusBadRequest, gin.H{"error": "No fields to update"})
		return
	}

	if err := h.DB.Model(&itemType).Updates(updates).Error; err != nil {
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
