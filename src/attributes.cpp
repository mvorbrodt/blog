[[nodiscard]] int no_discard(int i)
{
	switch(i)
	{
		case 1:
		[[fallthrough]];
		[[likely]] case 2: return i;
		default: return i;
	}
}

struct E {};

struct [[nodiscard]] S
{
	int i;
	[[no_unique_address]] E e;
};

S no_discard_struct()
{
	return {};
}

struct [[deprecated("Use Q instead")]] P {};

[[deprecated("Use bar() instead")]] void foo() {}

[[noreturn]] void never_returns()
{
	while(true);
}

int main()
{
	[[maybe_unused]] int unused = 1;
	[[maybe_unused]] P p;

	no_discard(0);
	no_discard_struct();
	foo();
	never_returns();
}
