package handlers

import (
	"fmt"
	"net/http"
	"strconv"
	"time"

	"invelog/pkg/dto"
	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
)

// ==========================================
// Core Project Handlers
// ==========================================

func (h *Handler) CreateProject(c *gin.Context) {
	var req dto.CreateProjectRequest
	if err := c.ShouldBindJSON(&req); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	if req.OwnerID != nil {
		var user models.User
		if err := h.DB.First(&user, "id = ?", req.OwnerID).Error; err != nil {
			c.JSON(http.StatusBadRequest, gin.H{"error": "Referenced owner_id user does not exist"})
			return
		}
	} else if val, exists := c.Get("userID"); exists {
		if uID, ok := val.(uuid.UUID); ok {
			req.OwnerID = &uID
		}
	}

	phase := "Concept"
	if req.Phase != "" {
		phase = req.Phase
	}

	status := "Active"
	if req.Status != "" {
		status = req.Status
	}

	trl := 1
	if req.TRL != nil {
		trl = *req.TRL
	}

	budget := float64(0)
	if req.Budget != nil {
		budget = *req.Budget
	}

	project := models.Project{
		Name:          req.Name,
		Description:   req.Description,
		Phase:         phase,
		Status:        status,
		TRL:           trl,
		Budget:        budget,
		StartDate:     req.StartDate,
		TargetEndDate: req.TargetEndDate,
		OwnerID:       req.OwnerID,
	}

	if err := h.DB.Create(&project).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to create project"})
		return
	}

	// Add owner as ProjectLead member
	if req.OwnerID != nil {
		member := models.ProjectMember{
			ProjectID:   project.ID,
			UserID:      *req.OwnerID,
			ProjectRole: "ProjectLead",
		}
		h.DB.Create(&member)
	}

	h.LogActivityWithContext(c, "CREATE", "Project", project.ID, "Created project: "+project.Name)
	c.JSON(http.StatusCreated, project)
}

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

	query := h.DB.Model(&models.Project{})

	if phase := c.Query("phase"); phase != "" {
		query = query.Where("phase = ?", phase)
	}
	if status := c.Query("status"); status != "" {
		query = query.Where("status = ?", status)
	}
	if ownerID := c.Query("owner_id"); ownerID != "" {
		if u, err := uuid.Parse(ownerID); err == nil {
			query = query.Where("owner_id = ?", u)
		}
	}

	var projects []models.Project
	var total int64

	query.Count(&total)
	query.Preload("Owner").Order("created_at desc").Limit(limit).Offset(offset).Find(&projects)

	c.Header("X-Total-Count", fmt.Sprintf("%d", total))
	c.Header("X-Limit", fmt.Sprintf("%d", limit))
	c.Header("X-Offset", fmt.Sprintf("%d", offset))

	c.JSON(http.StatusOK, projects)
}

func (h *Handler) GetProject(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	var project models.Project
	if err := h.DB.Preload("Owner").First(&project, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Project not found"})
		return
	}

	c.JSON(http.StatusOK, project)
}

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

	var input dto.UpdateProjectInput
	if err := c.ShouldBindJSON(&input); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	if input.Name != nil {
		project.Name = *input.Name
	}
	if input.Description != nil {
		project.Description = *input.Description
	}
	if input.Phase != nil {
		project.Phase = *input.Phase
	}
	if input.Status != nil {
		project.Status = *input.Status
	}
	if input.TRL != nil {
		project.TRL = *input.TRL
	}
	if input.Budget != nil {
		project.Budget = *input.Budget
	}
	if input.AllocatedCost != nil {
		project.AllocatedCost = *input.AllocatedCost
	}
	if input.StartDate != nil {
		project.StartDate = input.StartDate
	}
	if input.TargetEndDate != nil {
		project.TargetEndDate = input.TargetEndDate
	}
	if input.OwnerID != nil {
		var user models.User
		if err := h.DB.First(&user, "id = ?", input.OwnerID).Error; err != nil {
			c.JSON(http.StatusBadRequest, gin.H{"error": "Referenced owner_id user does not exist"})
			return
		}
		project.OwnerID = input.OwnerID
	}

	if err := h.DB.Save(&project).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to update project"})
		return
	}

	h.LogActivityWithContext(c, "UPDATE", "Project", project.ID, "Updated project: "+project.Name)
	c.JSON(http.StatusOK, project)
}

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

	h.LogActivityWithContext(c, "DELETE", "Project", id, "Deleted project")
	c.JSON(http.StatusNoContent, nil)
}

