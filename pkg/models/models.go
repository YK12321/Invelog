package models

import (
	"time"

	"github.com/google/uuid"
	"gorm.io/datatypes"
	"gorm.io/gorm"
)

type Base struct {
	ID        uuid.UUID      `gorm:"type:uuid;primary_key;" json:"id"`
	CreatedAt time.Time      `json:"created_at"`
	UpdatedAt time.Time      `json:"updated_at"`
	DeletedAt gorm.DeletedAt `gorm:"index" json:"-"`
}

func (base *Base) BeforeCreate(tx *gorm.DB) error {
	if base.ID == uuid.Nil {
		base.ID = uuid.New()
	}
	return nil
}

type User struct {
	Base
	Username     string `gorm:"uniqueIndex;not null" json:"username"`
	Email        string `gorm:"uniqueIndex;not null" json:"email"`
	PasswordHash string `gorm:"not null" json:"-"`
	Role         string `gorm:"not null;default:'user'" json:"role"` // e.g., "admin", "user"
}

type Category struct {
	Base
	Name        string `gorm:"uniqueIndex;not null" json:"name"`
	Description string `json:"description"`
}

type Location struct {
	Base
	Name        string `gorm:"uniqueIndex;not null" json:"name"`
	Description string `json:"description"`
}

type Project struct {
	Base
	Name          string     `gorm:"uniqueIndex;not null" json:"name"`
	Description   string     `json:"description"`
	Phase         string     `gorm:"default:'Concept'" json:"phase"` // e.g. Concept, Definition, Design, Integration, Operations, Closeout
	Status        string     `gorm:"default:'Active'" json:"status"` // e.g. Active, Planning, InReview, Completed, Archived
	TRL           int        `gorm:"default:1" json:"trl"`           // Target Technology Readiness Level (1-9)
	Budget        float64    `gorm:"default:0" json:"budget"`
	AllocatedCost float64    `gorm:"default:0" json:"allocated_cost"`
	StartDate     *time.Time `json:"start_date,omitempty"`
	TargetEndDate *time.Time `json:"target_end_date,omitempty"`
	OwnerID       *uuid.UUID `gorm:"type:uuid;index" json:"owner_id"`
	Owner         *User      `json:"owner,omitempty"`
}

type ProjectMember struct {
	Base
	ProjectID   uuid.UUID `gorm:"type:uuid;index;not null" json:"project_id"`
	UserID      uuid.UUID `gorm:"type:uuid;index;not null" json:"user_id"`
	User        *User     `json:"user,omitempty"`
	ProjectRole string    `gorm:"default:'Contributor'" json:"project_role"` // e.g. ProjectLead, SystemsEngineer, Contributor, Viewer, Auditor
}

type Requirement struct {
	Base
	ProjectID          uuid.UUID  `gorm:"type:uuid;index;not null" json:"project_id"`
	Code               string     `gorm:"not null" json:"code"` // e.g., REQ-01
	Title              string     `gorm:"not null" json:"title"`
	Description        string     `json:"description"`
	Category           string     `gorm:"default:'Functional'" json:"category"` // Functional, Performance, Interface, Constraint, Security
	VerificationMethod string     `gorm:"default:'Test'" json:"verification_method"` // Test, Demonstration, Analysis, Inspection
	Status             string     `gorm:"default:'Draft'" json:"status"` // Draft, Approved, Verified
	OwnerID            *uuid.UUID `gorm:"type:uuid;index" json:"owner_id"`
	Owner              *User      `json:"owner,omitempty"`
}

type WBSNode struct {
	Base
	ProjectID   uuid.UUID  `gorm:"type:uuid;index;not null" json:"project_id"`
	Code        string     `gorm:"not null" json:"code"` // e.g., 1.1.2
	Name        string     `gorm:"not null" json:"name"`
	Description string     `json:"description"`
	ParentID    *uuid.UUID `gorm:"type:uuid;index" json:"parent_id"`
	OwnerID     *uuid.UUID `gorm:"type:uuid;index" json:"owner_id"`
	Owner       *User      `json:"owner,omitempty"`
	Status      string     `gorm:"default:'Planned'" json:"status"` // Planned, InProgress, Completed
	ProgressPct int        `gorm:"default:0" json:"progress_pct"`
}

