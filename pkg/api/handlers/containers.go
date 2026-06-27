package handlers

import (
	"net/http"

	"invelog/pkg/dto"
	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
)

// @Summary Create Container
// @Description Create a new container
// @Tags Containers
// @Accept json
// @Produce json
// @Param container body dto.CreateContainerRequest true "Container Data"
// @Success 201 {object} models.Container
// @Failure 400 {object} map[string]string
// @Router /containers [post]
func (h *Handler) CreateContainer(c *gin.Context) {
	var req dto.CreateContainerRequest
	if err := c.ShouldBindJSON(&req); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	container := models.Container{
		Name:        req.Name,
		Description: req.Description,
		LocationID:  req.LocationID,
		ParentID:    req.ParentID,
		ProjectID:   req.ProjectID,
	}

	if err := h.DB.Create(&container).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to create container"})
		return
	}

	h.LogActivity("CREATE", "Container", container.ID, "Created container: "+container.Name)
	c.JSON(http.StatusCreated, container)
}

// @Summary List Containers
// @Description Get all containers
// @Tags Containers
// @Produce json
// @Success 200 {array} models.Container
// @Router /containers [get]
func (h *Handler) ListContainers(c *gin.Context) {
	var containers []models.Container
	h.DB.Preload("Location").Preload("Parent").Preload("Project").Find(&containers)
	c.JSON(http.StatusOK, containers)
}

// @Summary Get Container
// @Description Get a container by ID
// @Tags Containers
// @Produce json
// @Param id path string true "Container ID"
// @Success 200 {object} models.Container
// @Failure 404 {object} map[string]string
// @Router /containers/{id} [get]
func (h *Handler) GetContainer(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	var container models.Container
	if err := h.DB.Preload("Location").Preload("Parent").Preload("Project").First(&container, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Container not found"})
		return
	}

	c.JSON(http.StatusOK, container)
}

// @Summary Update Container
// @Description Update a container by ID
// @Tags Containers
// @Accept json
// @Produce json
// @Param id path string true "Container ID"
// @Param container body models.Container true "Container Data"
// @Success 200 {object} models.Container
// @Failure 400 {object} map[string]string
// @Failure 404 {object} map[string]string
// @Router /containers/{id} [put]
func (h *Handler) UpdateContainer(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	var container models.Container
	if err := h.DB.First(&container, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Container not found"})
		return
	}

	if err := c.ShouldBindJSON(&container); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}
	container.ID = id // Ensure ID cannot be changed

	// Security fix: Set relational struct pointers to nil to prevent mass assignment vulnerabilities
	// when saving the model directly after binding JSON.
	container.Location = nil
	container.Parent = nil
	container.Project = nil

	if err := h.DB.Save(&container).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to update container"})
		return
	}

	h.LogActivity("UPDATE", "Container", container.ID, "Updated container: "+container.Name)
	c.JSON(http.StatusOK, container)
}

// @Summary Delete Container
// @Description Delete a container by ID
// @Tags Containers
// @Produce json
// @Param id path string true "Container ID"
// @Success 204
// @Failure 400 {object} map[string]string
// @Failure 404 {object} map[string]string
// @Router /containers/{id} [delete]
func (h *Handler) DeleteContainer(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	if err := h.DB.Delete(&models.Container{}, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to delete container"})
		return
	}

	h.LogActivity("DELETE", "Container", id, "Deleted container")
	c.JSON(http.StatusNoContent, nil)
}
