#include <iostream>
#include <ostream>
#include <string>
using namespace std;

class IDocument
{
public:
	virtual void Save(ostream& os) const = 0;
};

class PdfDocument : public IDocument
{
public:
	virtual void Save(ostream& os) const override { os << "Saving Pdf..." << endl; }
};

class DocDocument : public IDocument
{
public:
	virtual void Save(ostream& os) const override { os << "Saving Doc..." << endl; }
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
	CompressedDocumentDecorator(IDocument* d, const string& c) : IDocumentDecorator(d), comp{ c } {}
	virtual void Save(ostream& os) const override
	{
		os << "Creating '" << comp << "' compression output stream..." << endl;
		GetDecorated()->Save(os);
	}
private:
	string comp;
};

class EncryptedDocumentDecorator : public IDocumentDecorator
{
public:
	EncryptedDocumentDecorator(IDocument* d, const string& e) : IDocumentDecorator(d), enc{ e } {}
	virtual void Save(ostream& os) const override
	{
		os << "Creating '" << enc << "' encryption output stream..." << endl;
		GetDecorated()->Save(os);
	}
private:
	string enc;
};



class ISocket
{
public:
	virtual void Send(byte* data, size_t size) const = 0;
	virtual void Recv(byte* data, size_t size) const = 0;
};

class TCPSocket : public ISocket
{
public:
	TCPSocket(const string& a, unsigned short p) : addr{ a }, port{ p } {}
	virtual void Send(byte* data, size_t size) const override { cout << "Sending " << size << " bytes over TCP to " << addr << ":" << port << endl; }
	virtual void Recv(byte* data, size_t size) const override { cout << "Receiving " << size << " bytes over TCP from " << addr << ":" << port << endl; }
private:
	string addr;
	unsigned short port;
};

class UDPSocket : public ISocket
{
public:
	UDPSocket(const string& a, unsigned short p) : addr{ a }, port{ p } {}
	virtual void Send(byte* data, size_t size) const override { cout << "Sending " << size << " bytes over UDP to " << addr << ":" << port << endl; }
	virtual void Recv(byte* data, size_t size) const override { cout << "Receiving " << size << " bytes over UDP from " << addr << ":" << port << endl; }
private:
	string addr;
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
	CompressedSocketDecorator(ISocket* s, const string& c) : ISocketDecorator(s), comp{ c } {}
	virtual void Send(byte* data, size_t size) const override
	{
		cout << "Compressing " << size << " bytes using '" << comp << "'..." << endl;
		GetDecorated()->Send(data, size / 2);
	}
	virtual void Recv(byte* data, size_t size) const override
	{
		GetDecorated()->Recv(data, size);
		cout << "Decompressing " << size << " bytes using '" << comp << "'..." << endl;
	}
private:
	string comp;
};

class EncryptedSocketDecorator : public ISocketDecorator
{
public:
	EncryptedSocketDecorator(ISocket* s, const string& e) : ISocketDecorator(s), enc{ e } {}
	virtual void Send(byte* data, size_t size) const override
	{
		cout << "Encrypting " << size << " bytes using '" << enc << "'..." << endl;
		GetDecorated()->Send(data, size);
	}
	virtual void Recv(byte* data, size_t size) const override
	{
		GetDecorated()->Recv(data, size);
		cout << "Decrypting " << size << " bytes using '" << enc << "'..." << endl;
	}
private:
	string enc;
};



int main()
{
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
}
