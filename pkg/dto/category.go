package dto

type UpdateCategoryInput struct {
	Name        *string `json:"name"`
	Description *string `json:"description"`
}
