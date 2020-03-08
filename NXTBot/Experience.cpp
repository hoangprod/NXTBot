#include "pch.h"
#include "GameClasses.h"
#include "Patterns.h"
#include "Inventory.h"
#include "Game.h"
#include "PlayableEntity.h"
#include "Experience.h"

extern Addr Patterns;


UINT_PTR Exp::GetCharacterInfoClass()
{
	GameContext* context = RS::GetGameContext();

	if (!context)
		return 0;

	return *(UINT_PTR*)((UINT_PTR)context + Patterns.Offset_CharacterInfo);
}

int Exp::GetCurrentExp(SkillType type)
{
	UINT_PTR CharacterInfoClass = GetCharacterInfoClass();

	if (!CharacterInfoClass)
		return -1;

	UINT_PTR SkillExpClass = *(UINT_PTR*)(*(UINT_PTR*)(CharacterInfoClass + Patterns.Offset_SkillExperience) + 0x10);

	if (!SkillExpClass)
		return -1;

	return *(int*)(SkillExpClass + (0x18 * (size_t)type) + 0xc);
}
