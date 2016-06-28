// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/UserDefinedEnum.h"
#include "Test2DEnums.generated.h"

/**
 * 
 */
UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EAllianceType : uint8
{
	AT_Player,
	AT_AI,
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ESkillCollisionType : uint8
{
	SC_Box 		UMETA(DisplayName = "Box"),
	SC_Circle 	UMETA(DisplayName = "Circle"),
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ESkillSlotTypes : uint8
{
	SS_Main,
	SS_Secondary,

	SS_Util_0,
	SS_Util_1,
	SS_Util_2,

	SS_Buff_0,
	SS_Buff_1,
	SS_Buff_2,

	SS_Dodge,

	SS_None,
};
UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ESkillMotion : uint8
{
	SM_START,
	SM_UPDATE,
	SM_END,
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EDmgType : uint8
{
	DT_Skill,
	DT_OnHit,
	DT_NumHitEffect,
	DT_DoT,
	DT_Heal,
	DT_Reflect,
	DT_Invulnerable,
	DT_Evade,

};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EStatusType : uint8
{
	ST_STATUS,
	ST_BUFF,
	ST_DEBUFF,
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EItemType : uint8
{
	EQUIPPABLE,
	USABLE,
	ETC,
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EEquipType : uint8
{
	WEAPON,
	HELMET,
	CHEST,
	PANTS,
	BOOTS,
	GLOVES,
	ACCESSORY,
	NONE,
};
UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EWeaponType : uint8
{
	THROWINGSTAR,
	BOW,
	GUN,
	SWORD,
	STAFF,
	NONE,
};
