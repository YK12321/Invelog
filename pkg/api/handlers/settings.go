package handlers

import (
	"net/http"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
)

type SetAdminSettingRequest struct {
	Key   string `json:"key" binding:"required"`
	Value string `json:"value" binding:"required"`
}

type SetUserSettingRequest struct {
	UserID uuid.UUID `json:"user_id" binding:"required"`
	Key    string    `json:"key" binding:"required"`
	Value  string    `json:"value" binding:"required"`
}

// @Summary Set Admin Setting
// @Description Set a global admin setting (e.g., default pagination limit)
// @Tags Settings
// @Accept json
// @Produce json
// @Param setting body SetAdminSettingRequest true "Admin Setting Data"
// @Success 200 {object} map[string]string
// @Failure 400 {object} map[string]string
// @Failure 500 {object} map[string]string
// @Router /settings/admin [put]
func (h *Handler) SetAdminSetting(c *gin.Context) {
	var req SetAdminSettingRequest
	if err := c.ShouldBindJSON(&req); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	if err := h.Settings.SetGlobalSetting(req.Key, req.Value); err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to update admin setting"})
		return
	}

	h.LogActivity("UPDATE", "GlobalSetting", uuid.Nil, "Updated global setting: "+req.Key)
	c.JSON(http.StatusOK, gin.H{"status": "success", "key": req.Key, "value": req.Value})
}

// @Summary Set User Setting
// @Description Set a user-specific setting (e.g., custom pagination limit)
// @Tags Settings
// @Accept json
// @Produce json
// @Param setting body SetUserSettingRequest true "User Setting Data"
// @Success 200 {object} map[string]string
// @Failure 400 {object} map[string]string
// @Failure 500 {object} map[string]string
// @Router /settings/user [put]
func (h *Handler) SetUserSetting(c *gin.Context) {
	var req SetUserSettingRequest
	if err := c.ShouldBindJSON(&req); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	if err := h.Settings.SetUserSetting(req.UserID, req.Key, req.Value); err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to update user setting"})
		return
	}

	h.LogActivity("UPDATE", "UserSetting", req.UserID, "Updated user setting: "+req.Key)
	c.JSON(http.StatusOK, gin.H{"status": "success", "key": req.Key, "value": req.Value, "user_id": req.UserID.String()})
}
