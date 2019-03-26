#include <iostream>
#include <sstream>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

using namespace std;

int main()
{
	curlpp::Cleanup clean;
	curlpp::Easy r;
	std::ostringstream response;

	r.setOpt(new curlpp::options::Url("http://ip-api.com/xml/8.8.4.4"));
	r.setOpt(new curlpp::options::WriteStream(&response));
	r.perform();

	cout << response.str() << endl;
}
