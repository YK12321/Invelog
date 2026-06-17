package dto

import "github.com/google/uuid"

type CreateItemTypeRequest struct {
	Name           string     `json:"name" binding:"required"`
	Description    string     `json:"description"`
	Specifications string     `json:"specifications"`
	Manufacturer   string     `json:"manufacturer"`
	PartNumber     string     `json:"part_number"`
	CategoryID     *uuid.UUID `json:"category_id"`
}

type UpdateItemTypeRequest struct {
	Name           *string    `json:"name"`
	Description    *string    `json:"description"`
	Specifications *string    `json:"specifications"`
	Manufacturer   *string    `json:"manufacturer"`
	PartNumber     *string    `json:"part_number"`
	CategoryID     *uuid.UUID `json:"category_id"`
}
