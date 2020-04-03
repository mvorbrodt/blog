#include <iostream>

#define MY_ENUM \
X(V1, -1) \
X(V2, -3) \
X(V3, -5)

#define X(name, value) name = value,

#define MY_ENUM_NAME MyEnum
#define MY_ENUM_TYPE int

enum class MY_ENUM_NAME : MY_ENUM_TYPE
{
	MY_ENUM
};

#undef X

constexpr auto MyEnumToString(MY_ENUM_NAME e) noexcept
{
#define X(name, value) case(MY_ENUM_NAME::name): return #name;
	switch(e)
	{
		MY_ENUM
	}
#undef X
	return "UNKNOWN";
}

int main(int argc, char** argv)
{
	std::cout << "value = " << (MY_ENUM_TYPE)MyEnum::V1 << ", name = " << MyEnumToString(MY_ENUM_NAME::V1) << std::endl;
	std::cout << "value = " << (MY_ENUM_TYPE)MyEnum::V2 << ", name = " << MyEnumToString(MY_ENUM_NAME::V2) << std::endl;
	std::cout << "value = " << (MY_ENUM_TYPE)MyEnum::V3 << ", name = " << MyEnumToString(MY_ENUM_NAME::V3) << std::endl;

	MY_ENUM_NAME unknown{-42};
	std::cout << "value = " << (MY_ENUM_TYPE)unknown << ", name = " << MyEnumToString(unknown) << std::endl;

	return 1;
}
