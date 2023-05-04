#include <iostream>
#include <string>
#include <sstream>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		cerr << "USAGE: geoip [hostname/IP]..." << endl;
		return 1;
	}

	try
	{
		curlpp::Cleanup cURLppStartStop;

		for(int arg = 1; arg < argc; ++arg)
		{
			auto host = string(argv[arg]);

			curlpp::Easy request;
			std::stringstream response;

			request.setOpt(curlpp::options::Verbose(false));
			request.setOpt(curlpp::options::WriteStream(&response));
			request.setOpt(curlpp::options::Url("http://ip-api.com/json/"s + host));
			request.setOpt(curlpp::options::Port(80));

			request.perform();

			boost::property_tree::ptree data;
			boost::property_tree::read_json(response, data);

			cout << host << "\t";
			// cout << "host = " << host << endl;
			for(auto& it : data)
				cout << it.second.data() << "\t";
				// cout << it.first << " = " << it.second.data() << endl;
			cout << endl << endl;
		}
	}
	catch(exception& e)
	{
		cerr << e.what() << endl;
	}
}
