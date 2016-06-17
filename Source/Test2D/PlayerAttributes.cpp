// Fill out your copyright notice in the Description page of Project Settings.

#include "Test2D.h"
#include "PlayerAttributes.h"

FPlayerAttributes UStructHelperFunctions::SubtractAttributes(FPlayerAttributes lhs, FPlayerAttributes rhs)
{
	FPlayerAttributes ret;
	ret.MaxHP = lhs.MaxHP - rhs.MaxHP;
	ret.HpRegeneration = lhs.HpRegeneration - rhs.HpRegeneration;
	ret.MaxJumps = lhs.MaxJumps - rhs.MaxJumps;
	ret.Defense = lhs.Defense - rhs.Defense;
	ret.Damage_Reduction = lhs.Damage_Reduction - rhs.Damage_Reduction;
	ret.Defense_Penetration = lhs.Defense_Penetration - rhs.Defense_Penetration;
	ret.Damage = lhs.Damage - rhs.Damage;
	ret.Critical_Chance = lhs.Critical_Chance - rhs.Critical_Chance;
	ret.Critical_Damage = lhs.Critical_Damage - rhs.Critical_Damage;
	ret.LifeSteal = lhs.LifeSteal - rhs.LifeSteal;
	ret.AttackSpeed = lhs.AttackSpeed - rhs.AttackSpeed;
	ret.AttackSpeed_Multiplier = lhs.AttackSpeed_Multiplier - rhs.AttackSpeed_Multiplier;
	ret.AdditionalHits = lhs.AdditionalHits - rhs.AdditionalHits;
	ret.HitMultiplier = lhs.HitMultiplier - rhs.HitMultiplier;
	ret.Stance = lhs.Stance - rhs.Stance;


	return ret;
}

FPlayerAttributes UStructHelperFunctions::AddAttributes(FPlayerAttributes lhs, FPlayerAttributes rhs)
{
	FPlayerAttributes ret;
	ret.MaxHP = lhs.MaxHP + rhs.MaxHP;
	ret.HpRegeneration = lhs.HpRegeneration + rhs.HpRegeneration;
	ret.MaxJumps = lhs.MaxJumps + rhs.MaxJumps;
	ret.Defense = lhs.Defense + rhs.Defense;
	ret.Damage_Reduction = lhs.Damage_Reduction + rhs.Damage_Reduction;
	ret.Defense_Penetration = lhs.Defense_Penetration + rhs.Defense_Penetration;
	ret.Damage = lhs.Damage + rhs.Damage;
	ret.Critical_Chance = lhs.Critical_Chance + rhs.Critical_Chance;
	ret.Critical_Damage = lhs.Critical_Damage + rhs.Critical_Damage;
	ret.LifeSteal = lhs.LifeSteal + rhs.LifeSteal;
	ret.AttackSpeed = lhs.AttackSpeed + rhs.AttackSpeed;
	ret.AttackSpeed_Multiplier = lhs.AttackSpeed_Multiplier + rhs.AttackSpeed_Multiplier;
	ret.AdditionalHits = lhs.AdditionalHits + rhs.AdditionalHits;
	ret.HitMultiplier = lhs.HitMultiplier + rhs.HitMultiplier;
	ret.Stance = lhs.Stance + rhs.Stance;


	return ret;
}

FPlayerAttributes UStructHelperFunctions::ScaleAttributes(FPlayerAttributes lhs, float rhs)
{
	FPlayerAttributes ret;
	ret.MaxHP = lhs.MaxHP * rhs;
	ret.HpRegeneration = lhs.HpRegeneration * rhs;
	ret.MaxJumps = lhs.MaxJumps * rhs;
	ret.Defense = lhs.Defense * rhs;
	ret.Damage_Reduction = lhs.Damage_Reduction * rhs;
	ret.Defense_Penetration = lhs.Defense_Penetration * rhs;
	ret.Damage = lhs.Damage * rhs;
	ret.Critical_Chance = lhs.Critical_Chance * rhs;
	ret.Critical_Damage = lhs.Critical_Damage * rhs;
	ret.LifeSteal = lhs.LifeSteal * rhs;
	ret.AttackSpeed = lhs.AttackSpeed * rhs;
	ret.AttackSpeed_Multiplier = lhs.AttackSpeed_Multiplier * rhs;
	ret.AdditionalHits = lhs.AdditionalHits * rhs;
	ret.HitMultiplier = lhs.HitMultiplier * rhs;
	ret.Stance = lhs.Stance * rhs;

	return ret;
}


