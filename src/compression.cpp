#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <lz4.h>

using namespace std;

using buffer = vector<char>;

void lz4_compress(const buffer& in, buffer& out)
{
	auto rv = LZ4_compress_default(in.data(), out.data(), in.size(), out.size());
	if(rv < 1) cerr << "Something went wrong!" << endl;
	else out.resize(rv);
}

void lz4_decompress(const buffer& in, buffer& out)
{
	auto rv = LZ4_decompress_safe(in.data(), out.data(), in.size(), out.size());
	if(rv < 1) cerr << "Something went wrong!" << endl;
	else out.resize(rv);
}

int main(int argc, char** argv)
{
	buffer data(1000, 'X');
	buffer compressed(data.size()), decompressed(data.size());

	lz4_compress(data, compressed);
	cout << "LZ4 compress, bytes in: " << data.size() << ", bytes out: " << compressed.size() << endl;

	lz4_decompress(compressed, decompressed);
	cout << "LZ4 decompress, bytes in: " << compressed.size() << ", bytes out: " << decompressed.size() << endl;

	if(data != decompressed) cerr << "Oh snap! Data mismatch!" << endl;
	else cout << "Decompressed data matches original :o)" << endl;

	return 1;
}
