package dto

import (
	"time"

	"github.com/google/uuid"
)

type CreateProjectRequest struct {
	Name          string     `json:"name" binding:"required"`
	Description   string     `json:"description"`
	Phase         string     `json:"phase"`
	Status        string     `json:"status"`
	TRL           *int       `json:"trl"`
	Budget        *float64   `json:"budget"`
	StartDate     *time.Time `json:"start_date"`
	TargetEndDate *time.Time `json:"target_end_date"`
	OwnerID       *uuid.UUID `json:"owner_id"`
}

type UpdateProjectInput struct {
	Name          *string    `json:"name"`
	Description   *string    `json:"description"`
	Phase         *string    `json:"phase"`
	Status        *string    `json:"status"`
	TRL           *int       `json:"trl"`
	Budget        *float64   `json:"budget"`
	AllocatedCost *float64   `json:"allocated_cost"`
	StartDate     *time.Time `json:"start_date"`
	TargetEndDate *time.Time `json:"target_end_date"`
	OwnerID       *uuid.UUID `json:"owner_id"`
}

type AddProjectMemberRequest struct {
	UserID      uuid.UUID `json:"user_id" binding:"required"`
	ProjectRole string    `json:"project_role"`
}

type CreateRequirementRequest struct {
	Code               string     `json:"code" binding:"required"`
	Title              string     `json:"title" binding:"required"`
	Description        string     `json:"description"`
	Category           string     `json:"category"`
	VerificationMethod string     `json:"verification_method"`
	Status             string     `json:"status"`
	OwnerID            *uuid.UUID `json:"owner_id"`
}

type UpdateRequirementInput struct {
	Code               *string    `json:"code"`
	Title              *string    `json:"title"`
	Description        *string    `json:"description"`
	Category           *string    `json:"category"`
	VerificationMethod *string    `json:"verification_method"`
	Status             *string    `json:"status"`
	OwnerID            *uuid.UUID `json:"owner_id"`
}

type CreateWBSNodeRequest struct {
	Code        string     `json:"code" binding:"required"`
	Name        string     `json:"name" binding:"required"`
	Description string     `json:"description"`
	ParentID    *uuid.UUID `json:"parent_id"`
	OwnerID     *uuid.UUID `json:"owner_id"`
	Status      string     `json:"status"`
	ProgressPct *int       `json:"progress_pct"`
}

type UpdateWBSNodeInput struct {
	Code        *string    `json:"code"`
	Name        *string    `json:"name"`
	Description *string    `json:"description"`
	ParentID    *uuid.UUID `json:"parent_id"`
	OwnerID     *uuid.UUID `json:"owner_id"`
	Status      *string    `json:"status"`
	ProgressPct *int       `json:"progress_pct"`
}

type CreateRiskItemRequest struct {
	Title              string `json:"title" binding:"required"`
	Description        string `json:"description"`
	Category           string `json:"category"`
	Severity           *int   `json:"severity"`
	Likelihood         *int   `json:"likelihood"`
	MitigationStrategy string `json:"mitigation_strategy"`
	Status             string `json:"status"`
}

type UpdateRiskItemInput struct {
	Title              *string `json:"title"`
	Description        *string `json:"description"`
	Category           *string `json:"category"`
	Severity           *int    `json:"severity"`
	Likelihood         *int    `json:"likelihood"`
	MitigationStrategy *string `json:"mitigation_strategy"`
	Status             *string `json:"status"`
}

type CreateMilestoneRequest struct {
	Name           string     `json:"name" binding:"required"`
	Description    string     `json:"description"`
	TargetDate     *time.Time `json:"target_date"`
	Status         string     `json:"status"`
	GateReviewType string     `json:"gate_review_type"`
}

type UpdateMilestoneInput struct {
	Name           *string    `json:"name"`
	Description    *string    `json:"description"`
	TargetDate     *time.Time `json:"target_date"`
	Status         *string    `json:"status"`
	GateReviewType *string    `json:"gate_review_type"`
}

type CreateTaskRequest struct {
	Title            string     `json:"title" binding:"required"`
	Description      string     `json:"description"`
	Status           string     `json:"status"`
	Priority         string     `json:"priority"`
	AssigneeID       *uuid.UUID `json:"assignee_id"`
	ReporterID       *uuid.UUID `json:"reporter_id"`
	StartDate        *time.Time `json:"start_date"`
	DueDate          *time.Time `json:"due_date"`
	EstimatedHours   *float64   `json:"estimated_hours"`
	ParentTaskID     *uuid.UUID `json:"parent_task_id"`
	DependencyTaskID *uuid.UUID `json:"dependency_task_id"`
	WBSNodeID        *uuid.UUID `json:"wbs_node_id"`
	RequirementID    *uuid.UUID `json:"requirement_id"`
	RiskItemID       *uuid.UUID `json:"risk_item_id"`
}

type UpdateTaskInput struct {
	Title            *string    `json:"title"`
	Description      *string    `json:"description"`
	Status           *string    `json:"status"`
	Priority         *string    `json:"priority"`
	AssigneeID       *uuid.UUID `json:"assignee_id"`
	ReporterID       *uuid.UUID `json:"reporter_id"`
	StartDate        *time.Time `json:"start_date"`
	DueDate          *time.Time `json:"due_date"`
	EstimatedHours   *float64   `json:"estimated_hours"`
	ActualHours      *float64   `json:"actual_hours"`
	ParentTaskID     *uuid.UUID `json:"parent_task_id"`
	DependencyTaskID *uuid.UUID `json:"dependency_task_id"`
	WBSNodeID        *uuid.UUID `json:"wbs_node_id"`
	RequirementID    *uuid.UUID `json:"requirement_id"`
	RiskItemID       *uuid.UUID `json:"risk_item_id"`
}

type AllocateTaskResourceRequest struct {
	ItemID           *uuid.UUID `json:"item_id"`
	ContainerID      *uuid.UUID `json:"container_id"`
	QuantityReserved *int       `json:"quantity_reserved"`
	Notes            string     `json:"notes"`
}

type PMDashboardResponse struct {
	ProjectID              uuid.UUID              `json:"project_id"`
	ProjectName            string                 `json:"project_name"`
	Phase                  string                 `json:"phase"`
	Status                 string                 `json:"status"`
	TRL                    int                    `json:"trl"`
	Budget                 float64                `json:"budget"`
	AllocatedCost          float64                `json:"allocated_cost"`
	TeamMembersCount       int64                  `json:"team_members_count"`
	TotalTasks             int64                  `json:"total_tasks"`
	TaskStatusCounts       map[string]int64       `json:"task_status_counts"`
	TaskPriorityCounts     map[string]int64       `json:"task_priority_counts"`
	OverdueTasksCount      int64                  `json:"overdue_tasks_count"`
	MilestonesCount        int64                  `json:"milestones_count"`
	AchievedMilestones     int64                  `json:"achieved_milestones"`
	TotalRequirements      int64                  `json:"total_requirements"`
	VerifiedRequirements   int64                  `json:"verified_requirements"`
	RequirementsVerifyPct  float64                `json:"requirements_verify_pct"`
	WBSNodesCount          int64                  `json:"wbs_nodes_count"`
	WBSAvgProgressPct      float64                `json:"wbs_avg_progress_pct"`
	TotalRisks             int64                  `json:"total_risks"`
	MaxRiskRPN             int                    `json:"max_risk_rpn"`
	TotalResourceAllocated int64                  `json:"total_resource_allocated"`
}
