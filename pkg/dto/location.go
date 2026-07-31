package dto

type CreateLocationRequest struct {
	Name        string `json:"name" binding:"required"`
	Description string `json:"description"`
}

type UpdateLocationInput struct {
	Name        *string `json:"name"`
	Description *string `json:"description"`
}
