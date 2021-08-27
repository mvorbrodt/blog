export module module_1;

export auto global_module_1_name = "module_1";

export auto module_1_name() { return ::global_module_1_name; }

export namespace module_1
{
	auto name() { return ::module_1_name(); }

	struct S
	{
		auto name() { return ::module_1::name(); }
	};
}

module :private;
