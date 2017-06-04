
#include <memory>
#include <future>
#include <cstdio>
#include <cstdlib>
#include <restbed>
#include <iostream>

using namespace std;
using namespace restbed;

void print(const shared_ptr< Response >& response)
{
    fprintf(stderr, "*** Response ***\n");
    fprintf(stderr, "Status Code:    %i\n", response->get_status_code());
    fprintf(stderr, "Status Message: %s\n", response->get_status_message().data());
    fprintf(stderr, "HTTP Version:   %.1f\n", response->get_version());
    fprintf(stderr, "HTTP Protocol:  %s\n", response->get_protocol().data());

    for (const auto header : response->get_headers())
    {
        fprintf(stderr, "Header '%s' = '%s'\n", header.first.data(), header.second.data());
    }

    auto length = response->get_header("Content-Length");

    Http::fetch(length, response);

    fprintf(stderr, "Body:           %.*s...\n\n", 25, response->get_body().data());
}

int main(int argc, const char* argv[])
{
    try
    {
        auto url = "http://www.williamsamtaylor.co.uk/"s;
        auto request = make_shared< Request>(Uri(url.c_str()));
        request->set_header("Accept", "*/*");
        request->set_header("Host", "www.williamsamtaylor.com");

        auto response = Http::sync(request);
        print(response);
    }
    catch (std::runtime_error err)
    {
        std::cout << err.what() << std::endl;
        std::cin.get();
    }

    return 0;
}