// ==========================================
// Project Team Member Handlers
// ==========================================

func (h *Handler) AddProjectMember(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Project ID"})
		return
	}

	var req dto.AddProjectMemberRequest
	if err := c.ShouldBindJSON(&req); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	var project models.Project
	if err := h.DB.First(&project, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Project not found"})
		return
	}

	var user models.User
	if err := h.DB.First(&user, "id = ?", req.UserID).Error; err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Referenced user_id does not exist"})
		return
	}

	role := "Contributor"
	if req.ProjectRole != "" {
		role = req.ProjectRole
	}

	member := models.ProjectMember{
		ProjectID:   id,
		UserID:      req.UserID,
		ProjectRole: role,
	}

	if err := h.DB.Create(&member).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to add team member"})
		return
	}

	h.LogActivityWithContext(c, "ADD_MEMBER", "Project", id, "Added member to project: "+user.Username)
	c.JSON(http.StatusCreated, member)
}

func (h *Handler) ListProjectMembers(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Project ID"})
		return
	}

	var members []models.ProjectMember
	if err := h.DB.Preload("User").Where("project_id = ?", id).Find(&members).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to list team members"})
		return
	}

	c.JSON(http.StatusOK, members)
}

func (h *Handler) RemoveProjectMember(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Project ID"})
		return
	}

	userID, err := uuid.Parse(c.Param("user_id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid User ID"})
		return
	}

	if err := h.DB.Where("project_id = ? AND user_id = ?", id, userID).Delete(&models.ProjectMember{}).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to remove member"})
		return
	}

	h.LogActivityWithContext(c, "REMOVE_MEMBER", "Project", id, "Removed member from project")
	c.JSON(http.StatusNoContent, nil)
}

// ==========================================
// Task Kanban & Timeline Handlers
// ==========================================

func (h *Handler) CreateTask(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Project ID"})
		return
	}

	var req dto.CreateTaskRequest
	if err := c.ShouldBindJSON(&req); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	var project models.Project
	if err := h.DB.First(&project, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Project not found"})
		return
	}

	if req.AssigneeID != nil {
		var user models.User
		if err := h.DB.First(&user, "id = ?", req.AssigneeID).Error; err != nil {
			c.JSON(http.StatusBadRequest, gin.H{"error": "Referenced assignee_id user does not exist"})
			return
		}
	}

	reporterID := req.ReporterID
	if reporterID == nil {
		if val, exists := c.Get("userID"); exists {
			if uID, ok := val.(uuid.UUID); ok {
				reporterID = &uID
			}
		}
	}

	status := "Todo"
	if req.Status != "" {
		status = req.Status
	}

	priority := "Medium"
	if req.Priority != "" {
		priority = req.Priority
	}

	estHours := float64(0)
	if req.EstimatedHours != nil {
		estHours = *req.EstimatedHours
	}

	task := models.ProjectTask{
		ProjectID:        id,
		Title:            req.Title,
		Description:      req.Description,
		Status:           status,
		Priority:         priority,
		AssigneeID:       req.AssigneeID,
		ReporterID:       reporterID,
		StartDate:        req.StartDate,
		DueDate:          req.DueDate,
		EstimatedHours:   estHours,
		ParentTaskID:     req.ParentTaskID,
		DependencyTaskID: req.DependencyTaskID,
		WBSNodeID:        req.WBSNodeID,
		RequirementID:    req.RequirementID,
		RiskItemID:       req.RiskItemID,
	}

	if err := h.DB.Create(&task).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to create task"})
		return
	}

	h.LogActivityWithContext(c, "CREATE_TASK", "ProjectTask", task.ID, "Created task: "+task.Title)
	c.JSON(http.StatusCreated, task)
}

