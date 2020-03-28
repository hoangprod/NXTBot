#pragma once


enum class Stat {
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
	SUMMONING,
	DUNGEONEERING,
	DIVINATION,
	INVENTION,
	ARCHEOLOGY
};

class Exp
{
public:
	static UINT_PTR GetCharacterInfoClass();
	static int GetCurrentExp(Stat type);
	static int GetSkillLevel(Stat type);

};