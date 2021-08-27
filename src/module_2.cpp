export module module_2;

export auto global_module_2_name = "module_2";

export auto module_2_name() { return ::global_module_2_name; }

export namespace module_2
{
	auto name() { return ::module_2_name(); }

	struct S
	{
		auto name() { return ::module_2::name(); }
	};
}

module :private;