type RiskItem struct {
	Base
	ProjectID          uuid.UUID `gorm:"type:uuid;index;not null" json:"project_id"`
	Title              string    `gorm:"not null" json:"title"`
	Description        string    `json:"description"`
	Category           string    `gorm:"default:'Technical'" json:"category"` // Technical, Cost, Schedule, Safety
	Severity           int       `gorm:"default:1" json:"severity"`          // 1-5
	Likelihood         int       `gorm:"default:1" json:"likelihood"`        // 1-5
	RPN                int       `gorm:"default:1" json:"rpn"`               // Severity * Likelihood
	MitigationStrategy string    `json:"mitigation_strategy"`
	Status             string    `gorm:"default:'Open'" json:"status"` // Open, Mitigated, Accepted, Closed
}

type ProjectMilestone struct {
	Base
	ProjectID      uuid.UUID  `gorm:"type:uuid;index;not null" json:"project_id"`
	Name           string     `gorm:"not null" json:"name"`
	Description    string     `json:"description"`
	TargetDate     *time.Time `json:"target_date,omitempty"`
	Status         string     `gorm:"default:'Pending'" json:"status"` // Pending, Achieved, Delayed
	GateReviewType string     `gorm:"default:'General'" json:"gate_review_type"` // SRR, PDR, CDR, TRR, ORR, General
}

type ProjectTask struct {
	Base
	ProjectID        uuid.UUID  `gorm:"type:uuid;index;not null" json:"project_id"`
	Title            string     `gorm:"not null" json:"title"`
	Description      string     `json:"description"`
	Status           string     `gorm:"default:'Todo'" json:"status"` // Backlog, Todo, InProgress, InReview, Done
	Priority         string     `gorm:"default:'Medium'" json:"priority"` // Low, Medium, High, Critical
	AssigneeID       *uuid.UUID `gorm:"type:uuid;index" json:"assignee_id"`
	Assignee         *User      `json:"assignee,omitempty"`
	ReporterID       *uuid.UUID `gorm:"type:uuid;index" json:"reporter_id"`
	Reporter         *User      `json:"reporter,omitempty"`
	StartDate        *time.Time `json:"start_date,omitempty"`
	DueDate          *time.Time `json:"due_date,omitempty"`
	EstimatedHours   float64    `gorm:"default:0" json:"estimated_hours"`
	ActualHours      float64    `gorm:"default:0" json:"actual_hours"`
	ParentTaskID     *uuid.UUID `gorm:"type:uuid;index" json:"parent_task_id"`
	DependencyTaskID *uuid.UUID `gorm:"type:uuid;index" json:"dependency_task_id"`
	WBSNodeID        *uuid.UUID `gorm:"type:uuid;index" json:"wbs_node_id"`
	RequirementID    *uuid.UUID `gorm:"type:uuid;index" json:"requirement_id"`
	RiskItemID       *uuid.UUID `gorm:"type:uuid;index" json:"risk_item_id"`
}

type TaskResourceAllocation struct {
	Base
	TaskID           uuid.UUID  `gorm:"type:uuid;index;not null" json:"task_id"`
	ItemID           *uuid.UUID `gorm:"type:uuid;index" json:"item_id"`
	Item             *Item      `json:"item,omitempty"`
	ContainerID      *uuid.UUID `gorm:"type:uuid;index" json:"container_id"`
	Container        *Container `json:"container,omitempty"`
	QuantityReserved int        `gorm:"default:1" json:"quantity_reserved"`
	Notes            string     `json:"notes"`
}

type Container struct {
	Base
	Name        string     `gorm:"not null" json:"name"`
	Description string     `json:"description"`
	Barcode     string     `gorm:"uniqueIndex" json:"barcode"`
	LocationID  *uuid.UUID `gorm:"type:uuid;index" json:"location_id"`
	Location    *Location  `json:"location,omitempty"`
	ParentID    *uuid.UUID `gorm:"type:uuid;index" json:"parent_id"`
	Parent      *Container `json:"parent,omitempty"`
	ProjectID   *uuid.UUID `gorm:"type:uuid;index" json:"project_id"`
	Project     *Project   `json:"project,omitempty"`
}

