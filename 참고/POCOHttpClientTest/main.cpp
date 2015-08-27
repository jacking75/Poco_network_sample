#include <string>
#include <iostream>

#include "SimpleHTTPGetter.h"
#include "SimpleServerRequest.h"


int main()
{
	/*Poco::URI uri("http://poco.roundsquare.net/downloads/test.txt");
	{
		SimpleHTTPGetter getter(uri.getHost(), uri.getPort());
		getter.Get(uri.getPath(), std::cout);
	}*/

	SimpleServerRequest poster;
	auto result = poster.Start("http://localhost:20401/Service/TestRequestStream", 1000);
	std::cout << result << std::endl;
 
	return 0;
}