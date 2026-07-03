package handlers

import (
	"fmt"
	"net/http"
	"strconv"

	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
)

// @Summary Create Project
// @Description Create a new project
// @Tags Projects
// @Accept json
// @Produce json
// @Param project body models.Project true "Project Data"
// @Success 201 {object} models.Project
// @Failure 400 {object} map[string]string
// @Router /projects [post]
func (h *Handler) CreateProject(c *gin.Context) {
	var project models.Project
	if err := c.ShouldBindJSON(&project); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	if err := h.DB.Create(&project).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to create project"})
		return
	}

	h.LogActivity("CREATE", "Project", project.ID, "Created project: "+project.Name)
	c.JSON(http.StatusCreated, project)
}

// @Summary List Projects
// @Description Get all projects
// @Tags Projects
// @Produce json
// @Param limit query int false "Limit (default 1000, max 10000)"
// @Param offset query int false "Offset (default 0)"
// @Success 200 {array} models.Project
// @Router /projects [get]
func (h *Handler) ListProjects(c *gin.Context) {
	limitStr := c.DefaultQuery("limit", "1000")
	offsetStr := c.DefaultQuery("offset", "0")

	limit, err := strconv.Atoi(limitStr)
	if err != nil || limit <= 0 {
		limit = 1000
	}
	if limit > 10000 {
		limit = 10000
	}

	offset, err := strconv.Atoi(offsetStr)
	if err != nil || offset < 0 {
		offset = 0
	}

	var projects []models.Project
	var total int64

	if err := h.DB.Model(&models.Project{}).Count(&total).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to count projects"})
		return
	}

	if err := h.DB.Order("created_at desc").Limit(limit).Offset(offset).Find(&projects).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to list projects"})
		return
	}

	c.Header("X-Total-Count", fmt.Sprintf("%d", total))
	c.Header("X-Limit", fmt.Sprintf("%d", limit))
	c.Header("X-Offset", fmt.Sprintf("%d", offset))

	c.JSON(http.StatusOK, projects)
}

// @Summary Get Project
// @Description Get a project by ID
// @Tags Projects
// @Produce json
// @Param id path string true "Project ID"
// @Success 200 {object} models.Project
// @Failure 404 {object} map[string]string
// @Router /projects/{id} [get]
func (h *Handler) GetProject(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	var project models.Project
	if err := h.DB.First(&project, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Project not found"})
		return
	}

	c.JSON(http.StatusOK, project)
}

// @Summary Update Project
// @Description Update a project by ID
// @Tags Projects
// @Accept json
// @Produce json
// @Param id path string true "Project ID"
// @Param project body models.Project true "Project Data"
// @Success 200 {object} models.Project
// @Failure 400 {object} map[string]string
// @Failure 404 {object} map[string]string
// @Router /projects/{id} [put]
func (h *Handler) UpdateProject(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	var project models.Project
	if err := h.DB.First(&project, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Project not found"})
		return
	}

	if err := c.ShouldBindJSON(&project); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}
	project.ID = id // Ensure ID cannot be changed

	if err := h.DB.Save(&project).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to update project"})
		return
	}

	h.LogActivity("UPDATE", "Project", project.ID, "Updated project: "+project.Name)
	c.JSON(http.StatusOK, project)
}

// @Summary Delete Project
// @Description Delete a project by ID
// @Tags Projects
// @Produce json
// @Param id path string true "Project ID"
// @Success 204
// @Failure 400 {object} map[string]string
// @Failure 404 {object} map[string]string
// @Router /projects/{id} [delete]
func (h *Handler) DeleteProject(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	if err := h.DB.Delete(&models.Project{}, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to delete project"})
		return
	}

	h.LogActivity("DELETE", "Project", id, "Deleted project")
	c.JSON(http.StatusNoContent, nil)
}
