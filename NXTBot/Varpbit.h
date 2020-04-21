#pragma once

enum class VarDomain
{
	PLAYER,
	NPC,
	CLIENT,
	OBJECT,
	CLAN,
	CLAN_SETTING,
	PLAYER_GROUP
};

class Varpbit
{
public:
	static int GetVarpBit(int VarpId);
	static int GetVarp(int VarpId);
	static UINT_PTR* GetVarpType(VarDomain domain, int VarpId);

	static int GetPlayerHealth();
	static int GetPlayerPrayer();
	static int GetPlayerAdrenaline();
	static int GetPlayerIsPoison();
	static int GetPlayerPrayerToggle();


	static std::vector<int> ScanVarpValue(std::vector<int> varps, int value);

};