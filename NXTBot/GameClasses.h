// Created with ReClass.NET 1.2 by KN4CK3R

enum class EntityType {
	Object = 0,
	NPC = 1,
	Players = 2,
	GroundItem = 3,
	Object2 = 12
};

class GameContextPtr
{
public:
	char pad_0000[8]; //0x0000
	class GameContext* gContext; //0x0008
	char pad_0010[24]; //0x0010
	class LoginScriptInstance* ScriptContext; //0x0028
	char pad_0030[224]; //0x0030
}; //Size: 0x0110

class DestinationFlag
{
public:
	char pad_0000[20]; //0x0000
	int X; //0x0014
	int Y; //0x0018
	char pad_001C[44]; //0x001C
}; //Size: 0x0048


class EntityObj
{
public:
	char pad_0008[48]; //0x0008
	void* camera; //0x0038
	uint32_t EntityType; //0x0040
	char pad_0044[12]; //0x0044
	uint32_t UnkAnimation; //0x0050
	char pad_0054[188]; //0x0054
	uint32_t EntityId; //0x0110
	char pad_0114[4]; //0x0114
	char* Name; //0x0118
	char pad_0120[308]; //0x0120
	uint32_t CurrentTarget; //0x0254
	char pad_0258[112]; //0x0258
	int32_t IsTargeting; //0x02C8
	char pad_02CC[116]; //0x02CC
	void* DefautAni; //0x0340
	void* CurrentAni; //0x0348
	char pad_0350[1712]; //0x0350
	uint32_t AnimationId; //0x0A00
	char pad_0A04[28]; //0x0A04
	void* DirectionPtr; //0x0A20
	char pad_0A28[68]; //0x0A28
	uint32_t ElapsedLastAction; //0x0A6C
	char pad_0A70[120]; //0x0A70
	int32_t UsefulAni; //0x0AE8
	char pad_0AEC[1160]; //0x0AEC
	uint32_t Level; //0x0F74

	virtual void Function0();
	virtual void Function1();
	virtual void Function2();
	virtual void Function3();
	virtual void Function4();
	virtual void Function5();
	virtual void Function6();
	virtual void Function7();
	virtual void Function8();
	virtual void Function9();
	virtual void Function10();
	virtual float* GetPos();
}; //Size: 0x0F78


class GameContext
{
public:
	char pad_0000[1168]; //0x0000
	class VarpWrap* VarpWrap; //0x0490
	char pad_0498[3184]; //0x0498
	class DestinationFlag* DestinationFlag; //0x1108
	char pad_1110[112]; //0x1110
	class WidgetUI* WidgetUI; //0x1180
	char pad_1188[16]; //0x1188
	class LoginManager* LoginManager; //0x1198
	char pad_11A0[8]; //0x11A0
	class CharacterInfo* CharacterInfo; //0x11A8
	char pad_11B0[8]; //0x11B0
	class EntityPtr* EntityPtr; //0x11B8
	class TileList* N00001FBE; //0x11C0
	char pad_11C8[16]; //0x11C8
	class PlayerListWrapper* PlayerListWrapper; //0x11D8
	char pad_11E0[32]; //0x11E0
	class GameContextPtr* GContext; //0x1200
	char pad_1208[24]; //0x1208
	class WorldClass* WorldClass; //0x1220
	char pad_1228[1384]; //0x1228
	class VarpInstance* VarpVtable; //0x1790
	char pad_1798[2672]; //0x1798
}; //Size: 0x2208



class NpcEntityPtr
{
public:
	char pad_0000[48]; //0x0000
	uint32_t EntityCount; //0x0030
	char pad_0034[45180]; //0x0034
	uint32_t EntityCnt; //0xB0B0
}; //Size: 0xB0B4

class EntityObjWrap
{
public:
	char pad_0000[16]; //0x0000
	class EntityObj* EntityObj; //0x0010
	char pad_0018[40]; //0x0018
}; //Size: 0x0040

class PlayerListWrapper
{
public:
	char pad_0000[264]; //0x0000
}; //Size: 0x0108

