package handlers

import (
	"net/http"

	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
)

// @Summary Create Location
// @Description Create a new location
// @Tags Locations
// @Accept json
// @Produce json
// @Param location body models.Location true "Location Data"
// @Success 201 {object} models.Location
// @Failure 400 {object} map[string]string
// @Router /locations [post]
func (h *Handler) CreateLocation(c *gin.Context) {
	var location models.Location
	if err := c.ShouldBindJSON(&location); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	if err := h.DB.Create(&location).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to create location"})
		return
	}

	h.LogActivity("CREATE", "Location", location.ID, "Created location: "+location.Name)
	c.JSON(http.StatusCreated, location)
}

// @Summary List Locations
// @Description Get all locations
// @Tags Locations
// @Produce json
// @Success 200 {array} models.Location
// @Router /locations [get]
func (h *Handler) ListLocations(c *gin.Context) {
	var locations []models.Location
	h.DB.Find(&locations)
	c.JSON(http.StatusOK, locations)
}

// @Summary Get Location
// @Description Get a location by ID
// @Tags Locations
// @Produce json
// @Param id path string true "Location ID"
// @Success 200 {object} models.Location
// @Failure 404 {object} map[string]string
// @Router /locations/{id} [get]
func (h *Handler) GetLocation(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	var location models.Location
	if err := h.DB.First(&location, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Location not found"})
		return
	}

	c.JSON(http.StatusOK, location)
}

// @Summary Update Location
// @Description Update a location by ID
// @Tags Locations
// @Accept json
// @Produce json
// @Param id path string true "Location ID"
// @Param location body models.Location true "Location Data"
// @Success 200 {object} models.Location
// @Failure 400 {object} map[string]string
// @Failure 404 {object} map[string]string
// @Router /locations/{id} [put]
func (h *Handler) UpdateLocation(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	var location models.Location
	if err := h.DB.First(&location, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Location not found"})
		return
	}

	if err := c.ShouldBindJSON(&location); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}
	location.ID = id // Ensure ID cannot be changed

	if err := h.DB.Save(&location).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to update location"})
		return
	}

	h.LogActivity("UPDATE", "Location", location.ID, "Updated location: "+location.Name)
	c.JSON(http.StatusOK, location)
}

// @Summary Delete Location
// @Description Delete a location by ID
// @Tags Locations
// @Produce json
// @Param id path string true "Location ID"
// @Success 204
// @Failure 400 {object} map[string]string
// @Failure 404 {object} map[string]string
// @Router /locations/{id} [delete]
func (h *Handler) DeleteLocation(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	if err := h.DB.Delete(&models.Location{}, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to delete location"})
		return
	}

	h.LogActivity("DELETE", "Location", id, "Deleted location")
	c.JSON(http.StatusNoContent, nil)
}
