#include <string>
#include <iostream>

#include "SimpleHttpGetter.h"
#include "SimpleHttpPoster.h"


int main()
{
	/*Poco::URI uri("http://poco.roundsquare.net/downloads/test.txt");
	{
		SimpleHTTPGetter getter(uri.getHost(), uri.getPort());
		getter.Get(uri.getPath(), std::cout);
	}*/

	SimpleHttpPoster poster;
	auto result = poster.Start("http://localhost:20401/Service/TestRequestStream", 1000);
	std::cout << result << std::endl;
 
	return 0;
}