class PlayerObjWrapper
{
public:
	char pad_0000[56]; //0x0000
	class EntityObj* EntityObj; //0x0038
	char pad_0040[64]; //0x0040
}; //Size: 0x0080

class Tile2D
{
public:
	Tile2D() :x(0), y(0) {};
	Tile2D(int32_t _x, int32_t _y) : x(_x), y(_y) {};
	int32_t x; //0x0000
	int32_t y; //0x0004
}; //Size: 0x0008


class ActionHandler
{
public:
	char pad_0000[80]; //0x0000
	uint32_t Index; //0x0050
	uint32_t TileX; //0x0054
	uint32_t TileY; //0x0058
}; //Size: 0x005C

class ActionPtr
{
public:
	char pad_0000[8]; //0x0000
	class Action* Action; //0x0008
}; //Size: 0x0010

class Action
{
public:
	char pad_0000[72]; //0x0000
	class Dispatcher* Dispatcher; //0x0048
	char pad_0050[8]; //0x0050
	uint32_t Index; //0x0058
	uint32_t TileX; //0x005C
	uint32_t TileY; //0x0060
}; //Size: 0x0064

class VarpWrap
{
public:
	char pad_0000[104]; //0x0000
	uint32_t isReady; //0x0068
	char pad_006C[276]; //0x006C
	class DomainClass* PlayerDomain; //0x0180
	class DomainClass* NpcDomain; //0x0188
	class DomainClass* ClientDomain; //0x0190
	class DomainClass* ObjectDomain; //0x0198
	class DomainClass* ClanDomain; //0x01A0
	class DomainClass* ClanSettingDomain; //0x01A8
	class DomainClass* PlayerGroupDomain; //0x01B0
	class DomainClass* VarpWrap2; //0x01B8
	char pad_01C0[64]; //0x01C0
}; //Size: 0x0200

class VarpWrap2
{
public:
	char pad_0000[64]; //0x0000
	class VarpWrap3* VarpWrap3; //0x0040
}; //Size: 0x0048

class VarpWrap3
{
public:
	class GetVarpVTable* GetVarpTable; //0x0000
}; //Size: 0x0008

class VarpInstance
{
public:
	char pad_0000[8]; //0x0000
	uint64_t GetVarp; //0x0008
	char pad_0010[8]; //0x0010
	uint64_t GetVarpbitFunc; //0x0018
	char pad_0020[40]; //0x0020
}; //Size: 0x0048

class GetVarpVTable
{
public:
	char pad_0000[56]; //0x0000
	int64_t GetVarpPtr; //0x0038
	char pad_0040[8]; //0x0040
}; //Size: 0x0048

class VarpClass
{
public:
	char pad_0000[40]; //0x0000
	uint16_t Value; //0x0028
}; //Size: 0x002A

class DomainClass
{
public:
	char pad_0000[64]; //0x0000
	class DomainClassTwo* DomainClass2; //0x0040
}; //Size: 0x0048

class DomainClassTwo
{
public:
	class DomainVTable* DomainVTable; //0x0000
}; //Size: 0x0008

class DomainVTable
{
public:
	char pad_0000[56]; //0x0000
	int64_t GetVarType; //0x0038
	char pad_0040[8]; //0x0040
}; //Size: 0x0048

class Dispatcher
{
public:
	char pad_0000[56]; //0x0000
	class DispatcherInstance* DispatcherInstance; //0x0038
	char pad_0040[8]; //0x0040
}; //Size: 0x0048

class DispatcherInstance
{
public:
	class dispatcherVTable* DispatcherVTable; //0x0000
}; //Size: 0x0008

class dispatcherVTable
{
public:
	char pad_0000[16]; //0x0000
	void* dispatcherFunc; //0x0010
}; //Size: 0x0018

class FakeItem
{
public:
	int ItemId; //0x0000
	uint32_t ItemQuantity; //0x0004
}; //Size: 0x0008

class ContainerObj
{
public:
	char pad_0000[8]; //0x0000
	uint32_t ContainerId; //0x0008
	char pad_000C[4]; //0x000C
	FakeItem* ContainerContent; //0x0010
	char pad_0018[16]; //0x0018
}; //Size: 0x0028


