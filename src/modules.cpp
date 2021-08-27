#include <iostream>

import module_1;
import module_2;

int main()
{
	using namespace std;

	cout << global_module_1_name << ", " << module_1_name() << ", " << module_1::name() << ", " << module_1::S().name() << endl;
	cout << global_module_2_name << ", " << module_2_name() << ", " << module_2::name() << ", " << module_2::S().name() << endl;
}
