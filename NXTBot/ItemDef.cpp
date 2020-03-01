#include "pch.h"
#include "GameClasses.h"
#include "Patterns.h"
#include "ItemDef.h"

extern Addr Patterns;

NpcDef::NpcDef(EntityObj* base)
{
	if (!base)
		return;

	UINT_PTR* optionBase = *(UINT_PTR**)((UINT_PTR)base + Patterns.Offset_OptionText);

	if (!optionBase)
		return;

	_base = optionBase;
}

bool NpcDef::HaveOption(const char* option)
{
	if (stored_options.size() == 0)
		if (options().size() == 0)
			return false;

	for (Option i : stored_options)
	{
		if (i.text == option)
			return true;
	}

	return false;
}

void NpcDef::SetBase(UINT_PTR* base)
{
	_base = base;
}

void NpcDef::printOptions()
{
	if (stored_options.size() == 0)
		if (options().size() == 0)
			return;

	for (Option i : stored_options)
	{
		printf("[%d] %s have option %s\n", i.index, name().data(), i.text.data());
	}
}

const uint32_t NpcDef::id()
{
	if (!_base)
		return 0;

	return *reinterpret_cast<uint32_t*>((UINT_PTR)_base + static_cast<int>(Offsets::id));
}

std::string NpcDef::name()
{
	if (!_base)
		return std::string();

	return std::string(*reinterpret_cast<const char**>((UINT_PTR)_base + static_cast<int>(Offsets::name)));
}

std::vector<NpcDef::Option> NpcDef::options()
{
	if(!_base)
		return std::vector<Option>();

	uint64_t opt_start = (UINT_PTR)_base + static_cast<int>(Offsets::options);

	size_t opt_size = 64;
	size_t opt_count = 6;

	uint64_t opt_end = opt_start + opt_count * opt_size;


	for (int i = 0; i < opt_count; ++i) {
		uint64_t opt = opt_start + i * opt_size;
		uint64_t text_start = *reinterpret_cast<uint64_t*>(opt);
		uint64_t text_end = *reinterpret_cast<uint64_t*>(opt + 8);
		size_t text_length = text_end - text_start;
		if (text_length > 0) {
			const char* text = (const char*)text_start;
			stored_options.push_back({ i, text });
		}
	}
	return stored_options;

}
