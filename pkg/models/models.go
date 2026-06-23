package models

import (
	"time"

	"github.com/google/uuid"
	"gorm.io/gorm"
)

type Base struct {
	ID        uuid.UUID      `gorm:"type:uuid;primary_key;" json:"id"`
	CreatedAt time.Time      `gorm:"index" json:"created_at"`
	UpdatedAt time.Time      `json:"updated_at"`
	DeletedAt gorm.DeletedAt `gorm:"index" json:"-"`
}

func (base *Base) BeforeCreate(tx *gorm.DB) error {
	if base.ID == uuid.Nil {
		base.ID = uuid.New()
	}
	return nil
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
	Name        string `gorm:"uniqueIndex;not null" json:"name"`
	Description string `json:"description"`
	Status      string `json:"status"` // e.g., "active", "completed", "archived"
}

type Container struct {
	Base
	Name        string     `gorm:"not null" json:"name"`
	Description string     `json:"description"`
	LocationID  *uuid.UUID `gorm:"type:uuid;index" json:"location_id"`
	Location    *Location  `json:"location,omitempty"`
	ParentID    *uuid.UUID `gorm:"type:uuid;index" json:"parent_id"`
	Parent      *Container `json:"parent,omitempty"`
	ProjectID   *uuid.UUID `gorm:"type:uuid;index" json:"project_id"`
	Project     *Project   `json:"project,omitempty"`
}

type ItemType struct {
	Base
	Name           string     `gorm:"not null" json:"name"`
	Description    string     `json:"description"`
	Specifications string     `json:"specifications"`
	Manufacturer   string     `json:"manufacturer"`
	PartNumber     string     `json:"part_number"`
	CategoryID     *uuid.UUID `gorm:"type:uuid;index" json:"category_id"`
	Category       *Category  `json:"category,omitempty"`
}

type Item struct {
	Base
	Name             string     `json:"name"` // Used if ItemType is not provided
	Description      string     `json:"description"`
	Quantity         int        `gorm:"not null;default:1" json:"quantity"`
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

type ActivityLog struct {
	Base
	Action          string     `gorm:"not null" json:"action"` // e.g., "CREATED", "MODIFIED", "DELETED", "MOVED", "CHECK_IN", "CHECK_OUT", "QUANTITY_ADJUSTED", "ASSIGNED_TO_PROJECT", "RETURNED_FROM_PROJECT"
	EntityID        uuid.UUID  `gorm:"type:uuid;index;not null" json:"entity_id"`
	EntityType      string     `gorm:"not null" json:"entity_type"` // e.g., "Item", "Container", "Location"
	Details         string     `json:"details"`
	UserID          *uuid.UUID `gorm:"type:uuid;index" json:"user_id"` // Optional: If we ever add users
	FromContainerID *uuid.UUID `gorm:"type:uuid;index" json:"from_container_id"`
	FromContainer   *Container `json:"from_container,omitempty"`
	ToContainerID   *uuid.UUID `gorm:"type:uuid;index" json:"to_container_id"`
	ToContainer     *Container `json:"to_container,omitempty"`
	ProjectID       *uuid.UUID `gorm:"type:uuid;index" json:"project_id"`
	Project         *Project   `json:"project,omitempty"`
	QuantityChange  int        `gorm:"default:0" json:"quantity_change"`
}