class FullObjectClass
{
public:
	char pad_0008[104]; //0x0008
	uint32_t N00005788; //0x0070
	uint32_t N0000581B; //0x0074
	char pad_0078[40]; //0x0078
	void* NameStart; //0x00A0
	void* NameEnd; //0x00A8
	char pad_00B0[16]; //0x00B0
	char* Name; //0x00C0
	char pad_00C8[280]; //0x00C8
	int Id; //0x01E0
	int Quantity; //0x01E4
	char pad_01E8[600]; //0x01E8

	virtual void Function0();
	virtual void Function1();
	virtual void Function2();
	virtual void Function3();
	virtual void Function4();
	virtual void Function5();
	virtual void Function6();
	virtual void Function7();
	virtual void Function8();
	virtual void Function9();
	virtual void Function10();
	virtual uint32_t GetId(UINT_PTR* itemPtr_1);
	virtual void Function12();
	virtual uint32_t GetQuantity();
}; //Size: 0x0440

class StaticObjCore
{
public:
	char pad_0000[32]; //0x0000
	uint32_t Id; //0x0020
	char pad_0024[60]; //0x0024
	char* Op0; //0x0060
	char pad_0068[56]; //0x0068
	char* Op1; //0x00A0
	char pad_00A8[56]; //0x00A8
	char* Op2; //0x00E0
	char pad_00E8[56]; //0x00E8
	char* Op3; //0x0120
	char pad_0128[56]; //0x0128
	char* Op4; //0x0160
	char pad_0168[56]; //0x0168
	char* Op5; //0x01A0
	char pad_01A8[56]; //0x01A8
	char* Name; //0x01E0
	char pad_01E8[80]; //0x01E8
}; //Size: 0x0238

class StaticObjEX
{
public:
	StaticObjEX()
	{
		Definition = 0;
	};

	// Can't really use this for any
	StaticObjEX(uint32_t id, int32_t x, int32_t y)
	{
		SecondId = id;
		TileX = x;
		TileY = y;
	};

	EntityType Type; //0x0040
	int32_t TileX; //0x00EC
	int32_t TileY; //0x00F0
	int32_t SecondId = 0;
	StaticObjCore* Definition; //0x0100
};

class StaticObj2Wrapper
{
public:
	char pad_0000[64]; //0x0000
	uint32_t Type; //0x0040
	char pad_0044[168]; //0x0044
	int32_t TileX; //0x00EC
	int32_t TileY; //0x00F0
	char pad_00F4[4]; //0x00F4
	void* DefaultInteraction; //0x00F8
	class StaticObjCore* Definition; //0x0100
	char pad_0108[58]; //0x0108
	int8_t IsHarvested; //0x0142
	char pad_0143[5]; //0x0143
}; //Size: 0x0148

class StaticObj1Wrapper
{
public:
	char pad_0000[248]; //0x0000
	uint32_t SecondaryId; //0x00F8
	char pad_00FC[28]; //0x00FC
	class StaticObjCore* Definition; //0x0118
	char pad_0120[76]; //0x0120
	uint32_t TileX; //0x016C
	uint32_t TileY; //0x0170
	char pad_0174[28]; //0x0174
}; //Size: 0x0190

class TileList
{
public:
	char pad_0000[48]; //0x0000
	class TileInfo* LastTile; //0x0030
	class TileInfo* NextTile; //0x0038
	char pad_0040[16]; //0x0040
	uint32_t LootTileCount; //0x0050
}; //Size: 0x0054

class TileInfo
{
public:
	char pad_0000[32]; //0x0000
	int32_t Plane; //0x0020
	int32_t TileX; //0x0024
	int32_t TileY; //0x0028
	char pad_002C[12]; //0x002C
	class LootInfo* LootInfo; //0x0038
}; //Size: 0x0040

class DialogInfoEx
{
public:
	char pad_0000[12]; //0x0000
	int32_t SecondaryId; //0x000C
	int32_t WidgetId; //0x0010
}; //Size: 0x0014

