#include <iostream>
#include <sstream>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;

int main()
{
	try
	{
		curlpp::Cleanup cURLppStartStop;

		curlpp::Easy request;
		std::stringstream response;

		request.setOpt(curlpp::options::Verbose(true));
		request.setOpt(curlpp::options::WriteStream(&response));
		request.setOpt(curlpp::options::Url("http://ip-api.com/json/vorbrodt.blog"));
		request.setOpt(curlpp::options::Port(80));

		request.perform();

		boost::property_tree::ptree data;
		boost::property_tree::read_json(response, data);

		for(auto& it : data)
			cout << it.first << " = " << it.second.data() << endl;
	}
	catch(exception& e)
	{
		cerr << e.what() << endl;
	}
}
