#include <iostream>
#include <vector>
#include <string>
#include <boost/program_options.hpp>

using namespace std;
using namespace boost::program_options;

int main(int argc, char** argv)
{
	int v{};
	float f{};
	string s{};
	vector<int> vi{};
	vector<string> vs{};

	options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("int,i", value<int>(&v)->default_value(42), "int value")
		("float,f", value<float>(&f)->default_value(3.141f), "float value")
		("string,s", value<string>(&s)->default_value("Vorbrodt"), "string value")
		("int_list,a", value<vector<int>>(&vi), "list of int values")
		("string_list,b", value<vector<string>>(&vs), "list of string values");

	variables_map vm{};
	store(parse_command_line(argc, argv, desc), vm);
	notify(vm);

	if (vm.size() == 0 || vm.count("help"))
	{
		cout << desc << "\n";
		return 1;
	}

	if(vm.count("int")) cout << "Int value was set to " << v << endl;
	if(vm.count("float")) cout << "Float value was set to " << f << endl;
	if(vm.count("string")) cout << "String value was set to \"" << s << "\"" << endl;
	if(vm.count("int_list"))
		for(auto it : vi)
			cout << "List of ints value was set to " << it << endl;
	if(vm.count("string_list"))
		for(auto& it : vs)
			cout << "List of strings value was set to \"" << it << "\"" << endl;

	return 1;
}