class DialogInfo
{
public:
	uint32_t DialogFullId; //0x0000
	char pad_0004[12]; //0x0004
	class DialogInfoEx* dialogInfoEx; //0x0010
}; //Size: 0x0108

class LootInfo
{
public:
	char pad_0000[232]; //0x0000
	class FakeItem* LootArray; //0x00E8
	void* LastLootPtr; //0x00F0
	char pad_00F8[16]; //0x00F8
	class FakeItem* VisibleLootArray; //0x0108
	void* VisibleLastLootPtr; //0x0110
}; //Size: 0x0118

class Tile3D
{
public:
	Tile3D() :x(0), y(0), plane(0){};
	Tile3D(int32_t _plane, int32_t _x, int32_t _y) : plane(_plane), x(_x), y(_y) {};
	int32_t plane;
	int32_t x; //0x0000
	int32_t y; //0x0004
}; //Size: 0x0008

class StaticObj
{
public:
	StaticObj() { ItemId = -1; };
	StaticObj(uint32_t id, uint32_t _x, uint32_t _y)
	{
		ItemId = id;
		Pos.x = _x;
		Pos.y = _y;
	};

	int ItemId; //0x0000
	Tile2D Pos;
};

class FakeItemEX
{
public:
	FakeItemEX() { ItemId = -1; };
	FakeItemEX(uint32_t id, uint32_t quantity, uint32_t _x, uint32_t _y, uint32_t plane)
	{
		ItemId = id;
		ItemQuantity = quantity;
		Pos.x = _x;
		Pos.y = _y;
		Pos.plane = plane;
	};

	int ItemId; //0x0000
	uint32_t ItemQuantity; //0x0004
	Tile3D Pos;
}; //Size: 0x0008

class WorldClass
{
public:
	char pad_0000[32]; //0x0000
	class CurrentWorld* CurrentWorld; //0x0020
	char pad_0028[32]; //0x0028
}; //Size: 0x0048

class CurrentWorld
{
public:
	char pad_0000[8]; //0x0000
	uint32_t CurrentWorld; //0x0008
	char pad_000C[4]; //0x000C
	char* WorldUrl; //0x0010
	char pad_0018[48]; //0x0018
}; //Size: 0x0048

class LoginScriptInstance
{
public:
	char pad_0000[8]; //0x0000
}; //Size: 0x0008

class LoginManager
{
public:
	char pad_0000[88]; //0x0000
	class Subconnection* Subconnection; //0x0058
	char pad_0060[80]; //0x0060
	char* Email; //0x00B0
	char pad_00B8[24]; //0x00B8
	char* Password; //0x00D0
	char pad_00D8[48]; //0x00D8
}; //Size: 0x0108


class Subconnection
{
public:
	char pad_0000[4400]; //0x0000
	class ConnectionTime* Time; //0x1130
	char pad_1138[720]; //0x1138
}; //Size: 0x1408

class ConnectionTime
{
public:
	char pad_0000[32]; //0x0000
	void* SubTime; //0x0020
	char pad_0028[32]; //0x0028
}; //Size: 0x0048

struct ActionMirror
{
	ActionMirror(uint32_t i, uint32_t p1, uint32_t p2, uint32_t p3, uint64_t addr)
	{
		index = i; param1 = p1; param2 = p2; param3 = p3; function = addr;
	}
	uint32_t index;
	uint32_t param1;
	uint32_t param2;
	uint32_t param3;
	uint64_t function;
};

struct AgilityCourse
{
	AgilityCourse() {};

	AgilityCourse(uint32_t _objId)
	{
		objId = _objId;
	}

	AgilityCourse(uint32_t _objId, Tile2D end) {
		objId = _objId;
		EndPos = end;
	}
	uint32_t objId;
	Tile2D EndPos;
};

struct mouse_biometric
{
	mouse_biometric(UINT m, UINT w, UINT l, UINT d)
	{
		uMsg = m;
		wParm = w;
		lParm = l;
		delay = d;
	}

	UINT uMsg;
	UINT wParm;
	UINT lParm;
	UINT delay;
};

struct mouse_data_header
{
	mouse_data_header(uint32_t h, uint32_t w, size_t e)
	{
		height = h;
		width = w;
		elements = e;
	};

