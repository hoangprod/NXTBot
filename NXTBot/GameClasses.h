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
	class GameContext* N0000016F; //0x0008
	char pad_0010[728]; //0x0010
	float worldX; //0x02E8
	char pad_02EC[4]; //0x02EC
	float WorldY; //0x02F0
	char pad_02F4[336]; //0x02F4
}; //Size: 0x0444

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
	char pad_0A70[1284]; //0x0A70
	uint32_t Level; //0x0F74
	char pad_0F78[1272]; //0x0F78


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

}; //Size: 0x0448

class GameContext
{
public:
	char pad_0000[4536]; //0x0000

	// NPC Related
	class NpcEntityPtr* EntityPtr; //0x11B8
	char pad_11C0[24]; //0x11C0
	class PlayerListWrapper* PlayerListWrapper; //0x11D8
	char pad_11E0[24]; //0x11E0
}; //Size: 0x11F8

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
	char pad_0000[440]; //0x0000
	class VarpWrap2* VarpWrap2; //0x01B8
	char pad_01C0[72]; //0x01C0
}; //Size: 0x0208

class VarpWrap2
{
public:
	char pad_0000[64]; //0x0000
	class VarpWrap3* VarpWrap3; //0x0040
}; //Size: 0x0048

class VarpWrap3
{
public:

	virtual void Function0();
	virtual void Function1();
	virtual void Function2();
	virtual void Function3();
	virtual void Function4();
	virtual void Function5();
	virtual void Function6();
	virtual void GetVarpWrap();
	virtual void Function8();
	virtual void Function9();
}; //Size: 0x0008

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
	virtual void Function14();
	virtual void Function15();
	virtual void Function16();
	virtual void Function17();
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
	int32_t SecondId;
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
	char pad_0000[264]; //0x0000
	class FakeItem* LootArray; //0x0108
	void* LastLootPtr; //0x0110
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

enum class ContainerType : uint32_t {
	Trade = 90,
	Backpack = 93,
	Equipment = 94,
	Bank = 95,
	Familiar = 530,
	ShopSell = 397, // 732
	ShopBuy = 508,
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
	GameDialog
};

enum class GameState {
	LoginScreen = 10,
	Lobby = 20,
	Ingame = 30,
	Disconnected = 40,
	Reconnecting = 35,
	PleaseWait = 37,
	Unknown = 666
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
typedef bool(__fastcall* fn_OnDispatchNetMessage)(UINT_PTR* a1, UINT_PTR* a2);
typedef float*(__fastcall* fn_GetWOrldTranslation)(UINT_PTR* camera);
typedef void(__fastcall* fn_GUIManagerRender)(UINT_PTR* a1);
typedef UINT_PTR*(__fastcall* fn_GetContainerPtr)(UINT_PTR* ContainerManager, uint32_t containerId, uint8_t idk);

typedef char* (__fastcall* fn_CopyString)(UINT_PTR string, int a2, int a3);

#define ReadPtrOffset(address, x) ((uint64_t)address == 0 || ((uint64_t)address % sizeof(uint64_t)) != 0) ? (0) : *(uint64_t*)((uint64_t)address + x)


#define BANK_GROUP 0x5c5
#define CONVERSATION_WIDGET 0x5C502B2
#define DEPOSIT_WIDGET 0x5C502A5
#define BANKING_WIDGET 0x5C5027B
