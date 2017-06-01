
#include "main.hpp"

using namespace Poco::Util;
using namespace Poco::JSON;
using namespace Poco::Dynamic;
using namespace Poco::Net;
using namespace Poco;

StravaCpp::StravaCpp() :
    skipApp(false),
    version("dev")
{
    setUnixOptions(true);
}

StravaCpp::~StravaCpp()
{
}

void StravaCpp::initialize(Application& self)
{
    loadConfiguration();

    Poco::Net::initializeSSL();
    Application::initialize(self);
}

void StravaCpp::reinitialize(Application& self)
{
    Application::reinitialize(self);
}

void StravaCpp::uninitialize()
{
    Poco::Net::uninitializeSSL();
    Application::uninitialize();
}

void StravaCpp::handleVersion(const std::string& name, const std::string& value)
{
    std::cout << version << std::endl;
    stopOptionsProcessing();
    skipApp = true;
}

void StravaCpp::handleHelp(const std::string& name, const std::string& value)
{
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setUnixStyle(true);
    helpFormatter.setHeader("Cpp app which interacts with the Strava API V3");
    helpFormatter.format(std::cout);

    stopOptionsProcessing();
    skipApp = true;
}

void StravaCpp::defineOptions(OptionSet& options)
{
    Application::defineOptions(options);

    options.addOption(
        Option("config-file", "f", "load configuration")
        .required(false)
        .repeatable(false)
        .argument("file")
        .callback(OptionCallback<StravaCpp>(this, &StravaCpp::handleConfig)));

    options.addOption(
        Option("help", "h", "display help information")
        .required(false)
        .repeatable(false)
        .callback(OptionCallback<StravaCpp>(this, &StravaCpp::handleHelp)));

    options.addOption(
        Option("version", "v", "display version")
        .required(false)
        .repeatable(false)
        .callback(OptionCallback<StravaCpp>(this, &StravaCpp::handleVersion)));
}

void StravaCpp::handleConfig(const std::string& name, const std::string& value)
{
    FileInputStream stream(value.substr(1), std::ios::in);
    StreamCopier::copyStream(stream, stringBuffer);
    Var contents;

    try
    {
        Parser parser;
        parser.setHandler(new ParseHandler());
        parser.parse(stringBuffer);
        contents = parser.result();
        clearBuffer();
    }
    catch (JSONException& ex)
    {
        std::cout << ex.message() << std::endl;
    }

    auto body = contents.extract<Object::Ptr>();
    auto input = body->get("requests").extract<Poco::JSON::Array::Ptr>();

    accessToken = body->get("accessToken").toString();

    for (auto i = input->begin(); i != input->end(); ++i)
    {
        auto entry = i->extract<Object::Ptr>();
        auto output = entry->getValue<std::string>("output");
        auto body = entry->getValue<std::string>("body");
        auto url = entry->getValue<std::string>("url");

        requests[output] = { url, body, entry->getValue<bool>("array") };
    }
}

void StravaCpp::clearBuffer()
{
    stringBuffer.str("");
    stringBuffer.clear();
}

void StravaCpp::httpsRequest(Context::Ptr context, std::string url, std::ostream& out, std::string body)
{
    URI uri("https://www.strava.com");

    try
    {
        HTTPSClientSession session(uri.getHost(), uri.getPort(), context);
        HTTPResponse response;
        HTTPRequest request;

        request.setMethod(body.empty() ? HTTPRequest::HTTP_GET : HTTPRequest::HTTP_POST);
        request.set("Authorization", "Bearer " + accessToken);
        request.setURI(url);

        session.setPort(443);
        session.setTimeout(Timespan(10L, 0L));

        auto& os = session.sendRequest(request);

        if (request.getMethod() == HTTPRequest::HTTP_POST)
        {
            os << body;
        }

        StreamCopier::copyStream(session.receiveResponse(response), stringBuffer);
    }
    catch (const SSLException& e)
    {
        std::cerr << e.what() << ": " << e.message() << std::endl;
    }
}

void StravaCpp::writeResponse(std::string url, std::string fn, std::stringstream& input, bool array)
{
    using JsonObject = Poco::JSON::Object;
    using JsonArray = Poco::JSON::Array;

    try
    {
        Parser parser;
        Var result = parser.parse(input.str());

        Object json;
        json.set("url", url);
        json.set("epochTime", Timestamp().epochTime());
        json.set("version", version);

        if (array)
            json.set("response", result.extract<JsonArray::Ptr>());
        else
            json.set("response", result.extract<JsonObject::Ptr>());

        FileOutputStream output(fn, std::ios::out | std::ios::trunc);
        Stringifier::stringify(json, true, output, 4);
    }
    catch (const Poco::Exception& e)
    {
        std::cerr << e.what() << ": " << e.message() << std::endl;
        std::cin.get();
    }
}

int StravaCpp::main(const std::vector<std::string>& args)
{
    if (!skipApp)
    {
        if (accessToken.empty())
        {
            handleConfig("-config", "=config.json");

            if (accessToken.empty())
            {
                throw Poco::Exception("No Config File Provided!");
            }
        }

        Context::Ptr context(new Context(Context::CLIENT_USE, ""));
        SSLManager::InvalidCertificateHandlerPtr handler(new AcceptCertificateHandler(false));
        SSLManager::instance().initializeClient(0, handler, context);

        for (auto& request : requests)
        {
            auto& req = request.second;
            auto& fn = request.first;

            httpsRequest(context, req.url, stringBuffer, req.body);
            writeResponse(req.url, fn, stringBuffer, req.array);
            clearBuffer();
        }
    }

    return EXIT_OK;
}

POCO_APP_MAIN(StravaCpp);