	uint32_t height;
	uint32_t width;
	size_t elements;
};

struct mouse_replay
{
	mouse_data_header header;
	mouse_biometric points[]; // this is actually an array of points
};

enum class ContainerType : uint32_t {
	Trade = 90,
	Backpack = 93,
	Equipment = 94,
	Bank = 95,
	Familiar = 530,
	ShopSell = 696969,  //There is no sell i think?
	ShopBuy = 628,
	ShopBuyFree = 629,
	AreaLoot = 773,
	CoinPouch = 623,
	DeathReclaim = 676,
};

enum class RSDialog
{
	Unknown,
	PlayerDialog,
	NPCDialog,
	MultiSelectionDialog,
	GameDialog,
	ModernDestroy
};

enum class UIType
{
	UNKNOWN,
	DEPOSIT_BOX,
	TELEPORT_OPTIONS,
	SHOP_STORE,
	OBELISK_WIDGET
};

enum class _game_state {
	LoginScreen = 10,
	Lobby = 20,
	Ingame = 30,
	Disconnected = 40,
	Reconnecting = 35,
	PleaseWait = 37,
	Unknown = 666
};

enum class _current_bot
{
	None,
	Penguin_Agility,
	Anachronia_Agility,
	Abyss_Crafting,
	Archeology,
	Watchtower_Agility,
	Wilderness_Agility,
	Divination,
	Fungal_Mage,
	Taverly_Summoning,
	Money_Drop,
	Wisp_Farming,
	General_Combat,
	Rabbit_Farming,
	Woodcutting,
	General_Mining
};


class eastlString
{
public:
	eastlString(const char * text)
	{
		start = new char[256]();
		strcpy((char*)start, text);

		end = start + strlen(text);
	}
	eastlString()
	{
		start = new char[1];
		end = start;
	}
	const char* start;
	const char* end;
};

#pragma pack(push)
struct dataStruct {
	uint64_t unk1;
	uint8_t* dataPtr;
};
#pragma pack(pop)

typedef bool(__stdcall* fn_wglSwapBuffers) (_In_ HDC hDc);
typedef UINT_PTR* (__fastcall* fn_GetLocalPlayer)(UINT_PTR* PlayerEntityList);
typedef DWORD* (__fastcall* fn_OnCursorDoAction) (UINT_PTR a1, ActionPtr actionPtr, float* postion);
typedef UINT_PTR(__fastcall* fn_CursorWorldContextMenu) (UINT_PTR* GameContext, int a2, int a3, int a4, int a5);
typedef int* (__fastcall* fn_ExecuteHookInner)(UINT_PTR* GameContext, unsigned int* a2, int a3);
typedef bool(__fastcall* fn_OnDispatchNetMessage)(UINT_PTR* a1, UINT_PTR* a2);
typedef float*(__fastcall* fn_GetWOrldTranslation)(UINT_PTR* camera);
typedef void(__fastcall* fn_GUIManagerRender)(UINT_PTR* a1);
typedef UINT_PTR*(__fastcall* fn_GetContainerPtr)(UINT_PTR* ContainerManager, uint32_t containerId, uint8_t idk);

typedef char* (__fastcall* fn_CopyString)(UINT_PTR string, int a2, int a3);
typedef void(__fastcall* fn_StartLogin)(__int64 a1, int a2, UINT_PTR a3, eastlString a4, eastlString a5, eastlString a6, char a7, char a8, char a9, char a10, UINT_PTR a11);
typedef void(__fastcall* fn_RequestLoginToGameFromLogin)(__int64 ConnectionManager, eastlString username, eastlString password, eastlString empty, char a5);

typedef __int64* (__fastcall* fn_PrepareUUIDPacket)(__int64* uuid_struct, __int64 packet);

typedef __int64* (__fastcall* fn_GetVarpType)(__int64 VarpWrap, __int64 VarpDomain, unsigned int varpid);
typedef __int64 (__fastcall* fn_SetVarpValueFromServer)(__int64 player, __int64 varptype, VarpClass* varpclass);

