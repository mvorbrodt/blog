#include <iostream>
#include <boost/function.hpp>
#include <boost/dll/import.hpp>

using namespace std;
using namespace boost::dll;
using namespace boost::filesystem;

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		cerr << "USAGE: " << argv[0] << " [path to plugin.dll]" << endl;
		return 1;
	}

	path lib_path(argv[1]);

	typedef const char* (plugin_name_t)();
	typedef const char* (plugin_version_t)();

	auto plugin_name = import_alias<plugin_name_t>(lib_path, "plugin_name", load_mode::append_decorations);
	auto plugin_version = import_alias<plugin_version_t>(lib_path, "plugin_version", load_mode::append_decorations);

	cout << "Plugin name    : " << plugin_name() << endl;
	cout << "Plugin version : " << plugin_version() << endl;
}
