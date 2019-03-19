#include <boost/dll/alias.hpp>

namespace plugin
{
	const char* plugin_name()
	{
		return "Vorbrodt's 1st Plugin";
	}

	const char* plugin_version()
	{
		return "1.0";
	}
}

BOOST_DLL_ALIAS(plugin::plugin_name, plugin_name);
BOOST_DLL_ALIAS(plugin::plugin_version, plugin_version);
