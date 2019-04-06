#include <iostream>
#include <fstream>
#include <ios>
#include <vector>
#include <botan-2/botan/hash.h>
#include <botan-2/botan/hex.h>

using namespace std;

const int READ_SIZE = 1024 * 1024;

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		cerr << "USAGE: " << argv[0] << " <file name>" << endl;
		return -1;
	}

	try
	{
		using Buffer = vector<char>;
		Buffer buffer(READ_SIZE);

		auto hash_sha1 = Botan::HashFunction::create_or_throw("SHA-1");

		ifstream ifs;

		ifs.exceptions(ios::failbit | ios::badbit);
		ifs.open(argv[1]);
		ifs.seekg(0, ios_base::end);
		size_t bytes_left = ifs.tellg();
		ifs.seekg(ios_base::beg);

		while(bytes_left)
		{
			size_t bytes_to_read = bytes_left > READ_SIZE ? READ_SIZE : bytes_left;

			buffer.resize(bytes_to_read);
			ifs.read(buffer.data(), buffer.size());

			hash_sha1->update((uint8_t*)buffer.data(), buffer.size());

			bytes_left -= bytes_to_read;
		}

		ifs.close();

		cout << Botan::hex_encode(hash_sha1->final()) << " " << argv[1] << endl;
	}
	catch(exception& e)
	{
		cerr << e.what() << endl;
	}
}
