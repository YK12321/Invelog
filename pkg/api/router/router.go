package router

import (
	"invelog/pkg/api/handlers"

	"github.com/gin-gonic/gin"
	swaggerFiles "github.com/swaggo/files"
	ginSwagger "github.com/swaggo/gin-swagger"
	"gorm.io/gorm"
)

func SetupRouter(db *gorm.DB) *gin.Engine {
	r := gin.Default()

	// Swagger documentation endpoint
	r.GET("/swagger/*any", ginSwagger.WrapHandler(swaggerFiles.Handler))

	h := handlers.NewHandler(db)

	api := r.Group("/api/v1")
	{
		api.GET("/health", h.HealthCheck)

		// Categories
		api.POST("/categories", h.CreateCategory)
		api.GET("/categories", h.ListCategories)
		api.GET("/categories/:id", h.GetCategory)
		api.PUT("/categories/:id", h.UpdateCategory)
		api.DELETE("/categories/:id", h.DeleteCategory)

		// Locations
		api.POST("/locations", h.CreateLocation)
		api.GET("/locations", h.ListLocations)
		api.GET("/locations/:id", h.GetLocation)
		api.PUT("/locations/:id", h.UpdateLocation)
		api.DELETE("/locations/:id", h.DeleteLocation)

		// Projects
		api.POST("/projects", h.CreateProject)
		api.GET("/projects", h.ListProjects)
		api.GET("/projects/:id", h.GetProject)
		api.PUT("/projects/:id", h.UpdateProject)
		api.DELETE("/projects/:id", h.DeleteProject)

		// Containers
		api.POST("/containers", h.CreateContainer)
		api.GET("/containers", h.ListContainers)
		api.GET("/containers/:id", h.GetContainer)
		api.PUT("/containers/:id", h.UpdateContainer)
		api.DELETE("/containers/:id", h.DeleteContainer)

		// ItemTypes
		api.POST("/item-types", h.CreateItemType)
		api.GET("/item-types", h.ListItemTypes)
		api.GET("/item-types/:id", h.GetItemType)
		api.PUT("/item-types/:id", h.UpdateItemType)
		api.DELETE("/item-types/:id", h.DeleteItemType)

		// Items
		api.POST("/items", h.CreateItem)
		api.GET("/items", h.ListItems)
		api.GET("/items/:id", h.GetItem)
		api.PUT("/items/:id", h.UpdateItem)
		api.DELETE("/items/:id", h.DeleteItem)
		api.POST("/items/:id/move", h.MoveItem)
		api.POST("/items/:id/checkout", h.CheckOutItem)
		api.POST("/items/:id/checkin", h.CheckInItem)

		// Search
		api.GET("/search/items", h.SearchItems)

		// Activity Logs
		api.GET("/activity-logs", h.ListActivityLogs)
	}

	return r
}
