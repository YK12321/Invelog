package router

import (
	"invelog/pkg/api/handlers"
	"invelog/pkg/api/middleware"

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

		// Public Auth routes
		auth := api.Group("/auth")
		{
			auth.POST("/register", h.Register)
			auth.POST("/login", h.Login)
		}

		// Protected API routes
		protected := api.Group("")
		protected.Use(middleware.AuthMiddleware())
		{
			// Rapid scan lookup
			protected.GET("/scan/:code", h.ScanLookup)

			// Audit Summary
			protected.GET("/audit/summary", h.GetAuditSummary)

			// Categories
			protected.POST("/categories", h.CreateCategory)
			protected.GET("/categories", h.ListCategories)
			protected.GET("/categories/:id", h.GetCategory)
			protected.PUT("/categories/:id", h.UpdateCategory)
			protected.DELETE("/categories/:id", middleware.RequireRole("admin"), h.DeleteCategory)

			// Locations
			protected.POST("/locations", h.CreateLocation)
			protected.GET("/locations", h.ListLocations)
			protected.GET("/locations/:id", h.GetLocation)
			protected.PUT("/locations/:id", h.UpdateLocation)
			protected.DELETE("/locations/:id", middleware.RequireRole("admin"), h.DeleteLocation)

			// Projects
			protected.POST("/projects", h.CreateProject)
			protected.GET("/projects", h.ListProjects)
			protected.GET("/projects/:id", h.GetProject)
			protected.PUT("/projects/:id", h.UpdateProject)
			protected.DELETE("/projects/:id", middleware.RequireRole("admin"), h.DeleteProject)

			// Containers
			protected.POST("/containers", h.CreateContainer)
			protected.GET("/containers", h.ListContainers)
			protected.GET("/containers/:id", h.GetContainer)
			protected.PUT("/containers/:id", h.UpdateContainer)
			protected.DELETE("/containers/:id", middleware.RequireRole("admin"), h.DeleteContainer)

			// ItemTypes
			protected.POST("/item-types", h.CreateItemType)
			protected.GET("/item-types", h.ListItemTypes)
			protected.GET("/item-types/low-stock", h.GetLowStockItemTypes)
			protected.GET("/item-types/:id", h.GetItemType)
			protected.PUT("/item-types/:id", h.UpdateItemType)
			protected.DELETE("/item-types/:id", middleware.RequireRole("admin"), h.DeleteItemType)

			// Items
			protected.POST("/items", h.CreateItem)
			protected.GET("/items", h.ListItems)
			protected.GET("/items/low-stock", h.GetLowStockItems)
			protected.POST("/items/import", h.ImportItemsCSV)
			protected.GET("/items/export", h.ExportItemsCSV)
			protected.GET("/items/:id", h.GetItem)
			protected.PUT("/items/:id", h.UpdateItem)
			protected.DELETE("/items/:id", middleware.RequireRole("admin"), h.DeleteItem)
			protected.POST("/items/:id/move", h.MoveItem)
			protected.POST("/items/:id/checkout", h.CheckOutItem)
			protected.POST("/items/:id/checkin", h.CheckInItem)
			protected.POST("/items/:id/audit", h.AuditItem)

			// Search
			protected.GET("/search/items", h.SearchItems)

			// Activity Logs
			protected.GET("/activity-logs", h.ListActivityLogs)
		}
	}

	return r
}