#define ReadPtrOffset(address, x) ((uint64_t)address == 0 || ((uint64_t)address % sizeof(uint64_t)) != 0) ? (0) : *(uint64_t*)((uint64_t)address + x)

#define BANK_GROUP 0x5c5
#define CONVERSATION_WIDGET 0x5C502B4
#define DEPOSIT_WIDGET 0x5C502A7
#define BANKING_WIDGET 0x5C5027D
#define CHANGE_WORLD_WIDGET 0x5C5029E
#define SELECT_AN_OPTION_TELEPORT_WIDGET 0x5C502A7
#define SHOP_WIDGET 0x5c502a7 //Secondary ID 0x4F1
#define SUMMONING_WIDGET 0x5C502A7 // Secondary ID 0x55a 

// Varps
#define SOIL_BOX_GRAVEL 9370
#define SOIL_BOX_FIERY 9372
#define SOIL_BOX_AERATED 9373

#define RUN_MODE 463
#define PRAYER_POINTS 3274
#define ABYSS_ENTERABLE 101
#define ADRENALINE 679
#define ANAGOGIC_ORTS 2006
#define AUTO_CASTING 0
#define CHARGE_PACK_COUNT 5984
#define DUNGEON_TOKENS 1097
#define ECTO_TOKENS 6533
#define FAMILIAR_SPECIAL_POINTS 1787
#define MTA_ALCHEMIST_POINTS 1347
#define MTA_ENCHANTMENT_POINTS 1348
#define MTA_GRAVEYARD_POINTS 1348
#define MTA_TELEKINETIC_POINTS 1347
#define RUN_ENABLED 463
#define RUNECRAFTING_POUCHES_FILLED 3214
#define RUNESPAN_POINTS 3220
#define SLAYER_MONSTER_CATEGORY 185
#define SLAYER_POINTS 2092
#define SLAYER_TASK_KILLS_LEFT 183
#define TOKKUL 6526
#define WARRIORS_GUILD_ATTACK_POINTS 3066
#define WARRIORS_GUILD_BALANCE_POINTS 3067
#define WARRIORS_GUILD_COMBAT_POINTS 3067
#define WARRIORS_GUILD_DEFENCE_POINTS 3066
#define WARRIORS_GUILD_STRENGTH_POINTS 3068
#define CHIMES 6528
#define AUTO_RETALIATE 462
#define CURRENT_SETTINGS_TAB 3709
#define EOC_COMBAT_MODE 3711
#define EOC_INTERFACE_MODE 3814
#define FOLLOWER_SETTINGS 1790
#define GAMEPLAY_SETTINGS_CATEGORY 5839
#define GRAND_EXCHANGE_ITEM_PRICE 137
#define GRAND_EXCHANGE_ITEM_QUANTITY 136
#define GRAND_EXCHANGE_OFFER_AMOUNT 136
#define GRAND_EXCHANGE_OFFER_ITEM 135
#define GRAND_EXCHANGE_OFFER_PRICE 137
#define GRAND_EXCHANGE_STATE 139
#define LOOT_INVENTORY 5413
#define PRIMARY_ACTION_BAR_NUMBER 682


// Potions
#define JUJU 4908
#define STRENGTH_POT 3531
#define SUPER_JUJU 4908
#define DEFENCE_POT 3563
#define ANTIFIRE 1299
#define ATTACK_POT 3561
#define AGGRESSION 33448

// BANK
#define MAX_BANK_SLOT 8968
#define CURRENT_BANKS_LOT 8971
#define CURRENT_BANK_TAB 45141
#define SLOT_BANK_TAB_1 45143


// UI
#define SHOP_TAB 303

