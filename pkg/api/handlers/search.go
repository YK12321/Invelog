package handlers

import (
	"net/http"
	"strings"

	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
)

// @Summary Search Items
// @Description Search across items using query parameters
// @Tags Search
// @Produce json
// @Param query query string false "Search query (matches name, description, notes)"
// @Param category_id query string false "Filter by Category ID"
// @Param container_id query string false "Filter by Container ID"
// @Param project_id query string false "Filter by Project ID"
// @Success 200 {array} models.Item
// @Router /search/items [get]
func (h *Handler) SearchItems(c *gin.Context) {
	query := h.DB.Model(&models.Item{}).
		Preload("Category").
		Preload("Container").
		Preload("ItemType").
		Preload("OriginLocation")

	// Filter by search query
	if q := c.Query("query"); q != "" {
		q = "%" + strings.ToLower(q) + "%"
		// Search across Item fields and associated ItemType fields
		query = query.Joins("LEFT JOIN item_types ON item_types.id = items.item_type_id").
			Where("LOWER(items.name) LIKE ? OR LOWER(items.description) LIKE ? OR LOWER(items.individual_notes) LIKE ? OR LOWER(item_types.name) LIKE ? OR LOWER(item_types.description) LIKE ?", q, q, q, q, q)
	}

	// Filter by category
	if categoryID := c.Query("category_id"); categoryID != "" {
		query = query.Where("items.category_id = ? OR item_types.category_id = ?", categoryID, categoryID)
	}

	// Filter by container
	if containerID := c.Query("container_id"); containerID != "" {
		query = query.Where("items.container_id = ?", containerID)
	}

	// Filter by project (via container)
	if projectID := c.Query("project_id"); projectID != "" {
		query = query.Joins("JOIN containers ON containers.id = items.container_id").
			Where("containers.project_id = ?", projectID)
	}

	var items []models.Item
	if err := query.Find(&items).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Search failed"})
		return
	}

	c.JSON(http.StatusOK, items)
}
