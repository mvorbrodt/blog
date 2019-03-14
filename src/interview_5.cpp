#include <iostream>

using namespace std;

void print_runs(const char* input)
{
	size_t offset = 0;
	size_t index = 0;
	
	while(char current = input[index])
	{
		char next = input[index + 1];
		if(next && next == current)
		{
			cout << " " << offset;
			while(input[index] && current == input[index]) ++index;
			offset = index;
		}
		else
		{
			offset = ++index;
		}
	}
}

int main(int argv, char** argc)
{
	const char* inputs[] =
	{
		"0000000000",
		"0101010101",
		"1111111111",
		"0110110110",
		"1110000011",
		"0010011010"
	};
	
	for(size_t index = 0; index < (sizeof(inputs) / sizeof(*inputs)); ++index)
	{
		cout << "Input: " << inputs[index] << endl;
		cout << "Runs :";
		print_runs(inputs[index]);
		cout << endl << endl;
	}
	
	return 1;
}
