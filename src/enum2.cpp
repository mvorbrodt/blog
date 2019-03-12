#include <iostream>

#define MY_ENUM \
X(V1) \
X(V2) \
X(V3)

#define X(name) name,

enum MyEnum
{
	MY_ENUM
};

#undef X

constexpr const char* MyEnumToString(MyEnum e) noexcept
{
#define X(name) case(name): return #name;
	switch(e)
	{
			MY_ENUM
	}
#undef X
}

int main(int argc, char** argv)
{
	std::cout << MyEnumToString(V1) << std::endl;
	std::cout << MyEnumToString(V2) << std::endl;
	std::cout << MyEnumToString(V3) << std::endl;
	return 1;
}
