class NpcDef
{
public:
	NpcDef(EntityObj* base = 0);

	struct Option {
		int index;
		std::string text;
	};

	std::vector<Option> stored_options;

	void SetBase(UINT_PTR* base);
	void printOptions();
	const uint32_t id();
	std::string name();
	std::vector<Option> options();
private:
	UINT_PTR* _base = 0;

	enum class Offsets {
		id = 0x20,
		name = 0x1b8,
		options = 0x38
	};
};