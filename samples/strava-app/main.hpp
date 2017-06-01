
#pragma once

#include <Poco/Util/Application.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Net/NetSSL.h>
#include <Poco/Net/InvalidCertificateHandler.h>
#include <Poco/Net/AcceptCertificateHandler.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/Context.h>
#include <Poco/SharedPtr.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/StreamCopier.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/FileStream.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/JSON.h>
#include <Poco/URI.h>
#include <Poco/Net/SSLException.h>

#include <algorithm>
#include <sstream>
#include <string>

struct RequestInfo
{
    std::string url;
    std::string body;
    bool array;
};

class StravaCpp : public Poco::Util::Application
{
    std::map<std::string, RequestInfo> requests;
    std::stringstream stringBuffer;
    std::string accessToken;
    std::string version;
    bool skipApp;
public:
    StravaCpp();
    ~StravaCpp();

    void httpsRequest(Poco::Net::Context::Ptr context, std::string, std::ostream&, std::string body);
    void handleVersion(const std::string& name, const std::string& value);
    void handleConfig(const std::string& name, const std::string& value);
    void handleHelp(const std::string& name, const std::string& value);

    void writeResponse(std::string url, std::string, std::stringstream&, bool array = false);
    void defineOptions(Poco::Util::OptionSet& options) override;
    void reinitialize(Application& self) override;
    void initialize(Application& self) override;
    void uninitialize() override;
    void clearBuffer();

protected:
    int main(const std::vector<std::string> &args);
};