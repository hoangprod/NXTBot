#include "pch.h"
#include "GameClasses.h"
#include "Varpbit.h"

extern UINT_PTR* g_GameContext;

int Varpbit::GetVarpBit(int VarpId)
{
	auto contextPtr = (GameContextPtr*)g_GameContext;

	if (!contextPtr)
		return -1;

	auto g_context = contextPtr->gContext;

	if (!g_context)
		return -1;

	auto wrap_1 = g_context->VarpWrap;

	if (!wrap_1)
		return -1;


	if (wrap_1->isReady == 4)
	{
		typedef UINT_PTR*(__fastcall* _GetVarp)(uint64_t* a1, unsigned int a2, uint64_t a3);
		UINT_PTR* wrap_2 = reinterpret_cast<_GetVarp>(wrap_1->VarpWrap2->DomainClass2->DomainVTable->GetVarType)((UINT_PTR*)wrap_1->VarpWrap2->DomainClass2, VarpId, 0);



		if (wrap_2[1])
		{
			typedef int (__fastcall* _GetVarpbit)(uint64_t a1, uint64_t * a2);
			int varpbit = reinterpret_cast<_GetVarpbit>(g_context->VarpVtable->GetVarpbitFunc)((uint64_t)&g_context->VarpVtable, wrap_2);

			return varpbit;
		}
	}

	return 0;
}

int Varpbit::GetVarp(int VarpId)
{
	auto contextPtr = (GameContextPtr*)g_GameContext;

	if (!contextPtr)
		return 0;

	auto g_context = contextPtr->gContext;

	if (!g_context)
		return 0;

	auto Varptype = Varpbit::GetVarpType(VarDomain::PLAYER, VarpId);

	if (Varptype[1])
	{

		typedef VarpClass*(__fastcall* _GetVarClass)(uint64_t a1, uint64_t* a2);
		VarpClass* varpClass = reinterpret_cast<_GetVarClass>(g_context->VarpVtable->GetVarp)((uint64_t)&g_context->VarpVtable, Varptype);

		if (!varpClass || (UINT_PTR)varpClass < 0x100000 || IsBadReadPtr(varpClass, 0x28))
			return -1;

		return varpClass->Value;
	}
	else
		return -1;
}

UINT_PTR* Varpbit::GetVarpType(VarDomain domain, int VarpId)
{
	auto contextPtr = (GameContextPtr*)g_GameContext;

	if (!contextPtr)
		return 0;

	auto g_context = contextPtr->gContext;

	if (!g_context)
		return 0;

	auto wrap_1 = g_context->VarpWrap;

	if (!wrap_1)
		return 0;

	switch (domain)
	{
	case VarDomain::PLAYER:
		typedef UINT_PTR* (__fastcall* _GetVarp)(uint64_t* a1, unsigned int a2, uint64_t a3);
		return reinterpret_cast<_GetVarp>(wrap_1->PlayerDomain->DomainClass2->DomainVTable->GetVarType)((UINT_PTR*)wrap_1->PlayerDomain->DomainClass2, VarpId, 0);
		break;
	case VarDomain::NPC:
		typedef UINT_PTR* (__fastcall* _GetVarp)(uint64_t* a1, unsigned int a2, uint64_t a3);
		return reinterpret_cast<_GetVarp>(wrap_1->NpcDomain->DomainClass2->DomainVTable->GetVarType)((UINT_PTR*)wrap_1->NpcDomain->DomainClass2, VarpId, 0);
		break;
	case VarDomain::CLIENT:
		typedef UINT_PTR* (__fastcall* _GetVarp)(uint64_t* a1, unsigned int a2, uint64_t a3);
		return reinterpret_cast<_GetVarp>(wrap_1->ClientDomain->DomainClass2->DomainVTable->GetVarType)((UINT_PTR*)wrap_1->ClientDomain->DomainClass2, VarpId, 0);
		break;
	case VarDomain::OBJECT:
		typedef UINT_PTR* (__fastcall* _GetVarp)(uint64_t* a1, unsigned int a2, uint64_t a3);
		return reinterpret_cast<_GetVarp>(wrap_1->ObjectDomain->DomainClass2->DomainVTable->GetVarType)((UINT_PTR*)wrap_1->ObjectDomain->DomainClass2, VarpId, 0);
		break;
	case VarDomain::CLAN:
		typedef UINT_PTR* (__fastcall* _GetVarp)(uint64_t* a1, unsigned int a2, uint64_t a3);
		return reinterpret_cast<_GetVarp>(wrap_1->ClanDomain->DomainClass2->DomainVTable->GetVarType)((UINT_PTR*)wrap_1->ClanDomain->DomainClass2, VarpId, 0);
		break;
	case VarDomain::CLAN_SETTING:
		typedef UINT_PTR* (__fastcall* _GetVarp)(uint64_t* a1, unsigned int a2, uint64_t a3);
		return reinterpret_cast<_GetVarp>(wrap_1->ClanSettingDomain->DomainClass2->DomainVTable->GetVarType)((UINT_PTR*)wrap_1->ClanSettingDomain->DomainClass2, VarpId, 0);
		break;
	case VarDomain::PLAYER_GROUP:
		typedef UINT_PTR* (__fastcall* _GetVarp)(uint64_t* a1, unsigned int a2, uint64_t a3);
		return reinterpret_cast<_GetVarp>(wrap_1->PlayerGroupDomain->DomainClass2->DomainVTable->GetVarType)((UINT_PTR*)wrap_1->PlayerGroupDomain->DomainClass2, VarpId, 0);
		break;
	default:
		break;
	}
	
	return 0;
}

