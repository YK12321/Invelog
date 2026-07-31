package dto

import (
	"encoding/json"

	"github.com/google/uuid"
)

type CreateItemTypeRequest struct {
	Name            string          `json:"name" binding:"required"`
	Description     string          `json:"description"`
	Specifications  string          `json:"specifications"`
	Parameters      json.RawMessage `json:"parameters"`
	Manufacturer    string          `json:"manufacturer"`
	PartNumber      string          `json:"part_number"`
	MinQuantity     *int            `json:"min_quantity"`
	ReorderQuantity *int            `json:"reorder_quantity"`
	CategoryID      *uuid.UUID      `json:"category_id"`
}

type UpdateItemTypeInput struct {
	Name            *string          `json:"name"`
	Description     *string          `json:"description"`
	Specifications  *string          `json:"specifications"`
	Parameters      json.RawMessage  `json:"parameters"`
	Manufacturer    *string          `json:"manufacturer"`
	PartNumber      *string          `json:"part_number"`
	MinQuantity     *int             `json:"min_quantity"`
	ReorderQuantity *int             `json:"reorder_quantity"`
	CategoryID      *uuid.UUID       `json:"category_id"`
}
