package models

import "github.com/google/uuid"

type GlobalSetting struct {
	Base
	Key   string `gorm:"uniqueIndex;not null" json:"key"`
	Value string `gorm:"not null" json:"value"`
}

type UserSetting struct {
	Base
	UserID uuid.UUID `gorm:"type:uuid;index;not null" json:"user_id"`
	Key    string    `gorm:"index;not null" json:"key"`
	Value  string    `gorm:"not null" json:"value"`
}
