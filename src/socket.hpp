#include <string>
#include <vector>
#include <utility>
#include <functional>
#include <stdexcept>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <cstdint>
#include <cstddef>
#include <cstring>



class client_socket;
class server_socket;

using socket_buffer_t = std::vector<std::byte>;

struct host_info_t
{
	std::string   ip;
	std::string   host;
	std::uint16_t port;
};



class client_socket
{
public:
	using data_handler_t = std::function<void(client_socket&, socket_buffer_t)>;

	client_socket(const char* hostname, std::uint16_t port)
	: m_socket{ ::socket(AF_INET, SOCK_STREAM, 0) }
	{
		if(m_socket == -1)
			throw std::runtime_error("::socket call failed!");

		auto host = ::gethostbyname(hostname);
		if(host == nullptr)
			throw std::runtime_error("::gethostbyname call failed!");

		sockaddr_in addr = { sizeof(sockaddr_in), AF_INET, htons(port) };
		addr.sin_len = host->h_length;
		std::memcpy(&addr.sin_addr, host->h_addr_list[0], host->h_length);

		auto result = ::connect(m_socket, (const sockaddr*)&addr, sizeof(addr));
		if(result == -1)
			throw std::runtime_error("::connect call failed!");
	}

	client_socket(const client_socket&) = delete;

	client_socket(client_socket&& other)
	: m_socket{ std::exchange(other.m_socket, -1) }, m_handler{ std::exchange(other.m_handler, data_handler_t()) } {}

	~client_socket() { close(); }

	client_socket& operator = (client_socket other)
	{
		swap_with(other);
		return *this;
	}

	void set_data_handler(data_handler_t handler) { m_handler = std::move(handler); }

	void send(const void* data, std::size_t length) const
	{
		using ptr_t = socket_buffer_t::const_pointer;
		send(socket_buffer_t(ptr_t(data), ptr_t(data) + length));
	}

	void send(const socket_buffer_t& data) const
	{
		auto result = ::send(m_socket, data.data(), data.size(), 0);
		if(result != data.size())
			throw std::runtime_error("::send call failed!");
	}

	auto receive()
	{
		socket_buffer_t data(65535);
		auto result = ::recv(m_socket, data.data(), data.size(), 0);
		if(result <= 0)
			return false;

		m_handler(*this, socket_buffer_t(data.data(), data.data() + result));

		return true;
	}

	void close()
	{
		::shutdown(m_socket, SHUT_RDWR);
		::close(m_socket);
	}

private:
	friend class server_socket;

	client_socket(int socket) : m_socket{ socket } {}

	void swap_with(client_socket& s)
	{
		std::swap(m_socket, s.m_socket);
		std::swap(m_handler, s.m_handler);
	}

	int m_socket;
	data_handler_t m_handler;
};



class server_socket
{
public:
	using accept_handler_t = std::function<void(server_socket&, client_socket, host_info_t)>;

	server_socket(std::uint16_t port)
	: m_socket{ ::socket(AF_INET, SOCK_STREAM, 0) }, m_port{ port }
	{
		if(m_socket == -1)
			throw std::runtime_error("::socket call failed!");

		auto reuse = int(true);
		auto result = ::setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
		if(result == -1)
			throw std::runtime_error("::setsockopt call failed!");

		sockaddr_in addr = { sizeof(sockaddr_in), AF_INET, htons(port), INADDR_ANY };
		result = ::bind(m_socket, (const sockaddr*)&addr, sizeof(addr));
		if(result == -1)
			throw std::runtime_error("::bind call failed!");

		result = ::listen(m_socket, 1);
		if(result == -1)
			throw std::runtime_error("::listen call failed!");
	}

	server_socket(const server_socket&) = delete;

	server_socket(server_socket&& other)
	: m_socket{ std::exchange(other.m_socket, -1) }, m_port{ std::exchange(other.m_port, 0) }, m_handler{ std::exchange(other.m_handler, accept_handler_t()) } {}

	~server_socket() { close(); }

	server_socket& operator = (server_socket other)
	{
		swap_with(other);
		return *this;
	}

	void set_accept_handler(accept_handler_t handler) { m_handler = std::move(handler); }

	auto accept()
	{
		auto addr = sockaddr_in();
		auto len = socklen_t(sizeof(addr));
		auto s = ::accept(m_socket, (sockaddr*)&addr, &len);
		if(s == -1)
			return false;

		auto ip = ::inet_ntoa(addr.sin_addr);
		auto ent = ::gethostbyaddr(&addr.sin_addr, sizeof(addr.sin_addr), AF_INET);
		auto info = host_info_t{ ip, ent->h_name, ntohs(addr.sin_port) };

		m_handler(*this, client_socket(s), info);

		return true;
	}

	void close()
	{
		::shutdown(m_socket, SHUT_RDWR);
		::close(m_socket);
	}

private:
	void swap_with(server_socket& s)
	{
		std::swap(m_socket, s.m_socket);
		std::swap(m_port, s.m_port);
		std::swap(m_handler, s.m_handler);
	}

	int m_socket;
	std::uint16_t m_port;
	accept_handler_t m_handler;
};
