DIFFBOT C++ CLIENT

HOW TO USE

  1) Make sure "diffbot.h" is on your compiler's Include path
  4) Link diffbot object file "diffbot.o" OR add "diffbot.cpp" as a source file into your project
  3) Use Diffbot classes according to demo file

  Please refer to src/demo.cpp file for usage examples.

CLASSES OVERVIEW

  Diffbot - the main interface to Diffbot API
  DiffbotResult - encapsulation of Json response from Diffbot server
  DiffbotException - Diffbot exception class

DIRECTORIES & FILES
  include/  - header files: diffbot.h
  src/      - source files: diffbot.cpp and demo.cpp
  lib/      - object files to be generated during compilation
  deps/     - the libraries that are required by Diffbot API client
  Makefile  - used by "make" command to manage building and testing
  INSTALL   - installation notes
  README    - this file

USAGE

 Typical usage scenario:

 1) Create Diffbot class instance using your Diffbot token:

	Diffbot diffbot("MY_DIFFBOT_TOKEN");

 2) Set options and parameters if required, for example:

	diffbot.setTimeout(20);
	diffbot.setFields("meta");
	diffbot.setParameter("mode", "article");
	diffbot.setMethod(method ? std::string(method) : "article");

 3) Send request, URL is of std::string type:

	diffbot.ApiRequest(url);

 4) Parse the response:

	DiffbotResult res = diffbot.parseResponse();

 5) Explore the result:

	// Get Json object
	Json::Value json = res.getJson();

	// Get the list of keys
	std::vector<std::string> list = json.getMemberNames();

	// Iterate over json object
	std::vector<std::string>::iterator i;
	for (i=members.begin(); i!=members.end(); i++) cout << json.get(*i, "") << " ";

     Please refer to src/demo.cpp for a working example of usage.

APIs

  Please refer to http://diffbot.com/products/automatic/  to get the detailed description of available Diffbot API
  and corresponding fields.
   
NOTES

  - Diffbot classes are declared inside "diffbot" namespace.
  You will need either to use "diffbot::Diffbot()" syntax or "using namespace diffbot" operator.

  - We recommend that you review include/diffbot.h and src/demo.cpp files for better understanding how Diffbot APIs work.

-Initial commit by Anatoliy Kyrychenko-
