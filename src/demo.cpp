/*
 *    Permission is hereby granted, free of charge, to any person obtaining
 *    a copy of this software and associated documentation files, 
 *    to deal in the Software without restriction, 
 *    including without limitation the rights to use, copy, modify, merge,
 *    publish, distribute, sublicense, and/or sell copies of the Software,
 *    and to permit persons to whom the Software is furnished to do so, 
 *    subject to the following conditions:
 *    
 *    The above copyright notice and this permission notice shall be included
 *    in all copies or substantial portions of the Software.
 *    
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 *    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
 *    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
 *    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 *	DIFFBOT API DEMO
*/

#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <string>
#include <map>

#include "diffbot.h"

#ifdef _WIN32
#include "stdafx.h"
#endif

using namespace diffbot;

int main(int argc, char *argv[])
{
	// Process arguments
	if(argc < 2) {
		std::cerr << argv[0] << ": no arguments" << std::endl << "Usage: " << argv[0] << " <url> [<method>]" << std::endl;
		return EXIT_FAILURE;
	}
	char *url = argv[1]; 			// url to get HTML from
	char *method = (argc<3 ? 0 : argv[2]);	// API method to use, optional

	try {

	// STEP #1: prepare the request
		
		// Define our token
		const char *MY_DIFFBOT_TOKEN="391948cea2ff50f89877d258c0d1154d";

		// Create Diffbot instance
		Diffbot diffbot(MY_DIFFBOT_TOKEN);

		// Optional: set options
		diffbot.setTimeout(20);		// In seconds

		// Optional: set fields to receive
		//diffbot.setFields("meta");	// request meta information to be provided

		// Optional: set custom parameters
		//diffbot.setParameter("mode", "article");	// request meta information to be provided

		// Optional: set method; "article" is used by default
		diffbot.setMethod(method ? std::string(method) : "article");
		
	// STEP #2: send the request
		diffbot.ApiRequest(url);
		
		// Optional: get raw response as a string
		std::string rawResponse = diffbot.getRawResponse();

		// Mandatory: parse the response to JSON
		DiffbotResult res = diffbot.parseResponse();
		
	// STEP #3: iterate over the parsed response
		
		// Method #1: get a certain field
		std::cout << "TEXT:" << std::endl << res["text"] << std::endl;

		// Method #2: get JSON object - this is the recommended option
		// Please see http://jsoncpp.sourceforge.net/annotated.html for usage details of how to explore Json::Value objects
		Json::Value json = res.getJson();
		Json::Value title = json.get("title", "<no title>");
		std::cout << "JSON title: " << title.asString() << std::endl;

		// Method #3: get the list of all properties, except Arrays and Objects which should be accessed via Json
		std::cout << "ALL FIELDS:" << std::endl;
		std::map<std::string, std::string> vals = res.getAllFields();
		for (std::map<std::string, std::string>::iterator i=vals.begin(); i!=vals.end(); i++) {
			std::cout << i->first << "=\"" << i->second << "\"" << std::endl;
		}

	} catch (DiffbotException &e) { // Catch Diffbot exception
		std::cerr << "DiffbotException caught: " << e.what() << std::endl;
		return EXIT_FAILURE;
	} catch (std::exception &e) {
		std::cerr << "Exception caught: " << e.what() << std::endl;
		return EXIT_FAILURE;
	} catch (...) {
		std::cerr << "Unknown Exception caught" << std::endl;
		return EXIT_FAILURE;
	}

	// Exit point
	return EXIT_SUCCESS;
}
