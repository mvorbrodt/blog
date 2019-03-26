#include <iostream>
#include <sstream>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

using namespace std;

int main()
{
	try
	{
		curlpp::Cleanup clean;

		curlpp::Easy r;
		std::ostringstream response;

		r.setOpt(new curlpp::options::Url("http://ip-api.com/json/162.241.253.105"));
		r.setOpt(new curlpp::options::WriteStream(&response));
		r.perform();

		cout << response.str() << endl;
	}
	catch(curlpp::LogicError& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch(curlpp::RuntimeError& e)
	{
		std::cout << e.what() << std::endl;
	}
}
