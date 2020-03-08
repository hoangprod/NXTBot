#pragma once


enum class SkillType {
	ATTACK,
	DEFENCE,
	STRENGTH,
	HITPOINTS,
	RANGE,
	PRAYER,
	MAGIC,
	COOKING,
	WOODCUTTING,
	FLETCHING,
	FISHING,
	FIREMAKING,
	CRAFTING,
	SMITHING,
	MINING,
	HERBLORE,
	AGILITY,
	THIEVING,
	SLAYER,
	FARMING,
	RUNECRAFT,
	HUNTER,
	CONSTRUCTION,
};

class Exp
{
public:
	static UINT_PTR GetCharacterInfoClass();
	static int GetCurrentExp(SkillType type);
};