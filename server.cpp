#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <string>

using namespace std;
using namespace Pistache;
using namespace Pistache::Rest;

int tempInside, tempOutside, humInside, humOutside, lightInside, lightOutside;
int windowOpenness = 0, curtainOpenness = 100, wantedTemp = 19, wantedHumidity = 40, wantedLight = 7500;

bool is_digits(const std::string &str)
{
    return str.find_first_not_of("0123456789") == std::string::npos;
}

void testRoute(const Rest::Request& request, Http::ResponseWriter response) {
    cout << "Received a request from IP address " << request.address().host() << '\n';

    response.send(Http::Code::Ok, "Test Route Ok!\n");
} 

void setSensor(const Rest::Request& request, Http::ResponseWriter response)
{
    auto location = request.param(":location").as<std::string>(); 
    auto option = request.param(":option").as<std::string>();             
    auto value = request.param(":value").as<std::string>();      

    if(location.compare("outside") != 0 && location.compare("inside") != 0)
    {
        response.send(Http::Code::Ok, "Location poate avea doar valorile outside sau inside.\n");
        return;
    }
    if(option.compare("temperature") != 0 && option.compare("humidity") != 0 && option.compare("light") != 0)
    {
        response.send(Http::Code::Ok, "Option poate avea doar valorile temperature, humidity sau light.\n");
        return;
    }
    if(!is_digits(value))
    {
        response.send(Http::Code::Ok, "Value poate fi doar un numar intreg.\n");
        return;
    }

    if(location.compare("inside") == 0)
        if(option.compare("temperature") == 0)
            tempInside = stoi(value);
        if(option.compare("humidity") == 0)
            humInside = stoi(value);
        if(option.compare("light") == 0)
            lightInside = stoi(value);
    if(location.compare("outside") == 0)
        if(option.compare("temperature") == 0)
            tempOutside = stoi(value);
        if(option.compare("humidity") == 0)
            humOutside = stoi(value);
        if(option.compare("light") == 0)
            lightOutside = stoi(value);
    
    response.send(Http::Code::Ok, "Settings saved\n");
}

void giveCommand()
{
    if((humInside < wantedHumidity) && (humOutside >= wantedHumidity))
        windowOpenness += 50;
    if((humInside > wantedHumidity) && (humOutside <= wantedHumidity))
        windowOpenness += 50;

    if((tempInside < wantedTemp) && (tempOutside >= wantedTemp))
        windowOpenness += 50;
    if((tempInside > wantedTemp) && (tempOutside <= wantedTemp))
        windowOpenness += 50;


    if((humInside < wantedHumidity) && (humOutside < wantedHumidity))
        windowOpenness = 0;
    if((humInside > wantedHumidity) && (humOutside > wantedHumidity))
        windowOpenness = 0;
    
    if((tempInside < wantedTemp) && (tempOutside < wantedTemp))
        windowOpenness = 0;
    if((tempInside > wantedTemp) && (tempOutside > wantedTemp))
        windowOpenness = 0;

    if((lightInside < wantedLight) && (lightOutside >= wantedLight))
        curtainOpenness = 100;
    if(lightInside > wantedLight)
        curtainOpenness -= 75;

    if(windowOpenness > 100)
        windowOpenness = 100;
    if(curtainOpenness < 0)
        curtainOpenness = 0;
}

int main() {
    Router router;

    router.get("/test", Routes::bind(testRoute));
    router.get("/sensor/set/:location/:option/:value", Routes::bind(setSensor));

    Address addr(Ipv4::any(), Port(9080));

    auto opts = Http::Endpoint::options().threads(1);
    Http::Endpoint server(addr);
    server.init(opts);
    server.setHandler(router.handler());
    server.serve(); 
}