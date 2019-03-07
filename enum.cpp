#include <iostream>
#include <array>

enum class MyEnum : int
{
	V1,
	V2,
	V3,
	SIZE
};

const char* MyEnumToString(MyEnum e)
{
	using MapType = std::array<const char*, (size_t)MyEnum::SIZE>;
	static const MapType kMap =
	{
		"V1",
		"V2",
		"V3"
	};
	static_assert(kMap.size() == (size_t)MyEnum::SIZE);
	return kMap.at((MapType::size_type)e);
}

int main(int argc, char** argv)
{
	std::cout << MyEnumToString(MyEnum::V1) << std::endl;
	std::cout << MyEnumToString(MyEnum::V2) << std::endl;
	std::cout << MyEnumToString(MyEnum::V3) << std::endl;
	return 1;
}