func (h *Handler) ListTasks(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Project ID"})
		return
	}

	query := h.DB.Model(&models.ProjectTask{}).Where("project_id = ?", id)

	if status := c.Query("status"); status != "" {
		query = query.Where("status = ?", status)
	}
	if priority := c.Query("priority"); priority != "" {
		query = query.Where("priority = ?", priority)
	}
	if assigneeID := c.Query("assignee_id"); assigneeID != "" {
		if u, err := uuid.Parse(assigneeID); err == nil {
			query = query.Where("assignee_id = ?", u)
		}
	}
	if wbsID := c.Query("wbs_node_id"); wbsID != "" {
		if u, err := uuid.Parse(wbsID); err == nil {
			query = query.Where("wbs_node_id = ?", u)
		}
	}

	var tasks []models.ProjectTask
	if err := query.Preload("Assignee").Preload("Reporter").Order("created_at desc").Find(&tasks).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to list tasks"})
		return
	}

	c.JSON(http.StatusOK, tasks)
}

func (h *Handler) GetMyTasks(c *gin.Context) {
	val, exists := c.Get("userID")
	if !exists {
		c.JSON(http.StatusUnauthorized, gin.H{"error": "Authentication required"})
		return
	}
	userID, ok := val.(uuid.UUID)
	if !ok {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid User ID"})
		return
	}

	var tasks []models.ProjectTask
	if err := h.DB.Preload("Assignee").Preload("Reporter").Where("assignee_id = ?", userID).Order("created_at desc").Find(&tasks).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to fetch user tasks"})
		return
	}

	c.JSON(http.StatusOK, tasks)
}

func (h *Handler) UpdateTask(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Project ID"})
		return
	}

	taskID, err := uuid.Parse(c.Param("task_id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Task ID"})
		return
	}

	var task models.ProjectTask
	if err := h.DB.First(&task, "id = ? AND project_id = ?", taskID, id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Task not found"})
		return
	}

	var input dto.UpdateTaskInput
	if err := c.ShouldBindJSON(&input); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	if input.Title != nil {
		task.Title = *input.Title
	}
	if input.Description != nil {
		task.Description = *input.Description
	}
	if input.Status != nil {
		task.Status = *input.Status
	}
	if input.Priority != nil {
		task.Priority = *input.Priority
	}
	if input.AssigneeID != nil {
		var user models.User
		if err := h.DB.First(&user, "id = ?", input.AssigneeID).Error; err != nil {
			c.JSON(http.StatusBadRequest, gin.H{"error": "Referenced assignee_id user does not exist"})
			return
		}
		task.AssigneeID = input.AssigneeID
	}
	if input.ReporterID != nil {
		task.ReporterID = input.ReporterID
	}
	if input.StartDate != nil {
		task.StartDate = input.StartDate
	}
	if input.DueDate != nil {
		task.DueDate = input.DueDate
	}
	if input.EstimatedHours != nil {
		task.EstimatedHours = *input.EstimatedHours
	}
	if input.ActualHours != nil {
		task.ActualHours = *input.ActualHours
	}
	if input.ParentTaskID != nil {
		task.ParentTaskID = input.ParentTaskID
	}
	if input.DependencyTaskID != nil {
		task.DependencyTaskID = input.DependencyTaskID
	}
	if input.WBSNodeID != nil {
		task.WBSNodeID = input.WBSNodeID
	}
	if input.RequirementID != nil {
		task.RequirementID = input.RequirementID
	}
	if input.RiskItemID != nil {
		task.RiskItemID = input.RiskItemID
	}

	if err := h.DB.Save(&task).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to update task"})
		return
	}

	h.LogActivityWithContext(c, "UPDATE_TASK", "ProjectTask", task.ID, "Updated task: "+task.Title)
	c.JSON(http.StatusOK, task)
}

