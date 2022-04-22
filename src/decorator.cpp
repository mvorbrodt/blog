#include <iostream>
#include <ostream>
#include <string>
#include <type_traits>

class IDocument
{
public:
	virtual void Save(std::ostream& os) const = 0;
};

class PdfDocument : public IDocument
{
public:
	virtual void Save(std::ostream& os) const override { os << "Saving Pdf..." << std::endl; }
};

class DocDocument : public IDocument
{
public:
	virtual void Save(std::ostream& os) const override { os << "Saving Doc..." << std::endl; }
};

class IDocumentDecorator : public IDocument
{
public:
	explicit IDocumentDecorator(IDocument* d) : doc{ d } {}
	IDocument* GetDecorated() const { return doc; }
private:
	IDocument* doc;
};

class CompressedDocumentDecorator : public IDocumentDecorator
{
public:
	CompressedDocumentDecorator(IDocument* d, const std::string& c) : IDocumentDecorator(d), comp{ c } {}
	virtual void Save(std::ostream& os) const override
	{
		os << "Creating '" << comp << "' compression output stream..." << std::endl;
		GetDecorated()->Save(os);
	}
private:
	std::string comp;
};

class EncryptedDocumentDecorator : public IDocumentDecorator
{
public:
	EncryptedDocumentDecorator(IDocument* d, const std::string& e) : IDocumentDecorator(d), enc{ e } {}
	virtual void Save(std::ostream& os) const override
	{
		os << "Creating '" << enc << "' encryption output stream..." << std::endl;
		GetDecorated()->Save(os);
	}
private:
	std::string enc;
};



class ISocket
{
public:
	virtual void Send(std::byte* data, size_t size) const = 0;
	virtual void Recv(std::byte* data, size_t size) const = 0;
};

class TCPSocket : public ISocket
{
public:
	TCPSocket(const std::string& a, unsigned short p) : addr{ a }, port{ p } {}
	virtual void Send(std::byte* data, size_t size) const override { std::cout << "Sending " << size << " bytes over TCP to " << addr << ":" << port << std::endl; }
	virtual void Recv(std::byte* data, size_t size) const override { std::cout << "Receiving " << size << " bytes over TCP from " << addr << ":" << port << std::endl; }
private:
	std::string addr;
	unsigned short port;
};

class UDPSocket : public ISocket
{
public:
	UDPSocket(const std::string& a, unsigned short p) : addr{ a }, port{ p } {}
	virtual void Send(std::byte* data, size_t size) const override { std::cout << "Sending " << size << " bytes over UDP to " << addr << ":" << port << std::endl; }
	virtual void Recv(std::byte* data, size_t size) const override { std::cout << "Receiving " << size << " bytes over UDP from " << addr << ":" << port << std::endl; }
private:
	std::string addr;
	unsigned short port;
};

class ISocketDecorator : public ISocket
{
public:
	explicit ISocketDecorator(ISocket* s) : sock{ s } {}
	ISocket* GetDecorated() const { return sock; }
private:
	ISocket* sock;
};

class CompressedSocketDecorator : public ISocketDecorator
{
public:
	CompressedSocketDecorator(ISocket* s, const std::string& c) : ISocketDecorator(s), comp{ c } {}
	virtual void Send(std::byte* data, size_t size) const override
	{
		std::cout << "Compressing " << size << " bytes using '" << comp << "'..." << std::endl;
		GetDecorated()->Send(data, size / 2);
	}
	virtual void Recv(std::byte* data, size_t size) const override
	{
		GetDecorated()->Recv(data, size);
		std::cout << "Decompressing " << size << " bytes using '" << comp << "'..." << std::endl;
	}
private:
	std::string comp;
};

