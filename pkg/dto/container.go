package dto

import "github.com/google/uuid"

type CreateContainerRequest struct {
	Name        string     `json:"name" binding:"required"`
	Description string     `json:"description"`
	LocationID  *uuid.UUID `json:"location_id"`
	ParentID    *uuid.UUID `json:"parent_id"`
	ProjectID   *uuid.UUID `json:"project_id"`
}
type UpdateContainerInput struct {
	Name        *string    `json:"name"`
	Description *string    `json:"description"`
	LocationID  *uuid.UUID `json:"location_id"`
	ParentID    *uuid.UUID `json:"parent_id"`
	ProjectID   *uuid.UUID `json:"project_id"`
}
