
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

strava::auth_info authentication;
strava::session client_session;

class not_implemented : public std::logic_error
{
public:
    not_implemented() : std::logic_error("Function not yet implemented") { };
};

void https(Context::Ptr context, std::string url, std::string token, std::ostream& out, std::string body = "")
{
    URI uri("https://www.strava.com");

    try
    {
        HTTPSClientSession session(uri.getHost(), uri.getPort(), context);
        HTTPResponse response;
        HTTPRequest request;

        request.setMethod(body.empty() ? HTTPRequest::HTTP_GET : HTTPRequest::HTTP_POST);
        request.set("Authorization", "Bearer " + token);
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

void strava::athletes::current(strava::athelete& athelete)
{
    std::stringstream response;
    https(client_session.context, "/api/v3/athlete", authentication.access_token, response);

    athelete = {};
    athelete.name = response.str();
}

void strava::setupSession()
{
    client_session.context = Context::Ptr(new Context(Context::CLIENT_USE, ""));

    SSLManager::InvalidCertificateHandlerPtr handler(new AcceptCertificateHandler(false));
    SSLManager::instance().initializeClient(0, handler, client_session.context);
}

void strava::authenticate(std::string token)
{
    authentication.access_token = token;
}