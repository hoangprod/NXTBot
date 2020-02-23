// Created with ReClass.NET 1.2 by KN4CK3R

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

class EntityObj
{
public:
	char pad_0008[48]; //0x0008
	void* camera; //0x0038
	char pad_0040[208]; //0x0040
	uint32_t EntityId; //0x0110
	char pad_0114[4]; //0x0114
	char* Name; //0x0118
	char pad_0120[808]; //0x0120

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
	virtual void Function12();
	virtual void Function13();
	virtual void Function14();
	virtual void Function15();
	virtual void Function16();
	virtual void Function17();
	virtual void Function18();
	virtual void Function19();
	virtual void Function20();
	virtual void Function21();
	virtual void Function22();
	virtual void Function23();
	virtual void Function24();
	virtual void Function25();
	virtual void Function26();
	virtual void Function27();
	virtual void Function28();
	virtual void Function29();
	virtual void Function30();
	virtual void Function31();
	virtual void Function32();
	virtual void Function33();
	virtual void Function34();
	virtual void Function35();
	virtual void Function36();
	virtual void Function37();
	virtual void Function38();
	virtual void Function39();
	virtual void Function40();
	virtual void Function41();
}; //Size: 0x0448

class GameContext
{
public:
	char pad_0000[4536]; //0x0000
	class EntityPtr* EntityPtr; //0x11B8
	char pad_11C0[24]; //0x11C0
	class PlayerListWrapper* PlayerListWrapper; //0x11D8
	char pad_11E0[24]; //0x11E0
}; //Size: 0x11F8

class EntityPtr
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
	char test[8]; //0x0000
	class Action* Action; //0x0008
}; //Size: 0x0010

class Action
{
public:
	char pad_0000[64]; //0x0000
	class Dispatcher* Dispatcher; //0x0040
	char pad_0048[8]; //0x0048
	uint32_t Index; //0x0050
	uint32_t TileX; //0x0054
	uint32_t TileY; //0x0058
}; //Size: 0x005C

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

class ContainerObj
{
public:
	char pad_0000[8]; //0x0000
	uint32_t ContainerId; //0x0008
	char pad_000C[4]; //0x000C
	void* ContainerContent; //0x0010
	char pad_0018[16]; //0x0018
}; //Size: 0x0028


enum class ContainerType : uint32_t {
	Backpack = 93,
	Bank = 95,
	CoinPouch = 623
};

enum EntityType {
	Object = 0,
	NPC = 1,
	Player = 2,
	GroundItem = 3,
	Object2 = 12
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

#define ReadPtrOffset(address, x) ((uint64_t)address == 0 || ((uint64_t)address % sizeof(uint64_t)) != 0) ? throw InvalidPointerException(std::to_string((uint64_t)address) + " in " __FUNCTION__ + ", " + __FILE__" at line " + std::to_string(__LINE__)) : *(uint64_t*)((uint64_t)address + x)