package dto

type CreateProjectRequest struct {
	Name        string `json:"name" binding:"required"`
	Description string `json:"description"`
}

type UpdateProjectInput struct {
	Name        *string `json:"name"`
	Description *string `json:"description"`
}