func (h *Handler) DeleteTask(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Project ID"})
		return
	}

	taskID, err := uuid.Parse(c.Param("task_id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Task ID"})
		return
	}

	if err := h.DB.Where("id = ? AND project_id = ?", taskID, id).Delete(&models.ProjectTask{}).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to delete task"})
		return
	}

	h.LogActivityWithContext(c, "DELETE_TASK", "ProjectTask", taskID, "Deleted task")
	c.JSON(http.StatusNoContent, nil)
}

func (h *Handler) ReserveTaskResource(c *gin.Context) {
	taskID, err := uuid.Parse(c.Param("task_id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Task ID"})
		return
	}

	var req dto.AllocateTaskResourceRequest
	if err := c.ShouldBindJSON(&req); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	if req.ItemID == nil && req.ContainerID == nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Must specify item_id or container_id"})
		return
	}

	if req.ItemID != nil {
		var item models.Item
		if err := h.DB.First(&item, "id = ?", req.ItemID).Error; err != nil {
			c.JSON(http.StatusBadRequest, gin.H{"error": "Referenced item_id does not exist"})
			return
		}
	}

	if req.ContainerID != nil {
		var container models.Container
		if err := h.DB.First(&container, "id = ?", req.ContainerID).Error; err != nil {
			c.JSON(http.StatusBadRequest, gin.H{"error": "Referenced container_id does not exist"})
			return
		}
	}

	qty := 1
	if req.QuantityReserved != nil {
		if *req.QuantityReserved <= 0 {
			c.JSON(http.StatusBadRequest, gin.H{"error": "quantity_reserved must be greater than 0"})
			return
		}
		qty = *req.QuantityReserved
	}

	alloc := models.TaskResourceAllocation{
		TaskID:           taskID,
		ItemID:           req.ItemID,
		ContainerID:      req.ContainerID,
		QuantityReserved: qty,
		Notes:            req.Notes,
	}

	if err := h.DB.Create(&alloc).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to reserve resource for task"})
		return
	}

	h.LogActivityWithContext(c, "RESERVE_RESOURCE", "ProjectTask", taskID, "Reserved resource for task")
	c.JSON(http.StatusCreated, alloc)
}

// ==========================================
// Project Milestone Handlers
// ==========================================

func (h *Handler) CreateMilestone(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Project ID"})
		return
	}

	var req dto.CreateMilestoneRequest
	if err := c.ShouldBindJSON(&req); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	status := "Pending"
	if req.Status != "" {
		status = req.Status
	}

	gate := "General"
	if req.GateReviewType != "" {
		gate = req.GateReviewType
	}

	milestone := models.ProjectMilestone{
		ProjectID:      id,
		Name:           req.Name,
		Description:    req.Description,
		TargetDate:     req.TargetDate,
		Status:         status,
		GateReviewType: gate,
	}

	if err := h.DB.Create(&milestone).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to create milestone"})
		return
	}

	h.LogActivityWithContext(c, "CREATE_MILESTONE", "ProjectMilestone", milestone.ID, "Created milestone: "+milestone.Name)
	c.JSON(http.StatusCreated, milestone)
}

func (h *Handler) ListMilestones(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Project ID"})
		return
	}

	var milestones []models.ProjectMilestone
	if err := h.DB.Where("project_id = ?", id).Order("created_at desc").Find(&milestones).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to list milestones"})
		return
	}

	c.JSON(http.StatusOK, milestones)
}

// ==========================================
// Requirement Handlers
// ==========================================

