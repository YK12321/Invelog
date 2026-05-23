package settings

import (
	"log"
	"os"
	"strconv"
	"strings"

	"invelog/pkg/models"

	"github.com/google/uuid"
	"gorm.io/gorm"
)

type SettingsManager struct {
	DB *gorm.DB
}

func NewSettingsManager(db *gorm.DB) *SettingsManager {
	sm := &SettingsManager{DB: db}
	sm.InitializeDefaults()
	return sm
}

func (sm *SettingsManager) InitializeDefaults() {
	// Define default keys and their fallback values if not set anywhere
	defaultLimits := map[string]int{
		"ITEMS_DEFAULT_LIMIT":         20,
		"ACTIVITY_LOGS_DEFAULT_LIMIT": 100,
		"ITEM_TYPES_DEFAULT_LIMIT":    100,
	}

	for key, fallback := range defaultLimits {
		envVal := os.Getenv(key)

		var globalSetting models.GlobalSetting
		err := sm.DB.Where("key = ?", key).First(&globalSetting).Error

		if err == nil {
			// Setting exists in DB
			if envVal != "" && envVal != globalSetting.Value {
				log.Printf("WARNING: Environment variable %s=%s overrides database global setting (%s) for this instance.\n", key, envVal, globalSetting.Value)
			}
		} else if err == gorm.ErrRecordNotFound {
			// Initialize in DB if empty (using env val if present, else fallback)
			valToSave := strconv.Itoa(fallback)
			if envVal != "" {
				valToSave = envVal
			}
			sm.DB.Create(&models.GlobalSetting{
				Key:   key,
				Value: valToSave,
			})
		}
	}
}

// GetGlobalLimit gets the effective global limit (Env var overrides DB)
func (sm *SettingsManager) GetGlobalLimit(key string) int {
	// 1. Check Env Var
	if envVal := os.Getenv(key); envVal != "" {
		if val, err := strconv.Atoi(envVal); err == nil {
			return val
		}
	}

	// 2. Check DB Global Setting
	var globalSetting models.GlobalSetting
	if err := sm.DB.Where("key = ?", key).First(&globalSetting).Error; err == nil {
		if val, err := strconv.Atoi(globalSetting.Value); err == nil {
			return val
		}
	}

	// 3. Fallback (should ideally be seeded, but safe fallback)
	if strings.HasPrefix(key, "ITEMS_") {
		return 20
	}
	return 100
}

// GetUserLimit gets the effective limit for a user (User DB -> Env Var -> Global DB)
func (sm *SettingsManager) GetUserLimit(userID uuid.UUID, key string) *int {
	var userSetting models.UserSetting
	if err := sm.DB.Where("user_id = ? AND key = ?", userID, key).First(&userSetting).Error; err == nil {
		if val, err := strconv.Atoi(userSetting.Value); err == nil {
			return &val
		}
	}
	return nil
}

// ResolveLimit decides the final limit for pagination
func (sm *SettingsManager) ResolveLimit(endpointKey string, requestedLimit int, userID *uuid.UUID) int {
	maxKey := strings.Replace(endpointKey, "DEFAULT", "MAX", 1)

	maxLimit := sm.GetGlobalLimit(maxKey)
	if maxLimit == 100 && strings.HasPrefix(maxKey, "ACTIVITY_LOGS") {
		maxLimit = 1000 // default fallback logic for max
	} else if maxLimit == 100 && strings.HasPrefix(maxKey, "ITEM_TYPES") {
		maxLimit = 1000
	} else if maxLimit == 100 && strings.HasPrefix(maxKey, "ITEMS") {
		maxLimit = 100
	}

	if requestedLimit > 0 {
		if requestedLimit > maxLimit {
			return maxLimit
		}
		return requestedLimit
	}

	// If no requested limit, fallback to settings
	if userID != nil && *userID != uuid.Nil {
		if userLim := sm.GetUserLimit(*userID, endpointKey); userLim != nil {
			if *userLim > maxLimit {
				return maxLimit
			}
			return *userLim
		}
	}

	globalLim := sm.GetGlobalLimit(endpointKey)
	if globalLim > maxLimit {
		return maxLimit
	}
	return globalLim
}

// SetGlobalSetting updates a global setting
func (sm *SettingsManager) SetGlobalSetting(key, value string) error {
	var setting models.GlobalSetting
	if err := sm.DB.Where("key = ?", key).First(&setting).Error; err != nil {
		if err == gorm.ErrRecordNotFound {
			return sm.DB.Create(&models.GlobalSetting{Key: key, Value: value}).Error
		}
		return err
	}
	setting.Value = value
	return sm.DB.Save(&setting).Error
}

// SetUserSetting updates a user setting
func (sm *SettingsManager) SetUserSetting(userID uuid.UUID, key, value string) error {
	var setting models.UserSetting
	if err := sm.DB.Where("user_id = ? AND key = ?", userID, key).First(&setting).Error; err != nil {
		if err == gorm.ErrRecordNotFound {
			return sm.DB.Create(&models.UserSetting{UserID: userID, Key: key, Value: value}).Error
		}
		return err
	}
	setting.Value = value
	return sm.DB.Save(&setting).Error
}
