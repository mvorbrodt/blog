#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <arpa/inet.h>
#include "lsf.hpp"

// noisy test type, to see if we're not creating unnececary instances...
struct N
{
	N(int v = -1) : m_v{ v } { std::cout << "N(" << v << ")\n"; }
	N(const N& o) : m_v{ o.m_v } { std::cout << "N(const N&) " << m_v << "\n"; }
	N(N&& o) : m_v{ o.m_v } { std::cout << "N(N&&) " << m_v << "\n"; }
	~N() { std::cout << "~N(" << m_v << ")\n"; }
	N& operator = (N o) {
		std::cout << "operator=(N) " << m_v << " = " << o.m_v << "\n";
		std::swap(m_v, o.m_v); return *this; }

private:
	int m_v;
};

int main()
{
	add_pack_transform<std::uint16_t>([](std::uint16_t v, buffer_output_t& it) { pack_value(it, htons(v)); });
	add_pack_transform<std::uint32_t>([](std::uint32_t v, buffer_output_t& it) { pack_value(it, htonl(v)); });
	add_pack_transform<std::uint64_t>([](std::uint64_t v, buffer_output_t& it) { pack_value(it, htonll(v)); });

	add_unpack_transform<std::uint16_t>([](buffer_input_t& it, std::uint16_t* v) { *v = ntohs(unpack_value<std::uint16_t>(it)); });
	add_unpack_transform<std::uint32_t>([](buffer_input_t& it, std::uint32_t* v) { *v = ntohl(unpack_value<std::uint32_t>(it)); });
	add_unpack_transform<std::uint64_t>([](buffer_input_t& it, std::uint64_t* v) { *v = ntohll(unpack_value<std::uint64_t>(it)); });

	auto buf_1 = pack(std::uint8_t(0x12), std::uint16_t(0x1234), std::uint32_t(0x12345678), std::uint64_t(0x1234567890ABCDEF));
	auto tup_1 = unpack<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>(buf_1);



	add_pack_transform<std::size_t>([](std::size_t v, buffer_output_t& it) { pack_value(it, std::uint8_t(v)); });
	add_unpack_transform<std::size_t>([](buffer_input_t& it, std::size_t* v) { *v = unpack_value<std::uint8_t>(it); });
	add_pack_size_proc<std::size_t>([](std::size_t) { return sizeof(std::uint8_t); });

	auto buf_2_1 = pack(std::size_t(-1));
	auto tup_2_1 = unpack<std::size_t>(buf_2_1);

	remove_pack_transform<std::size_t>();
	remove_unpack_transform<std::size_t>();
	remove_pack_size_proc<std::size_t>();

	auto buf_2_2 = pack(std::size_t(-1));
	auto tup_2_2 = unpack<std::size_t>(buf_2_2);



	add_pack_transform<const char*>(
		[](const char* v, buffer_output_t& it)
		{
			auto len = std::strlen(v);
			pack_type(it, len);
			pack_bytes(it, v, len);
		});

	add_pack_transform<std::string>(
		[](const std::string& v, buffer_output_t& it)
		{
			pack_type(it, v.length());
			pack_bytes(it, v.data(), v.length());
		});

	add_unpack_transform<std::string>(
		[](buffer_input_t& it, std::string* v)
		{
			auto len = unpack_type<std::string::size_type>(it);
			new (v) std::string(len, std::string::value_type()); // do this cleanly !!!
			unpack_bytes(it, v->data(), len);
		});

	add_pack_size_proc<const char*>([](const char* v) { return pack_size(std::strlen(v)) + std::strlen(v); });
	add_pack_size_proc<std::string>([](const std::string& v) { return pack_size(v.length()) + v.length(); });

	auto ccp_1 = "this is hardcoded string #1";
	auto buf_3 = pack(ccp_1, "this is hardcoded string #2");
	auto tup_3 = unpack<std::string, std::string>(buf_3);

	auto str_1 = std::string("this is std::string #1");
	auto buf_4 = pack(str_1, std::string("this is std::string #2"));
	auto tup_4 = unpack<std::string, std::string>(buf_4);



	auto n_1 = N(11);
	auto buf_5 = pack(n_1, N(17));
	auto tup_5 = unpack<N, N>(buf_5);

	auto buf_6 = pack('C', '+', '+', 11, 14, 17, "this is hardcoded string", std::string("this is std::string"), N(20), N(23));
	auto tup_6 = unpack<char, char, char, int, int, int, std::string, std::string, N, N>(buf_6);



	using strings_t = std::vector<std::string>;

	add_pack_transform<strings_t>(
		[](const strings_t& vs, buffer_output_t& it)
		{
			pack_type(it, vs.size());
			std::for_each(std::begin(vs), std::end(vs),
				[&](const std::string& s) { pack_type(it, s); });
		});

	add_unpack_transform<strings_t>(
		[](buffer_input_t& it, strings_t* vs)
		{
			auto size = unpack_type<strings_t::size_type>(it);
			new (vs) strings_t();
			vs->reserve(size);
			std::generate_n(std::back_inserter(*vs), size,
				[&]() { return unpack_type<std::string>(it); });
		});

	auto vec_1 = strings_t{ "string 1", "string 2", "string 3" };
	auto buf_7 = pack(vec_1);
	auto tup_7 = unpack<strings_t>(buf_7);

	return rand();
}