func (c *Container) BeforeCreate(tx *gorm.DB) error {
	if err := c.Base.BeforeCreate(tx); err != nil {
		return err
	}
	if c.Barcode == "" {
		c.Barcode = "BC-CNT-" + c.ID.String()[:8]
	}
	return nil
}

type ItemType struct {
	Base
	Name            string         `gorm:"not null" json:"name"`
	Description     string         `json:"description"`
	Specifications  string         `json:"specifications"`
	Parameters      datatypes.JSON `gorm:"type:json" json:"parameters,omitempty"`
	Manufacturer    string         `json:"manufacturer"`
	PartNumber      string         `json:"part_number"`
	MinQuantity     int            `gorm:"default:0" json:"min_quantity"`
	ReorderQuantity int            `gorm:"default:0" json:"reorder_quantity"`
	CategoryID      *uuid.UUID     `gorm:"type:uuid;index" json:"category_id"`
	Category        *Category      `json:"category,omitempty"`
}

type Item struct {
	Base
	Name             string     `json:"name"` // Used if ItemType is not provided
	Description      string     `json:"description"`
	Quantity         int        `gorm:"not null;default:1" json:"quantity"`
	MinQuantity      int        `gorm:"default:0" json:"min_quantity"`
	ReorderQuantity  int        `gorm:"default:0" json:"reorder_quantity"`
	SKU              string     `gorm:"uniqueIndex" json:"sku"`
	Barcode          string     `gorm:"uniqueIndex" json:"barcode"`
	IndividualNotes  string     `json:"individual_notes"`
	SerialNumber     string     `json:"serial_number"`
	CheckedOut       bool       `gorm:"default:false" json:"checked_out"`
	LastCheckOutTime *time.Time `json:"last_check_out_time"`
	ItemTypeID       *uuid.UUID `gorm:"type:uuid;index" json:"item_type_id"`
	ItemType         *ItemType  `json:"item_type,omitempty"`
	CategoryID       *uuid.UUID `gorm:"type:uuid;index" json:"category_id"`
	Category         *Category  `json:"category,omitempty"`
	ContainerID      *uuid.UUID `gorm:"type:uuid;index" json:"container_id"`
	Container        *Container `json:"container,omitempty"`
	OriginLocationID *uuid.UUID `gorm:"type:uuid;index" json:"origin_location_id"`
	OriginLocation   *Location  `json:"origin_location,omitempty"`
	CreatedBy        string     `gorm:"default:'system'" json:"created_by"`
}

func (item *Item) BeforeCreate(tx *gorm.DB) error {
	if err := item.Base.BeforeCreate(tx); err != nil {
		return err
	}
	if item.SKU == "" {
		item.SKU = "ITM-" + item.ID.String()[:8]
	}
	if item.Barcode == "" {
		item.Barcode = "BC-ITM-" + item.ID.String()[:8]
	}
	return nil
}

type ActivityLog struct {
	Base
	Action          string     `gorm:"not null" json:"action"` // e.g., "CREATED", "MODIFIED", "DELETED", "MOVED", "CHECK_IN", "CHECK_OUT", "QUANTITY_ADJUSTED", "ASSIGNED_TO_PROJECT", "RETURNED_FROM_PROJECT"
	EntityID        uuid.UUID  `gorm:"type:uuid;index;not null" json:"entity_id"`
	EntityType      string     `gorm:"not null" json:"entity_type"` // e.g., "Item", "Container", "Location"
	Details         string     `json:"details"`
	UserID          *uuid.UUID `gorm:"type:uuid;index" json:"user_id"` // Optional: Set when user performs action
	FromContainerID *uuid.UUID `gorm:"type:uuid;index" json:"from_container_id"`
	FromContainer   *Container `json:"from_container,omitempty"`
	ToContainerID   *uuid.UUID `gorm:"type:uuid;index" json:"to_container_id"`
	ToContainer     *Container `json:"to_container,omitempty"`
	ProjectID       *uuid.UUID `gorm:"type:uuid;index" json:"project_id"`
	Project         *Project   `json:"project,omitempty"`
	QuantityChange  int        `gorm:"default:0" json:"quantity_change"`
}

