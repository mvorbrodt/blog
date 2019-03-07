#include <iostream>
#include <limits>
#include <bitset>
#include <cstdint>

using namespace std;

int main(int argc, char** argv)
{
	using type = uint8_t;
	
	for(type number = 0; number < numeric_limits<type>::max(); ++number)
	{
		bool print = true;
		
		for(type shift = 0; shift <= numeric_limits<type>::digits - 2; ++shift)
		{
			static type mask = 0b11;
			if((number & (mask << shift)) == (mask << shift))
			{
				print = false;
				break;
			}
		}
		
		if(print)
			cout << bitset<numeric_limits<type>::digits>(number) << endl;
	}
	
	return 1;
}
