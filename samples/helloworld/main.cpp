
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <iostream>
#include <string>

using namespace rapidjson;

int main(int argc, char* argv[])
{
    std::string json = "{\"project\":\"rapidjson\",\"stars\":10}";
    
    Document d;
    d.Parse(json.c_str());
    
    Value& s = d["stars"];
    s.SetInt(s.GetInt() + 1);
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    std::cout << buffer.GetString() << std::endl;
    std::cin.get();
}