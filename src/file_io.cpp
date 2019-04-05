#include <iostream>
#include <fstream>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

using namespace std;

const int SIZE = 1024 * 1024;
const int COUNT = 1024;
const char* NAME = "file.dat";

using Buffer = vector<char>;
Buffer buffer(SIZE);

TEST_CASE("File I/O", "[benchmark]")
{
	BENCHMARK("C")
	{
		int f = open(NAME, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		for(int i = 0; i < COUNT; ++i)
			write(f, buffer.data(), buffer.size());
		close(f);

		f = open(NAME, O_RDONLY);
		for(int i = 0; i < COUNT; ++i)
			read(f, buffer.data(), buffer.size());
		close(f);

		unlink(NAME);
	}

	BENCHMARK("CPP")
	{
		ofstream ofs;
		//ofs.rdbuf()->pubsetbuf(0, 0);
		ofs.open(NAME);
		for(int i = 0; i < COUNT; ++i)
			ofs.write(buffer.data(), buffer.size());
		ofs.close();

		ifstream ifs;
		//ifs.rdbuf()->pubsetbuf(0, 0);
		ifs.open(NAME);
		for(int i = 0; i < COUNT; ++i)
			ifs.read(buffer.data(), buffer.size());
		ifs.close();

		unlink(NAME);
	}
}
