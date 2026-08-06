package handlers

import (
	"fmt"
	"net/http"
	"strconv"
	"strings"

	"invelog/pkg/dto"
	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
	"gorm.io/datatypes"
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

	if req.CategoryID != nil {
		var cat models.Category
		if err := h.DB.First(&cat, "id = ?", req.CategoryID).Error; err != nil {
			c.JSON(http.StatusBadRequest, gin.H{"error": "Referenced category_id does not exist"})
			return
		}
	}

	minQty := 0
	if req.MinQuantity != nil {
		if *req.MinQuantity < 0 {
			c.JSON(http.StatusBadRequest, gin.H{"error": "min_quantity cannot be negative"})
			return
		}
		minQty = *req.MinQuantity
	}

	reorderQty := 0
	if req.ReorderQuantity != nil {
		if *req.ReorderQuantity < 0 {
			c.JSON(http.StatusBadRequest, gin.H{"error": "reorder_quantity cannot be negative"})
			return
		}
		reorderQty = *req.ReorderQuantity
	}

	itemType := models.ItemType{
		Name:            req.Name,
		Description:     req.Description,
		Specifications:  req.Specifications,
		Manufacturer:    req.Manufacturer,
		PartNumber:      req.PartNumber,
		MinQuantity:     minQty,
		ReorderQuantity: reorderQty,
		CategoryID:      req.CategoryID,
	}

	if len(req.Parameters) > 0 {
		itemType.Parameters = datatypes.JSON(req.Parameters)
	}

	if err := h.DB.Create(&itemType).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to create item type"})
		return
	}

	h.LogActivityWithContext(c, "CREATE", "ItemType", itemType.ID, "Created item type: "+itemType.Name)
	c.JSON(http.StatusCreated, itemType)
}

// @Summary List ItemTypes
// @Description Get all item types with optional parametric filters (e.g. param.package=0805)
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

	query := h.DB.Model(&models.ItemType{})

	// Handle parametric query filtering (param.key=val)
	for key, values := range c.Request.URL.Query() {
		if strings.HasPrefix(key, "param.") && len(values) > 0 {
			paramKey := strings.TrimPrefix(key, "param.")
			paramVal := values[0]
			query = query.Where(datatypes.JSONQuery("parameters").Equals(paramVal, paramKey))
		}
	}

	var itemTypes []models.ItemType
	var total int64

	query.Count(&total)
	query.Preload("Category").Order("created_at desc").Limit(limit).Offset(offset).Find(&itemTypes)

	c.Header("X-Total-Count", fmt.Sprintf("%d", total))
	c.Header("X-Limit", fmt.Sprintf("%d", limit))
	c.Header("X-Offset", fmt.Sprintf("%d", offset))

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

	if input.CategoryID != nil {
		var cat models.Category
		if err := h.DB.First(&cat, "id = ?", input.CategoryID).Error; err != nil {
			c.JSON(http.StatusBadRequest, gin.H{"error": "Referenced category_id does not exist"})
			return
		}
		itemType.CategoryID = input.CategoryID
	}

	if input.Name != nil {
		itemType.Name = *input.Name
	}
	if input.Description != nil {
		itemType.Description = *input.Description
	}
	if input.Specifications != nil {
		itemType.Specifications = *input.Specifications
	}
	if len(input.Parameters) > 0 {
		itemType.Parameters = datatypes.JSON(input.Parameters)
	}
	if input.Manufacturer != nil {
		itemType.Manufacturer = *input.Manufacturer
	}
	if input.PartNumber != nil {
		itemType.PartNumber = *input.PartNumber
	}
	if input.MinQuantity != nil {
		if *input.MinQuantity < 0 {
			c.JSON(http.StatusBadRequest, gin.H{"error": "min_quantity cannot be negative"})
			return
		}
		itemType.MinQuantity = *input.MinQuantity
	}
	if input.ReorderQuantity != nil {
		if *input.ReorderQuantity < 0 {
			c.JSON(http.StatusBadRequest, gin.H{"error": "reorder_quantity cannot be negative"})
			return
		}
		itemType.ReorderQuantity = *input.ReorderQuantity
	}

	itemType.Category = nil

	if err := h.DB.Save(&itemType).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to update item type"})
		return
	}

	h.LogActivityWithContext(c, "UPDATE", "ItemType", itemType.ID, "Updated item type: "+itemType.Name)
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

	h.LogActivityWithContext(c, "DELETE", "ItemType", id, "Deleted item type")
	c.JSON(http.StatusNoContent, nil)
}

// @Summary Get Low Stock ItemTypes
// @Description Query component types where aggregate quantity across containers drops below configured min_quantity
// @Tags ItemTypes
// @Produce json
// @Success 200 {array} models.ItemType
// @Router /item-types/low-stock [get]
func (h *Handler) GetLowStockItemTypes(c *gin.Context) {
	var lowStockTypes []models.ItemType

	if err := h.DB.Preload("Category").
		Joins("LEFT JOIN items ON items.item_type_id = item_types.id AND items.deleted_at IS NULL").
		Where("item_types.min_quantity > 0").
		Group("item_types.id").
		Having("COALESCE(SUM(items.quantity), 0) <= item_types.min_quantity").
		Find(&lowStockTypes).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to fetch low stock item types"})
		return
	}

	c.JSON(http.StatusOK, lowStockTypes)
}
