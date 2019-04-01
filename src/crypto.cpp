#include <iostream>
#include <sstream>
#include <string>
#include <cryptopp/cryptlib.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>
#include <wolfssl/wolfcrypt/sha.h>
#include <botan-2/botan/hash.h>
#include <botan-2/botan/hex.h>

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
	std::string msg = "Vorbrodt's C++ Blog @ https://vorbrodt.blog";

	cout << "Message: " << msg << endl;
	cout << "Digest : " << Hash_CryptoPP(msg) << endl << endl;

	cout << "Message: " << msg << endl;
	cout << "Digest : " << Hash_WolfSSL(msg) << endl << endl;

	cout << "Message: " << msg << endl;
	cout << "Digest : " << Hash_Botan(msg) << endl << endl;
}
