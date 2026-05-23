package models

type PaginatedResponse[T any] struct {
	Items  []T   `json:"items"`
	Total  int64 `json:"total"`
	Limit  int   `json:"limit"`
	Offset int   `json:"offset"`
}

// Concrete types for swagger since swagger doesn't fully support go generics cleanly
type PaginatedItemsResponse struct {
	Items  []Item `json:"items"`
	Total  int64  `json:"total"`
	Limit  int    `json:"limit"`
	Offset int    `json:"offset"`
}

type PaginatedActivityLogsResponse struct {
	Items  []ActivityLog `json:"items"`
	Total  int64         `json:"total"`
	Limit  int           `json:"limit"`
	Offset int           `json:"offset"`
}

type PaginatedItemTypesResponse struct {
	Items  []ItemType `json:"items"`
	Total  int64      `json:"total"`
	Limit  int        `json:"limit"`
	Offset int        `json:"offset"`
}
