#pragma once

#include <Windows.h>
#include <map>

using namespace std;

class VMTHook
{
public:

	void** vmt = nullptr; // Pointer to the VMT, we're using it more as an array of void*

	VMTHook();
	VMTHook(void* vmt); // Hook original VMT by it's address
	~VMTHook(); // Destructor, removes all hooks

	void* Hook(int index, void* hk);
	void ClearHooks();
private:
	map<int, void*>::iterator func_iterator; // Iterator so we can iterate the map below
	map<int, void*> hooked_funcs; // std::map which holds the index hooked and the original function's address
};

inline VMTHook::VMTHook()
{
}

VMTHook::VMTHook(void* vmt_addy)
{
	vmt = (void**)vmt_addy;
}


void* VMTHook::Hook(int index, void* hk)
{
	// Store the index and original function address
	hooked_funcs.insert(make_pair(index, vmt[index]));

	// Change the memory's access rights, patch the address to our hook, restore original rights
	DWORD old;
	VirtualProtect(&vmt[index], sizeof(void*), PAGE_EXECUTE_READWRITE, &old);
	vmt[index] = hk;
	VirtualProtect(&vmt[index], sizeof(void*), old, &old);

	return hooked_funcs[index];
}

void VMTHook::ClearHooks()
{
	for (func_iterator = hooked_funcs.begin(); func_iterator != hooked_funcs.end(); func_iterator++)
	{
		DWORD old;
		VirtualProtect(&vmt[func_iterator->first], sizeof(void*), PAGE_EXECUTE_READWRITE, &old);
		vmt[func_iterator->first] = func_iterator->second;
		VirtualProtect(&vmt[func_iterator->first], sizeof(void*), old, &old);
	}
	hooked_funcs.clear();
	vmt = nullptr;
}

VMTHook::~VMTHook() {}