func (h *Handler) CreateRequirement(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Project ID"})
		return
	}

	var req dto.CreateRequirementRequest
	if err := c.ShouldBindJSON(&req); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	cat := "Functional"
	if req.Category != "" {
		cat = req.Category
	}

	method := "Test"
	if req.VerificationMethod != "" {
		method = req.VerificationMethod
	}

	status := "Draft"
	if req.Status != "" {
		status = req.Status
	}

	requirement := models.Requirement{
		ProjectID:          id,
		Code:               req.Code,
		Title:              req.Title,
		Description:        req.Description,
		Category:           cat,
		VerificationMethod: method,
		Status:             status,
		OwnerID:            req.OwnerID,
	}

	if err := h.DB.Create(&requirement).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to create requirement"})
		return
	}

	h.LogActivityWithContext(c, "CREATE_REQ", "Requirement", requirement.ID, "Created requirement: "+requirement.Code)
	c.JSON(http.StatusCreated, requirement)
}

func (h *Handler) ListRequirements(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Project ID"})
		return
	}

	var reqs []models.Requirement
	if err := h.DB.Preload("Owner").Where("project_id = ?", id).Find(&reqs).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to list requirements"})
		return
	}

	c.JSON(http.StatusOK, reqs)
}

// ==========================================
// WBS Node Handlers
// ==========================================

func (h *Handler) CreateWBSNode(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Project ID"})
		return
	}

	var req dto.CreateWBSNodeRequest
	if err := c.ShouldBindJSON(&req); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	progress := 0
	if req.ProgressPct != nil {
		progress = *req.ProgressPct
	}

	status := "Planned"
	if req.Status != "" {
		status = req.Status
	}

	node := models.WBSNode{
		ProjectID:   id,
		Code:        req.Code,
		Name:        req.Name,
		Description: req.Description,
		ParentID:    req.ParentID,
		OwnerID:     req.OwnerID,
		Status:      status,
		ProgressPct: progress,
	}

	if err := h.DB.Create(&node).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to create WBS node"})
		return
	}

	h.LogActivityWithContext(c, "CREATE_WBS", "WBSNode", node.ID, "Created WBS node: "+node.Code)
	c.JSON(http.StatusCreated, node)
}

func (h *Handler) ListWBSNodes(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Project ID"})
		return
	}

	var nodes []models.WBSNode
	if err := h.DB.Preload("Owner").Where("project_id = ?", id).Find(&nodes).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to list WBS nodes"})
		return
	}

	c.JSON(http.StatusOK, nodes)
}

// ==========================================
// Risk Register / FMEA Handlers
// ==========================================

func (h *Handler) CreateRiskItem(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Project ID"})
		return
	}

	var req dto.CreateRiskItemRequest
	if err := c.ShouldBindJSON(&req); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	sev := 1
	if req.Severity != nil && *req.Severity >= 1 && *req.Severity <= 5 {
		sev = *req.Severity
	}

	lik := 1
	if req.Likelihood != nil && *req.Likelihood >= 1 && *req.Likelihood <= 5 {
		lik = *req.Likelihood
	}

	rpn := sev * lik

	status := "Open"
	if req.Status != "" {
		status = req.Status
	}

	cat := "Technical"
	if req.Category != "" {
		cat = req.Category
	}

	risk := models.RiskItem{
		ProjectID:          id,
		Title:              req.Title,
		Description:        req.Description,
		Category:           cat,
		Severity:           sev,
		Likelihood:         lik,
		RPN:                rpn,
		MitigationStrategy: req.MitigationStrategy,
		Status:             status,
	}

	if err := h.DB.Create(&risk).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to create risk item"})
		return
	}

	h.LogActivityWithContext(c, "CREATE_RISK", "RiskItem", risk.ID, "Created risk item: "+risk.Title)
	c.JSON(http.StatusCreated, risk)
}

func (h *Handler) ListRiskItems(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Project ID"})
		return
	}

	var risks []models.RiskItem
	if err := h.DB.Where("project_id = ?", id).Order("rpn desc").Find(&risks).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to list risk items"})
		return
	}

	c.JSON(http.StatusOK, risks)
}

// ==========================================
// PM & SEPM Executive Dashboard Handler
// ==========================================

