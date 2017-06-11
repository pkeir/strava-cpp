
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/NetSSL.h>
#include <Poco/Net/InvalidCertificateHandler.h>
#include <Poco/Net/AcceptCertificateHandler.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/SSLException.h>

#include <Poco/SharedPtr.h>
#include <Poco/StreamCopier.h>
#include <Poco/FileStream.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/JSON.h>
#include <Poco/URI.h>
#include <strava.hpp>

#include <algorithm>
#include <exception>
#include <iostream>
#include <sstream>

using namespace Poco;
using namespace Poco::Net;

Poco::Net::Context::Ptr session = nullptr;

strava::oauth authentication;

void request(Context::Ptr context, std::string url, std::ostream& out, std::string body = "")
{
    URI uri("https://www.strava.com");

    try
    {
        HTTPSClientSession session(uri.getHost(), uri.getPort(), context);
        HTTPResponse response;
        HTTPRequest request;

        request.setMethod(body.empty() ? HTTPRequest::HTTP_GET : HTTPRequest::HTTP_POST);
        request.set("Authorization", "Bearer " + authentication.access_token);
        request.setURI(url);

        session.setPort(443);
        session.setTimeout(Timespan(10L, 0L));

        auto& os = session.sendRequest(request);

        if (request.getMethod() == HTTPRequest::HTTP_POST)
        {
            os << body;
        }

        StreamCopier::copyStream(session.receiveResponse(response), out);
    }
    catch (const SSLException& e)
    {
        std::cerr << e.what() << ": " << e.message() << std::endl;
    }
}

JSON::Object::Ptr parse(std::stringstream& response) 
{
    Poco::JSON::Parser parser;
    auto tree = parser.parse(response.str());
    return tree.extract<JSON::Object::Ptr>();
}

void strava::athletes::current(strava::athlete& athlete)
{
    std::stringstream response;
    request(session, "/api/v3/athlete", response);

    auto root = parse(response);

    std::map<std::string, std::string*> bindings 
    {
        {"firstname", &athlete.firstname },
        {"lastname", &athlete.lastname }
    };

    for (auto& e : *root)
    {
        auto key = e.first;
        auto value = e.second;

        if (bindings.count(key) != 0)
        {
            *bindings[key] = value.toString();
        }
    }
}

void strava::authenticate(strava::oauth&& autho, bool skip_init)
{
    authentication = autho;

    if(!skip_init && session.isNull())
    {
        session = Context::Ptr(new Context(Context::CLIENT_USE, ""));

        SSLManager::InvalidCertificateHandlerPtr handler(new AcceptCertificateHandler(false));
        SSLManager::instance().initializeClient(0, handler, session);
    }
}