int Varpbit::GetPlayerHealth()
{
	return GetVarpBit(1668);
}

int Varpbit::GetPlayerPrayer()
{
	return GetVarp(3274);
}

int Varpbit::GetPlayerAdrenaline()
{
	return GetVarp(679);
}

int Varpbit::GetPlayerIsPoison()
{
	return GetVarpBit(2104);
}


int Varpbit::GetPlayerPrayerToggle()
{
	return GetVarpBit(5941);
}

std::vector<std::pair<int, int>> Varpbit::ScanNewVarpValue(std::vector<std::pair<int, int>> varps, int value)
{
	std::vector<std::pair<int, int>> result;

	if (varps.size() > 0)
	{
		for (auto varp : varps)
		{
			auto varpValue = GetVarpBit(varp.first);

			if (varpValue == value)
			{
				std::pair<int, int> pair = std::make_pair(varp.first, varpValue);
				result.push_back(pair);
				printf("Varp: %d, value: %d\n", varp.first, varpValue);
			}
		}
	}
	else
	{
		for (int varp = 10; varp < 200000; varp++)
		{
			auto varpValue = GetVarpBit(varp);

			if (varpValue == value)
			{
				std::pair<int, int> pair = std::make_pair(varp, varpValue);
				result.push_back(pair);
				printf("Varp: %d, value: %d\n", varp, varpValue);
			}
		}
	}

	printf("-----------------------------------------------\n\n");

	return result;
}

std::vector<std::pair<int, int>> Varpbit::ScanNextVarpValue(std::vector<std::pair<int, int>> varps, bool bChanged, int value)
{
	std::vector<std::pair<int, int>> result;

	if (varps.size() > 0)
	{
		for (auto varp : varps)
		{
			auto varpValue = GetVarpBit(varp.first);

			if (bChanged)
			{
				if (value != 9999)
				{
					if (varpValue == value)
					{
						std::pair<int, int> pair = std::make_pair(varp.first, varpValue);
						result.push_back(pair);
						printf("Unchanged: Varp: %d, value: %d\n", varp.first, varpValue);
					}
				}
				else
				{
					if (varpValue != varp.second)
					{
						std::pair<int, int> pair = std::make_pair(varp.first, varpValue);
						result.push_back(pair);
						printf("Unchanged: Varp: %d, value: %d\n", varp.first, varpValue);
					}
				}
			}
			else
			{
				if (varpValue == varp.second)
				{
					std::pair<int, int> pair = std::make_pair(varp.first, varpValue);
					result.push_back(pair);
					printf("Unchanged: Varp: %d, value: %d\n", varp.first, varpValue);
				}
			}


		}
	}

	printf("-----------------------------------------------\n\n");
	return result;

}
