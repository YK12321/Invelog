package dto

import "github.com/google/uuid"

type CreateItemRequest struct {
	Name             string     `json:"name"`
	Description      string     `json:"description"`
	Quantity         *int       `json:"quantity"`
	MinQuantity      *int       `json:"min_quantity"`
	ReorderQuantity  *int       `json:"reorder_quantity"`
	SKU              string     `json:"sku"`
	Barcode          string     `json:"barcode"`
	IndividualNotes  string     `json:"individual_notes"`
	SerialNumber     string     `json:"serial_number"`
	ItemTypeID       *uuid.UUID `json:"item_type_id"`
	CategoryID       *uuid.UUID `json:"category_id"`
	ContainerID      *uuid.UUID `json:"container_id"`
	OriginLocationID *uuid.UUID `json:"origin_location_id"`
}
