#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <utility>
#include <cstdint>
#include <arpa/inet.h>
#include "lsf.hpp"

// noisy type...
// ...to see if we're not creating unnececary instances...
// ...and moving instead of copying whenever possible...
struct N
{
	N(int v = -1) : m_v{ v } { std::cout << "N(" << v << ")\n"; }
	N(const N& o) : m_v{ o.m_v } { std::cout << "N(const N&) " << m_v << "\n"; }
	N(N&& o) : m_v{ std::exchange(o.m_v, -1) } { std::cout << "N(N&&) " << m_v << "\n"; }
	~N() { std::cout << "~N(" << m_v << ")\n"; }
	N& operator = (N o) {
		std::cout << "operator=(N) " << m_v << " = " << o.m_v << "\n";
		std::swap(m_v, o.m_v);
		return *this; }

	auto get_value() const { return m_v; }

private:
	int m_v;
};

int main()
{
	serializer lws;

	lws.add_pack_transform<std::uint16_t>([](const serializer& s, std::uint16_t v, buffer_output_t& it) { s.pack_value(it, htons(v)); });
	lws.add_pack_transform<std::uint32_t>([](const serializer& s, std::uint32_t v, buffer_output_t& it) { s.pack_value(it, htonl(v)); });
	lws.add_pack_transform<std::uint64_t>([](const serializer& s, std::uint64_t v, buffer_output_t& it) { s.pack_value(it, htonll(v)); });

	lws.add_unpack_transform<std::uint16_t>([](const serializer& s, buffer_input_t& it) { return ntohs(s.unpack_value<std::uint16_t>(it)); });
	lws.add_unpack_transform<std::uint32_t>([](const serializer& s, buffer_input_t& it) { return ntohl(s.unpack_value<std::uint32_t>(it)); });
	lws.add_unpack_transform<std::uint64_t>([](const serializer& s, buffer_input_t& it) { return ntohll(s.unpack_value<std::uint64_t>(it)); });

	auto buf_1 = lws.pack(std::uint8_t(0x12), std::uint16_t(0x1234), std::uint32_t(0x12345678), std::uint64_t(0x1234567890ABCDEF));
	auto tup_1 = lws.unpack<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>(buf_1);



	lws.add_pack_transform<std::size_t>([](const serializer& s, std::size_t v, buffer_output_t& it) { s.pack_value(it, std::uint16_t(v)); });
	lws.add_unpack_transform<std::size_t>([](const serializer& s, buffer_input_t& it) { return s.unpack_value<std::uint16_t>(it); });
	lws.add_pack_size_proc<std::size_t>([](const serializer&, std::size_t) { return sizeof(std::uint16_t); });

	auto buf_2_1 = lws.pack(std::size_t(-1));
	auto tup_2_1 = lws.unpack<std::size_t>(buf_2_1);

	lws.remove_pack_transform<std::size_t>();
	lws.remove_unpack_transform<std::size_t>();
	lws.remove_pack_size_proc<std::size_t>();

	auto buf_2_2 = lws.pack(std::size_t(-1));
	auto tup_2_2 = lws.unpack<std::size_t>(buf_2_2);

	lws.add_pack_transform<std::size_t>([](const serializer& s, std::size_t v, buffer_output_t& it) { s.pack_value(it, std::uint16_t(v)); });
	lws.add_unpack_transform<std::size_t>([](const serializer& s, buffer_input_t& it) { return s.unpack_value<std::uint16_t>(it); });
	lws.add_pack_size_proc<std::size_t>([](const serializer&, std::size_t) { return sizeof(std::uint16_t); });



	lws.add_pack_transform<const char*>(
		[](const serializer& s, const char* v, buffer_output_t& it)
		{
			auto len = std::strlen(v);
			s.pack_type(it, len);
			s.pack_bytes(it, v, len);
		});

	lws.add_pack_transform<std::string>(
		[](const serializer& s, const std::string& v, buffer_output_t& it)
		{
			s.pack_type(it, v.length());
			s.pack_bytes(it, v.data(), v.length());
		});

	lws.add_unpack_transform<std::string>(
		[](const serializer& s, buffer_input_t& it)
		{
			using it_t = byte_cast_buffer_input_t<char>;
			auto len = s.unpack_type<std::string::size_type>(it);
			return std::string(it_t(it), it_t(it + len));
		});

	lws.add_pack_size_proc<const char*>([](const serializer& s, const char* v) { return s.pack_size(std::strlen(v)) + std::strlen(v); });
	lws.add_pack_size_proc<std::string>([](const serializer& s, const std::string& v) { return s.pack_size(v.length()) + v.length(); });

	auto ccp_1 = "this is hardcoded string #1";
	auto buf_3 = lws.pack(ccp_1, "this is hardcoded string #2");
	auto tup_3 = lws.unpack<std::string, std::string>(buf_3);

	auto str_1 = std::string("this is std::string #1");
	auto buf_4 = lws.pack(str_1, std::string("this is std::string #2"));
	auto tup_4 = lws.unpack<std::string, std::string>(buf_4);



	lws.add_pack_transform<int>([](const serializer& s, int v, buffer_output_t& it) { s.pack_value(it, htonl(v)); });
	lws.add_unpack_transform<int>([](const serializer& s, buffer_input_t& it) { return ntohl(s.unpack_value<decltype(htonl(0))>(it)); });
	lws.add_pack_size_proc<int>([](const serializer&, int) { return sizeof(decltype(htonl(0))); });

	lws.add_pack_transform<N>([](const serializer& s, const N& v, buffer_output_t& it) { s.pack_type(it, v.get_value()); });
	lws.add_unpack_transform<N>([](const serializer& s, buffer_input_t& it) { return N(s.unpack_type<int>(it)); });
	lws.add_pack_size_proc<N>([](const serializer& s, const N& v) { return s.pack_size(v.get_value()); });

	auto n_1 = N(11);
	auto buf_5 = lws.pack(n_1, N(17));
	auto tup_5 = lws.unpack<N, N>(buf_5);

	auto buf_6 = lws.pack('C', '+', '+', 11, 14, 17, "this is hardcoded string", std::string("this is std::string"), N(20), N(23));
	auto tup_6 = lws.unpack<char, char, char, int, int, int, std::string, std::string, N, N>(buf_6);



	using strings_t = std::vector<std::string>;

	lws.add_pack_transform<strings_t>(
		[](const serializer& s, const strings_t& vs, buffer_output_t& it)
		{
			s.pack_type(it, vs.size());
			std::for_each(std::begin(vs), std::end(vs),
				[&](const std::string& str) { s.pack_type(it, str); });
		});

	lws.add_unpack_transform<strings_t>(
		[](const serializer& s, buffer_input_t& it)
		{
			auto size = s.unpack_type<strings_t::size_type>(it);
			auto vs = strings_t();
			vs.reserve(size);
			std::generate_n(std::back_inserter(vs), size,
				[&]() { return s.unpack_type<std::string>(it); });
			return vs;
		});

	lws.add_pack_size_proc<strings_t>(
		[](const serializer& s, const strings_t& v)
		{
			return s.pack_size(v.size()) +
				std::accumulate(std::begin(v), std::end(v), 0,
					[&](auto sum, const std::string& v) { return sum + s.pack_size(v); });
		});

	auto vec_1 = strings_t{ "string 1", "string 2", "string 3" };
	auto buf_7 = lws.pack(vec_1);
	auto tup_7 = lws.unpack<strings_t>(buf_7);

	return rand();
}
