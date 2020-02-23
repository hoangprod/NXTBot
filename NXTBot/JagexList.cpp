#pragma once

#include "pch.h"
#include "GameClasses.h"
#include "Patterns.h"
#include "ItemDef.h"
#include "Hooks.h"
#include "JagexList.h"

extern Addr Patterns;
extern UINT_PTR* g_GameContext;

#pragma pack(push)
struct InventoryItem {
	int32_t id;
	uint32_t quantity;
};
#pragma pack(pop)

inline const uint64_t hash_64(const uint32_t in) {
	uint64_t p1 = (in & 0xFF) ^ 0xCBF29CE484222325;
	uint64_t p2 = (in & 0xFF00) ^ 0x100000001B3;
	uint64_t p3 = (in & 0xFF0000) ^ 0x100000001B3;
	uint64_t p4 = (in & 0xFF000000) ^ 0x100000001B3;
	return p1 * p2 * p3 * p4;
}


inline uint64_t GetInventoryList() {
	return *reinterpret_cast<uint64_t*>(*reinterpret_cast<uint64_t*>((uint64_t)g_GameContext + 8) + 0x1180);
}

inline uint64_t GetInventoryByType(const uint32_t type) {
	const uint32_t _type = type * 2;

	uint64_t list_ptr = GetInventoryList() + 8;
	//std::cout << "inventoryList: " << std::hex << inventoryList << std::endl;

	uint64_t array_ptr = *(UINT_PTR*)(list_ptr +  0x18);
	uint64_t tail = *(UINT_PTR*)(list_ptr + 8);

	uint64_t type_hash = hash_64(_type);
	uint64_t count = *(UINT_PTR*)(list_ptr + 0x30);

	uint64_t index = 2 * (type_hash & count);

	if (array_ptr == 0) {
		printf("arrayPtr in GetInventoryByType is null. Check inventory pointer.");
		return 0;
	}

	uint64_t first_ptr = *(UINT_PTR*)(array_ptr + (8 * index));
	uint64_t curr_ptr = first_ptr;


	uint32_t curr_type = 0;

	while (1) {
		curr_type = *reinterpret_cast<uint32_t*>(curr_ptr + 0x10);
		if (curr_type == _type) {
			break;
		}
		uint64_t next_ptr = *reinterpret_cast<uint64_t*>(curr_ptr);
		if (next_ptr == first_ptr) {
			break;
		}
		curr_ptr = next_ptr;
	}

	return curr_type == _type ? *(UINT_PTR*)(curr_ptr + 0x20) : 0;
}

inline const JagexList<InventoryItem> GetBackpack() {
	return JagexList<InventoryItem>(GetInventoryByType(static_cast<uint32_t>(93)));
}

inline const JagexList<InventoryItem> GetInventory(const uint32_t type) {
	return JagexList<InventoryItem>(GetInventoryByType(type));
}

void holyshit()
{
	auto arrayL  = *(UINT_PTR*)(*(UINT_PTR*)((UINT_PTR)g_GameContext + 8) + 0x1180);;
	printf("test = %p\n", arrayL);
}
