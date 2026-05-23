package handlers

import (
	"net/http"

	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
)

type UpdateCategoryInput struct {
	Name        *string `json:"name"`
	Description *string `json:"description"`
}

// @Summary Create Category
// @Description Create a new category
// @Tags Categories
// @Accept json
// @Produce json
// @Param category body models.Category true "Category Data"
// @Success 201 {object} models.Category
// @Failure 400 {object} map[string]string
// @Router /categories [post]
func (h *Handler) CreateCategory(c *gin.Context) {
	var category models.Category
	if err := c.ShouldBindJSON(&category); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	if err := h.DB.Create(&category).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to create category"})
		return
	}

	h.LogActivity("CREATE", "Category", category.ID, "Created category: "+category.Name)
	c.JSON(http.StatusCreated, category)
}

// @Summary List Categories
// @Description Get all categories
// @Tags Categories
// @Produce json
// @Success 200 {array} models.Category
// @Router /categories [get]
func (h *Handler) ListCategories(c *gin.Context) {
	var categories []models.Category
	h.DB.Find(&categories)
	c.JSON(http.StatusOK, categories)
}

// @Summary Get Category
// @Description Get a category by ID
// @Tags Categories
// @Produce json
// @Param id path string true "Category ID"
// @Success 200 {object} models.Category
// @Failure 404 {object} map[string]string
// @Router /categories/{id} [get]
func (h *Handler) GetCategory(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	var category models.Category
	if err := h.DB.First(&category, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Category not found"})
		return
	}

	c.JSON(http.StatusOK, category)
}

// @Summary Update Category
// @Description Update a category by ID
// @Tags Categories
// @Accept json
// @Produce json
// @Param id path string true "Category ID"
// @Param category body UpdateCategoryInput true "Category Data"
// @Success 200 {object} models.Category
// @Failure 400 {object} map[string]string
// @Failure 404 {object} map[string]string
// @Router /categories/{id} [put]
func (h *Handler) UpdateCategory(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	var category models.Category
	if err := h.DB.First(&category, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Category not found"})
		return
	}

	var input UpdateCategoryInput
	if err := c.ShouldBindJSON(&input); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	// Map updated fields
	if input.Name != nil {
		category.Name = *input.Name
	}
	if input.Description != nil {
		category.Description = *input.Description
	}

	if err := h.DB.Save(&category).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to update category"})
		return
	}

	h.LogActivity("UPDATE", "Category", category.ID, "Updated category: "+category.Name)
	c.JSON(http.StatusOK, category)
}

// @Summary Delete Category
// @Description Delete a category by ID
// @Tags Categories
// @Produce json
// @Param id path string true "Category ID"
// @Success 204
// @Failure 400 {object} map[string]string
// @Failure 404 {object} map[string]string
// @Router /categories/{id} [delete]
func (h *Handler) DeleteCategory(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	if err := h.DB.Delete(&models.Category{}, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to delete category"})
		return
	}

	h.LogActivity("DELETE", "Category", id, "Deleted category")
	c.JSON(http.StatusNoContent, nil)
}
