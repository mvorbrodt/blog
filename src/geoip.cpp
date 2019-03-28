#include <iostream>
#include <sstream>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace boost::property_tree;

int main()
{
	try
	{
		curlpp::Cleanup clean;

		curlpp::Easy request;
		std::stringstream response;

		request.setOpt(new curlpp::options::Url("http://ip-api.com/json/vorbrodt.blog"));
		request.setOpt(new curlpp::options::WriteStream(&response));
		request.perform();

		ptree data;
		read_json(response, data);

		for(auto& it : data)
		{
			cout << it.first << " = " << it.second.data() << endl;
		}
	}
	catch(exception& e)
	{
		cerr << e.what() << endl;
	}
}
