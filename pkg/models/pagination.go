package models

// PaginatedItemsResponse represents a paginated list of items
type PaginatedItemsResponse struct {
	Items  []Item `json:"items"`
	Total  int64  `json:"total"`
	Limit  int    `json:"limit"`
	Offset int    `json:"offset"`
}
