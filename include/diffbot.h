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

#ifndef _DIFFBOT_H
#define _DIFFBOT_H

#include <exception>
#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <string>
#include <map>
#include <sstream>

#include <curl/curl.h>
#include <jsoncpp/json/json.h>

/*
 	DIFFBOT API HEADER FILE
*/

namespace diffbot {

// Encapsulating response from Diffbot server in JSON format
class DiffbotResult {
protected:
	Json::Value json;
	std::string method;
public:
	DiffbotResult(std::string met, Json::Value v) : method(met), json(v) {};

	/**
	* @return - a Json object
	*/
	Json::Value getJson() const {return json;}

	/**
	* a shortcut for getField method
	* @see getField method
	*/
	std::string operator [](std::string field) const {return getField(field);}

	/**
	* get particular field from Json as a string
	* @return field value as a String
	*/
	std::string getField(std::string field) const;

	/**
	* get all fields from Json as a string-to-string map, only for the 1st nesting level, i.e. "title" "html"
	* for the 2 and higher levels please use getJson object
	* @return string map
	*/
	std::map<std::string, std::string> getAllFields() const;
};

// Main Diffbot class
class Diffbot {
protected:
	std::string ApiUrl, token, fields, method, response;
	std::map<std::string, std::string> params;
	int timeout, version;

public:
	/**
	* default constructor
	*/
	Diffbot();

	/**
	* constructor
	* @tok a token
	*/
	Diffbot(std::string tok);

	// token access methods
	void setToken(std::string tok);
	std::string getToken() const;

	// version access methods
	void setVersion(int ver);
	int getVersion() const;

	// timeout access methods
	void setTimeout(int t);
	int getTimeout() const;

	// fields access methods
	void setFields(std::string f);
	std::string getFields() const;

	// "method" access methods
	void setMethod(std::string m);
	std::string getMethod() const;

	// custom request parameters access methods, i.e. "mode" for "classifier" API
	void setParameter(std::string key, std::string value);
	void delParameter(std::string key);
	std::string getParameter(std::string key) const;

	// access method for current "response" string, unparsed
	std::string getRawResponse() const {return response;}

	/**
	* parses an article
	* @url - URL to download the article from
	* @see ApiRequest
	* @see parseResponse
	* @return - DiffbotResult object
	*/
	//DiffbotResult parseArticle(std::string url);	// example of specific methods for certain APIs

	/**
	* the main workhorse
	* @url - URL to download the page from
	* @return - string containg the response, not parsed yet; it is also stored in "response" member variable
	*/
	std::string ApiRequest(std::string url);

	/**
	* parses the "response" member variable
	* @return - a Diffbot Result object containing encapsulated JSON
	*/
	DiffbotResult parseResponse();

protected:
	/**
	* constructs API URL, used internally
	*/
	std::string mkApiUrl() const;
	/**
	* Initialize default values, used internally
	*/
	void init();
};

// Diffbot Exception class
class DiffbotException : public std::exception {
public:
	/**
	* constructor
	* @m is an exception message which contains details what has happened
	*/
	DiffbotException(std::string m) : msg(m) {}
	~DiffbotException() throw() {}
	/**
	* overrided what method
	* @return the message that was supplied to the constructor
	*/
	const char* what() const throw() { return msg.c_str(); }

private:
	std::string msg;
};

// Global Diffbot initializer, a singleton class. Used internally.
class DiffbotInitializer {
public:
	DiffbotInitializer();
	~DiffbotInitializer();
};

}
#endif