class EncryptedSocketDecorator : public ISocketDecorator
{
public:
	EncryptedSocketDecorator(ISocket* s, const std::string& e) : ISocketDecorator(s), enc{ e } {}
	virtual void Send(std::byte* data, size_t size) const override
	{
		std::cout << "Encrypting " << size << " bytes using '" << enc << "'..." << std::endl;
		GetDecorated()->Send(data, size);
	}
	virtual void Recv(std::byte* data, size_t size) const override
	{
		GetDecorated()->Recv(data, size);
		std::cout << "Decrypting " << size << " bytes using '" << enc << "'..." << std::endl;
	}
private:
	std::string enc;
};



template<typename Callable>
class DebugDecorator
{
public:
	DebugDecorator(const Callable& c, const std::string& m) : callable{ c }, message{ m } {}

	template<typename... Args>
	auto operator () (Args&&... args) const -> std::invoke_result_t<Callable, Args...>
	{
		std::cout << "Invoking: " << message << std::endl;

		auto constexpr returns = not std::is_same_v<void, std::invoke_result_t<Callable, Args...>>;
		if constexpr (returns) return callable(std::forward<Args>(args)...);
		else callable(std::forward<Args>(args)...);
	}

private:
	const Callable& callable;
	const std::string message;
};

template<typename Callable>
auto DecorateDebug(const Callable& callable, const std::string& message)
{
	return DebugDecorator(callable, message);
}



int main()
{
	using namespace std;

	IDocument* pdf = new PdfDocument;
	IDocument* doc = new DocDocument;
	pdf->Save(cout);
	doc->Save(cout);
	cout << endl;

	IDocument* cmp_pdf = new CompressedDocumentDecorator(pdf, "Zip");
	cmp_pdf->Save(cout);
	cout << endl;

	IDocument* enc_doc = new EncryptedDocumentDecorator(doc, "AES");
	enc_doc->Save(cout);
	cout << endl;

	IDocument* cmp_enc_pdf = new CompressedDocumentDecorator(new EncryptedDocumentDecorator(pdf, "RSA"), "TGS");
	cmp_enc_pdf->Save(cout);
	cout << endl;

	IDocument* cmp_enc_doc = new CompressedDocumentDecorator(new EncryptedDocumentDecorator(doc, "DES"), "LZ4");
	cmp_enc_doc->Save(cout);
	cout << endl;



	ISocket* tcp = new TCPSocket("1.1.1.1", 53);
	ISocket* udp = new TCPSocket("8.8.8.8", 53);
	tcp->Send(nullptr, 1024);
	udp->Recv(nullptr, 1024);
	cout << endl;

	ISocket* cmp_tcp = new CompressedSocketDecorator(tcp, "Zip");
	cmp_tcp->Send(nullptr, 2048);
	cmp_tcp->Recv(nullptr, 2048 / 2);
	cout << endl;

	ISocket* enc_udp = new EncryptedSocketDecorator(udp, "AES");
	enc_udp->Send(nullptr, 2048);
	enc_udp->Recv(nullptr, 2048);
	cout << endl;

	ISocket* cmp_enc_tcp = new CompressedSocketDecorator(new EncryptedSocketDecorator(tcp, "RSA"), "TGZ");
	cmp_enc_tcp->Send(nullptr, 4096);
	cmp_enc_tcp->Recv(nullptr, 4096 / 2);
	cout << endl;

	ISocket* cmp_enc_udp = new CompressedSocketDecorator(new EncryptedSocketDecorator(udp, "DES"), "LZ4");
	cmp_enc_udp->Send(nullptr, 4096);
	cmp_enc_udp->Recv(nullptr, 4096 / 2);
	cout << endl;



	auto debug1 = DecorateDebug([] () { cout << "Hi!" << endl; }, "Hello!");
	debug1();
	debug1();
	cout << endl;

	auto debug2 = DecorateDebug([] (int x, int y) { return x * y; }, "X * Y");
	auto result = debug2(2, 2);
	cout << result << endl;
	result = debug2(8, 8);
	cout << result << endl;
	cout << endl;
}
