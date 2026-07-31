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
			// Personal tasks query
			protected.GET("/my-tasks", h.GetMyTasks)

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

			// Projects & SEPM / PM Suite
			protected.POST("/projects", h.CreateProject)
			protected.GET("/projects", h.ListProjects)
			protected.GET("/projects/:id", h.GetProject)
			protected.PUT("/projects/:id", h.UpdateProject)
			protected.DELETE("/projects/:id", middleware.RequireRole("admin"), h.DeleteProject)

			// Project Team Members
			protected.POST("/projects/:id/members", h.AddProjectMember)
			protected.GET("/projects/:id/members", h.ListProjectMembers)
			protected.DELETE("/projects/:id/members/:user_id", h.RemoveProjectMember)

			// Project Tasks & Resource Allocation
			protected.POST("/projects/:id/tasks", h.CreateTask)
			protected.GET("/projects/:id/tasks", h.ListTasks)
			protected.PUT("/projects/:id/tasks/:task_id", h.UpdateTask)
			protected.DELETE("/projects/:id/tasks/:task_id", h.DeleteTask)
			protected.POST("/projects/:id/tasks/:task_id/reserve-resource", h.ReserveTaskResource)

			// Project Milestones
			protected.POST("/projects/:id/milestones", h.CreateMilestone)
			protected.GET("/projects/:id/milestones", h.ListMilestones)

			// Project Requirements (SEPM)
			protected.POST("/projects/:id/requirements", h.CreateRequirement)
			protected.GET("/projects/:id/requirements", h.ListRequirements)

			// Project WBS (SEPM)
			protected.POST("/projects/:id/wbs", h.CreateWBSNode)
			protected.GET("/projects/:id/wbs", h.ListWBSNodes)

			// Project Risk Register (FMEA / SEPM)
			protected.POST("/projects/:id/risks", h.CreateRiskItem)
			protected.GET("/projects/:id/risks", h.ListRiskItems)

			// PM & SEPM Executive Dashboard
			protected.GET("/projects/:id/dashboard", h.GetPMDashboard)

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