/*
	SHIELD(22842), // Might also be offhand, not sure
	EXCALIBUR(35309),
	SUMMONING_FAMILIAR_PRESENCE(6051),
	SUMMONING_POINTS(41524),
	FAMILIAR_SPECIAL_POINTS(26474),
	ASPHYXIATE(2099),
	ASSAULT(2093),
	ATTACK_XP(33076),
	AUTO_CAST_SPELL(43),
	BOSS_HEALTH(28663),
	COMBAT_STYLE(7618),
	COMBUST(1933),
	CRYSTAL_MASK(29116),
	CURSES(16789),
	DEBILITATE(35400),
	DEFENCE_MAGIC_XP(33082),
	DEFLECT_MAGIC(16768),
	DEFLECT_MELEE(16770),
	DEFLECT_RANGE(16769),
	DESTROY(2075),
	DEVOTION(21023),
	DISMEMBER_ON_PLAYER(1668),
	DREADNIP(35312),
	METAMORPHOSIS(2081),
	SLAUGHTER(1913),
	PROTECT_MAGIC(16745),
	PROTECT_MELEE(16744),
	PROTECT_RANGE(16746),
	QUICK_PRAYER(5941),
	RANGED_XP(33079),
	RAPID_FIRE(2096),
	REFLECT(2067),
	RESONANCE(2065),
	REVOLUTION_SLOTS_USED(38639),
	SLAYER_MASTER(9072),
	SLAYER_TASK_AMOUNT(521),
	SLAYER_TASK_ID(7219),
	SLAYER_TASK_KILLS_DONE(516),
	SNIPE(2095),
	STRENGTH_XP(33077),
	SOUL_SPLIT(16779),
	MAGIC_COMBAT_XP(1908),
	MAGIC_XP(33081),
	LOCAL_HEALTH(1668),
	FINISHING_BLOW_UNLOCKED(9085),
	FREEDOM(2063),
	VULNERABILITY(1939),

	//prayers


	//potions
	ANTIFIRE(497),
	SUPER_ANTIFIRE(498),
	SUPER_PRAYER_RENEWAL(25852),
	AGGRESSION(33448),
	WEAPON_POISON(2102),
	PRAYER_RENEWAL(25852),
	OVERLOAD(500),
	PERFECT_AGILITY_JUJU(25857),
	PERFECT_MINING_JUJU(25855),
	PERFECT_PLUS(26026),

	//reputation/points/score/currency
	MEMORY_STRANDS(34807),
	ZAMORAK_REPUTATION(30861),
	ZAROS_REPUTATION(30856),
	SLISKE_REPUTATION(30860),
	SEREN_REPUTATION(30864),
	PEST_CONTROL_REWARD_POINTS(4861),

	//other
	ACTION_BAR_LOCKED(1892),
	ABILITY_COOLDOWN_TEXT_ON(25401),
	AREA_LOOT(32132),
	ARTISANS_WORKSHOP_ADAMANT(366),
	ARTISANS_WORKSHOP_COAL(369),
	ARTISANS_WORKSHOP_CURRENT_ITEM(28416),
	ARTISANS_WORKSHOP_IRON(361),
	ARTISANS_WORKSHOP_MITHRIL(363),
	ARTISANS_WORKSHOP_RUNE(367),
	AUTO_TELEPORT_TO_URNS(30673),
	BUILDING_MODE_RS3(1537),
	CAMERA_ZOOM_LOCKED(19926),

	FAIRY_RING_LEFT_DIAL(12072),
	FAIRY_RING_MIDDLE_DIAL(12073),
	FAIRY_RING_RIGHT_DIAL(12074),
	GAMEPLAY_SETTINGS_OPEN_WINDOW(29043),
	GRIM_REAPER(28112),
	INFINITY_ETHEREAL_OUTFIT(25475),
	INSIDE_BOSS_INSTANCE(204),
	LEGACY_MODE(21682),
	LIGHT_FORM(29066),
	LOOT_INVENTORY(27942),
	PEST_CONTROL_DAMAGE_DEALT(4863),
	PLAYER_ATTACK_OPTION(35185),
	PRIFDDINAS_AGILITY_OBSTACLE(25926),
	PRIFDDINAS_AGILITY_REWARDS(25930),
	RUNECRAFTING_POUCH_GIANT(16500),
	RUNECRAFTING_POUCH_LARGE(16499),
	RUNECRAFTING_POUCH_MEDIUM(16498),
	RUNECRAFTING_POUCH_SMALL(16497),
	SETTINGS_TAB_TOP_MENU(19001),

*/