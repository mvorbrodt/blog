#include <iostream>
#include <string>
#include "lsf.hpp"

// noisy test type, to see if we're not creating unnececary instances...
struct N
{
	N(int v) : m_v{ v } { std::cout << "N(" << v << ")\n"; }
	N(const N& o) : m_v{ o.m_v } { std::cout << "N(const N&) " << m_v << "\n"; }
	N(N&& o) : m_v{ o.m_v } { std::cout << "N(NN&) " << m_v << "\n"; }
	~N() { std::cout << "~N(" << m_v << ")\n"; }
	N& operator = (const N& o) { std::cout << "operator=(const N&) " << m_v << ", " << o.m_v << "\n"; return *this; }
	N& operator = (N&& o) { std::cout << "operator=(N&&) " << m_v << ", " << o.m_v << "\n"; return *this; }

private:
	int m_v;
};

int main()
{
	using namespace std;

	auto buf_1 = pack(std::size_t(20));
	auto tup_1 = unpack<std::size_t>(buf_1);

	auto buf_2 = pack("this is a hardcoded string");
	auto tup_2 = unpack<std::string>(buf_2);

	auto buf_3 = pack("this is std::string"s);
	auto tup_3 = unpack<std::string>(buf_3);

	auto buf_4 = pack('C', '+', '+', 11, 14, 17, "this is a hardcoded string", "this is std::string"s, N(20), N(23));
	auto tup_4 = unpack<char, char, char, int, int, int, std::string, std::string, N, N>(buf_4);

	return rand();
}
