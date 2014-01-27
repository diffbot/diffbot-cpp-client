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
 *	DIFFBOT API IMPLEMENTATION
*/

#include "diffbot.h"

/* HELPER FUNCTIONS */

namespace diffbot {

// curl callback that collects HTTP response
size_t write_func(void *ptr, size_t size, size_t count, void *stream)
{
	((std::string*)stream)->append((char*)ptr, 0, size*count);
	return size*count;
}

// Universal string converter
template<class T> std::string toStr(T i)
{
	std::stringstream ss;
	std::string s;
	ss << i;
	s = ss.str();
	return s;
}

// 
std::string escapeStr(std::string str)
{
	CURL *curl;
	curl = curl_easy_init();
	if (! curl) throw DiffbotException( "Unable to create CURL object");
	char* urlEscaped = curl_easy_escape(curl, str.c_str(), 0);
	std::string ret = std::string(urlEscaped);
	curl_free(urlEscaped);
	curl_easy_cleanup(curl);
	return ret;
}

// Diffbot class methods implementation
Diffbot::Diffbot(std::string tok) {
	init();
	setToken(tok);
}

Diffbot::Diffbot() {
	init();
}

void Diffbot::init() {
	ApiUrl = "http://api.diffbot.com/";
	setVersion(2);
	setTimeout(5);
	setMethod("article");
}

void Diffbot::setToken(std::string tok)
{
	token = tok;
	if (token.empty()) throw DiffbotException("Token is empty");
}

std::string Diffbot::getToken() const
{
	return token;
}

void Diffbot::setFields(std::string f)
{
	fields = f;
}

std::string Diffbot::getFields() const
{
	return fields;
}

void Diffbot::setMethod(std::string m)
{
	method = m;
	//if (method!="article") throw DiffbotException("Unsupported method: "+method);
}

std::string Diffbot::getMethod() const
{
	return method;
}

void Diffbot::setVersion(int ver)
{
	version = ver;
	if (version!=2) throw DiffbotException(std::string("Unsupported version: ")+toStr(version));
}

int Diffbot::getVersion() const
{
	return version;
}

void Diffbot::setTimeout(int t)
{
	timeout = t;
	if (timeout<0) throw DiffbotException(std::string("Unsupported timeout: ")+toStr(timeout));
}

int Diffbot::getTimeout() const
{
	return timeout;
}

void Diffbot::setParameter(std::string key, std::string value)
{
	params[key] = value;
}

void Diffbot::delParameter(std::string key)
{
	params.erase(key);
}

std::string Diffbot::getParameter(std::string key) const
{
	std::map<std::string, std::string>::const_iterator i=params.find(key);
	if (i==params.end()) return std::string("");
	return i->second;
}

/*
DiffbotResult Diffbot::parseArticle(std::string url)
{
	setMethod("article");
	ApiRequest(url);
	return parseResponse();
}*/

std::string Diffbot::mkApiUrl() const {
	return ApiUrl+"v"+toStr(version)+"/"+method;
}

std::string Diffbot::ApiRequest(std::string url)
{
	CURL *curl;
	CURLcode res;
 
	/* Get a curl handle */ 
	curl = curl_easy_init();
	if (! curl) throw DiffbotException( "Unable to create CURL object");

	/* First set the URL that is about to receive our POST. This URL can
		 just as well be a https:// URL if that is what should receive the
		 data. */ 

	std::string curApiUrl = mkApiUrl();
	curl_easy_setopt(curl, CURLOPT_URL, curApiUrl.c_str());

	/* Now specify the POST data */ 
	std::string postData;
	postData += std::string("token=") + getToken();
	postData += "&url=" + escapeStr(url);
	if (!fields.empty()) postData += "&fields=" + fields;
	for (std::map<std::string,std::string>::const_iterator i=params.begin(); i!=params.end(); i++) {
		postData += std::string("&")+escapeStr(i->first)+"="+escapeStr(i->second);
	}

	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_func);
	if (timeout>0) curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout+2); // allow 2 additional seconds for network delay

	/* Perform the request, res will get the return code */ 
	res = curl_easy_perform(curl);

	/* Check for errors */ 
	if(res != CURLE_OK) throw DiffbotException(std::string("curl_easy_perform() failed: ")+curl_easy_strerror(res));

	/* always cleanup */ 
	curl_easy_cleanup(curl);

	return response;
}

DiffbotResult Diffbot::parseResponse() {
	Json::Value values;
	Json::Reader reader;

	bool parseSuccess = reader.parse(response, values);
	if (! parseSuccess) throw DiffbotException("Incorrect JSON response: " + response);

	DiffbotResult res(method, values);

	return res;
}

std::string DiffbotResult::getField(std::string field) const {
	Json::Value val = getJson().get(field, "");

	switch ( val.type() ) {
		case Json::nullValue:
		case Json::stringValue:
		case Json::booleanValue: return val.asString();
		case Json::intValue: return toStr(val.asInt());
		case Json::uintValue: return toStr(val.asUInt());
		case Json::realValue: return toStr(val.asFloat());
		case Json::arrayValue:
		case Json::objectValue: return "<object>";
		default: return "<unknown>";
	}
	
	return val.asString();
}

std::map<std::string, std::string> DiffbotResult::getAllFields() const {
	std::map<std::string, std::string> res;

	Json::Value::Members members = getJson().getMemberNames();
	for (std::vector<std::string>::iterator i=members.begin(); i!=members.end(); i++) {
		res[*i] = getField(*i);
	}

	return res;
}

DiffbotInitializer::DiffbotInitializer() {
	/* In windows, this will init the winsock stuff */ 
	curl_global_init(CURL_GLOBAL_ALL);
}

DiffbotInitializer::~DiffbotInitializer() {
	curl_global_cleanup();
}

static DiffbotInitializer diffbot_initializer; // static initializer

}
