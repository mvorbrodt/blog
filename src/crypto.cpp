#include <iostream>
#include <sstream>
#include <string>
#include <cryptopp/cryptlib.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>

#include <memory>
#include <utility>
#include <fstream>
#include <ios>
#include <stdexcept>
#include <vector>
#include <cstdint>
#include <botan/auto_rng.h>
#include <botan/rsa.h>
#include <botan/x509_key.h>
#include <botan/pkcs8.h>
#include <botan/pk_keys.h>
#include <botan/pubkey.h>

#include <botan/hash.h>
#include <botan/hex.h>
#include <wolfssl/wolfcrypt/sha.h>

class RSAKeyPair;
using RSAKeyPairPtr = std::unique_ptr<RSAKeyPair>;

class RSAKeyPair
{
public:
	static auto Create(std::size_t bits = 4096)
	{
		return RSAKeyPair(std::make_unique<Botan::RSA_PrivateKey>(kRNG, bits));
	}

	static void Save(const RSAKeyPair& key, const std::string& name, const std::string& pass)
	{
		auto text = Botan::PKCS8::PEM_encode(*key.m_key.get(), kRNG, pass);

		std::ofstream ofs;
		ofs.exceptions(std::ios::failbit | std::ios::badbit);
		ofs.open(name, std::ios::out | std::ios::trunc);
		ofs.write(text.c_str(), text.length());
		ofs.close();
	}

	static auto CreateAndSave(const std::string& name, const std::string& pass, std::size_t bits = 4096)
	{
		auto key = Create(bits);
		Save(key, name, pass);
		return std::move(key);
	}

	static auto Load(const std::string& file, const std::string& pass, bool check = true, bool strong = false)
	{
		auto key = std::unique_ptr<Botan::Private_Key>(Botan::PKCS8::load_key(file, kRNG, pass));
		if (check && !key->check_key(kRNG, strong))
			throw std::invalid_argument("Invalid key!");

		return RSAKeyPair(std::move(key));
	}

	auto Encrypt(const std::string& str) const
	{
		return m_enc->encrypt((const std::uint8_t*)str.data(), str.length(), kRNG);
	}

	auto Encrypt(const std::vector<std::byte>& data) const
	{
		return m_enc->encrypt((const std::uint8_t*)data.data(), data.size(), kRNG);
	}

private:
	RSAKeyPair(std::unique_ptr<Botan::Private_Key>&& key)
	: m_key(std::move(key)),
	m_enc(std::make_unique<Botan::PK_Encryptor_EME>(*m_key, kRNG, "EME1(SHA-256)")),
	m_dec(std::make_unique<Botan::PK_Decryptor_EME>(*m_key, kRNG, "EME1(SHA-256)")) {}

	std::unique_ptr<Botan::Private_Key> m_key;
	std::unique_ptr<Botan::PK_Encryptor_EME> m_enc;
	std::unique_ptr<Botan::PK_Decryptor_EME> m_dec;

	inline static Botan::AutoSeeded_RNG kRNG;
};



using namespace std;

string Hash_CryptoPP(const string& msg)
{
	CryptoPP::SHA1 hash;
	std::string digest(hash.DigestSize(), '*');
	stringstream output;

	hash.Update((const unsigned char*)msg.data(), msg.size());
	hash.Final((unsigned char*)&digest[0]);

	CryptoPP::HexEncoder encoder(new CryptoPP::FileSink(output));
	CryptoPP::StringSource(digest, true, new CryptoPP::Redirector(encoder));

	return output.str();
}

string Hash_WolfSSL(const string& msg)
{
	Sha sha;
	::byte shaSum[SHA_DIGEST_SIZE];
	stringstream output;

	wc_InitSha(&sha);
	wc_ShaUpdate(&sha, (::byte*)msg.data(), msg.length());
	wc_ShaFinal(&sha, shaSum);

	string digest(shaSum, shaSum + SHA_DIGEST_SIZE);
	CryptoPP::HexEncoder encoder(new CryptoPP::FileSink(output));
	CryptoPP::StringSource(digest, true, new CryptoPP::Redirector(encoder));

	return output.str();
}

string Hash_Botan(const string& msg)
{
	auto hash = Botan::HashFunction::create("SHA-1");
	hash->update((uint8_t*)msg.data(), msg.length());
	return Botan::hex_encode(hash->final());
}

int main()
{
	// RSAKeyPair::CreateAndSave("key.txt", "123", 1024);
	// auto key = RSAKeyPair::Load("key.txt", "123");
	// return 0;

	std::string msg = "Vorbrodt's C++ Blog @ https://vorbrodt.blog";

	cout << "Message: " << msg << endl;
	cout << "Digest : " << Hash_CryptoPP(msg) << endl << endl;

	cout << "Message: " << msg << endl;
	cout << "Digest : " << Hash_WolfSSL(msg) << endl << endl;

	cout << "Message: " << msg << endl;
	cout << "Digest : " << Hash_Botan(msg) << endl << endl;
}