func (h *Handler) GetPMDashboard(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Project ID"})
		return
	}

	var project models.Project
	if err := h.DB.First(&project, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Project not found"})
		return
	}

	var membersCount int64
	h.DB.Model(&models.ProjectMember{}).Where("project_id = ?", id).Count(&membersCount)

	var totalTasks int64
	h.DB.Model(&models.ProjectTask{}).Where("project_id = ?", id).Count(&totalTasks)

	taskStatusCounts := make(map[string]int64)
	statuses := []string{"Backlog", "Todo", "InProgress", "InReview", "Done"}
	for _, st := range statuses {
		var cnt int64
		h.DB.Model(&models.ProjectTask{}).Where("project_id = ? AND status = ?", id, st).Count(&cnt)
		taskStatusCounts[st] = cnt
	}

	taskPriorityCounts := make(map[string]int64)
	priorities := []string{"Low", "Medium", "High", "Critical"}
	for _, pr := range priorities {
		var cnt int64
		h.DB.Model(&models.ProjectTask{}).Where("project_id = ? AND priority = ?", id, pr).Count(&cnt)
		taskPriorityCounts[pr] = cnt
	}

	var overdueCount int64
	now := time.Now()
	h.DB.Model(&models.ProjectTask{}).Where("project_id = ? AND due_date < ? AND status != ?", id, now, "Done").Count(&overdueCount)

	var totalMilestones int64
	h.DB.Model(&models.ProjectMilestone{}).Where("project_id = ?", id).Count(&totalMilestones)

	var achievedMilestones int64
	h.DB.Model(&models.ProjectMilestone{}).Where("project_id = ? AND status = ?", id, "Achieved").Count(&achievedMilestones)

	var totalReqs int64
	h.DB.Model(&models.Requirement{}).Where("project_id = ?", id).Count(&totalReqs)

	var verifiedReqs int64
	h.DB.Model(&models.Requirement{}).Where("project_id = ? AND status = ?", id, "Verified").Count(&verifiedReqs)

	reqVerifyPct := float64(0)
	if totalReqs > 0 {
		reqVerifyPct = (float64(verifiedReqs) / float64(totalReqs)) * 100.0
	}

	var totalWBS int64
	h.DB.Model(&models.WBSNode{}).Where("project_id = ?", id).Count(&totalWBS)

	var wbsAvgProgress float64
	h.DB.Model(&models.WBSNode{}).Where("project_id = ?", id).Select("COALESCE(AVG(progress_pct), 0)").Scan(&wbsAvgProgress)

	var totalRisks int64
	h.DB.Model(&models.RiskItem{}).Where("project_id = ?", id).Count(&totalRisks)

	var maxRPN int
	h.DB.Model(&models.RiskItem{}).Where("project_id = ?", id).Select("COALESCE(MAX(rpn), 0)").Scan(&maxRPN)

	var totalResAlloc int64
	h.DB.Table("task_resource_allocations").Joins("JOIN project_tasks ON project_tasks.id = task_resource_allocations.task_id").Where("project_tasks.project_id = ?", id).Count(&totalResAlloc)

	resp := dto.PMDashboardResponse{
		ProjectID:              project.ID,
		ProjectName:            project.Name,
		Phase:                  project.Phase,
		Status:                 project.Status,
		TRL:                    project.TRL,
		Budget:                 project.Budget,
		AllocatedCost:          project.AllocatedCost,
		TeamMembersCount:       membersCount,
		TotalTasks:             totalTasks,
		TaskStatusCounts:       taskStatusCounts,
		TaskPriorityCounts:     taskPriorityCounts,
		OverdueTasksCount:      overdueCount,
		MilestonesCount:        totalMilestones,
		AchievedMilestones:     achievedMilestones,
		TotalRequirements:      totalReqs,
		VerifiedRequirements:   verifiedReqs,
		RequirementsVerifyPct:  reqVerifyPct,
		WBSNodesCount:          totalWBS,
		WBSAvgProgressPct:      wbsAvgProgress,
		TotalRisks:             totalRisks,
		MaxRiskRPN:             maxRPN,
		TotalResourceAllocated: totalResAlloc,
	}

	c.JSON(http.StatusOK, resp)
}
