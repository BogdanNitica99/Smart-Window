#include <string>
#include <pistache/endpoint.h>
#include <pistache/router.h>


using namespace std;
using namespace Pistache;
using namespace Pistache::Rest;

int tempInside, tempOutside, humInside, humOutside, lightInside, lightOutside;
int windowOpenness = 0, curtainOpenness = 100, wantedTemp = 19, wantedHumidity = 40, wantedLight = 7500;

bool is_digits(const std::string &str) {
    return str.find_first_not_of("0123456789") == std::string::npos;
}

void testRoute(const Rest::Request& request, Http::ResponseWriter response) {
    cout << "Received a request from IP address " << request.address().host() << '\n';

    response.send(Http::Code::Ok, "Test Route Ok!\n");
} 

void setSensorValue(const Rest::Request& request, Http::ResponseWriter response) {
    auto location = request.param(":location").as<std::string>();
    auto option = request.param(":option").as<std::string>();
    auto value = request.param(":value").as<std::string>();

    if(location.compare("outside") != 0 && location.compare("inside") != 0) {
        response.send(Http::Code::Not_Found, "Location poate avea doar valorile outside sau inside.\n");
        return;
    }
    if(option.compare("temperature") != 0 && option.compare("humidity") != 0 && option.compare("light") != 0) {
        response.send(Http::Code::Not_Found, "Option poate avea doar valorile temperature, humidity sau light.\n");
        return;
    }
    if(!is_digits(value)) {
        response.send(Http::Code::Not_Found, "Value poate fi doar un numar intreg.\n");
        return;
    }

    if(location.compare("inside") == 0) {
        if(option.compare("temperature") == 0) {
            tempInside = stoi(value);
        }
        if(option.compare("humidity") == 0) {
            humInside = stoi(value);
        }
        if(option.compare("light") == 0) {
            lightInside = stoi(value);
        }
    }
    if(location.compare("outside") == 0) {
        if(option.compare("temperature") == 0) {
            tempOutside = stoi(value);
        }
        if(option.compare("humidity") == 0) {
            humOutside = stoi(value);
        }
        if(option.compare("light") == 0) {
            lightOutside = stoi(value);
        }
    }
    
    response.send(Http::Code::Ok, "Sensor settings saved\n");
}

void getSensorValue(const Rest::Request& request, Http::ResponseWriter response) {
    auto location = request.param(":location").as<std::string>();
    auto option = request.param(":option").as<std::string>();

    if(location.compare("outside") != 0 && location.compare("inside") != 0) {
        response.send(Http::Code::Not_Found, "Location poate avea doar valorile outside sau inside.\n");
        return;
    }
    if(option.compare("temperature") != 0 && option.compare("humidity") != 0 && option.compare("light") != 0) {
        response.send(Http::Code::Not_Found, "Option poate avea doar valorile temperature, humidity sau light.\n");
        return;
    }

    if(location.compare("inside") == 0) {
        if(option.compare("temperature") == 0) {
            response.send(Http::Code::Ok, location + " + " + option + " = " + to_string(tempInside) + "\n");
        }
        if(option.compare("humidity") == 0) {
            response.send(Http::Code::Ok, location + " + " + option + " = " + to_string(humInside) + "\n");
        }
        if(option.compare("light") == 0) {
            response.send(Http::Code::Ok, location + " + " + option + " = " + to_string(lightInside) + "\n");
        }
    }
    if(location.compare("outside") == 0) {
        if(option.compare("temperature") == 0) {
            response.send(Http::Code::Ok, location + " + " + option + " = " + to_string(tempOutside) + "\n");
        }
        if(option.compare("humidity") == 0) {
            response.send(Http::Code::Ok, location + " + " + option + " = " + to_string(humOutside) + "\n");
        }
        if(option.compare("light") == 0) {
            response.send(Http::Code::Ok, location + " + " + option + " = " + to_string(lightOutside) + "\n");
        }
    }
}

void getPositionValue (const Rest::Request& request, Http::ResponseWriter response) {
    auto object = request.param(":object").as<std::string>();

    if(object.compare("window") != 0 && object.compare("curtain") != 0) {
        response.send(Http::Code::Not_Found, "Location poate avea doar valorile window sau curtain.\n");
        return;
    }

    if(object.compare("window") == 0) {
        if(windowOpenness == 0) {
            response.send(Http::Code::Ok, "close 0");
        }
        else{
            response.send(Http::Code::Ok, "open " + to_string(windowOpenness));
        }
    }

    if(object.compare("curtain") == 0) {
        if(curtainOpenness == 0) {
            response.send(Http::Code::Ok, "close 0");
        }
        else{
            response.send(Http::Code::Ok, "open " + to_string(windowOpenness));
        }
    }
}

void setWantedValues(const Rest::Request& request, Http::ResponseWriter response) {
    auto option = request.param(":option").as<std::string>();
    auto value = request.param(":value").as<std::string>();

    if(option.compare("temperature") != 0 && option.compare("humidity") != 0 && option.compare("light") != 0) {
        response.send(Http::Code::Not_Found, "Option poate avea doar valorile temperature, humidity sau light.\n");
        return;
    }

    if(!is_digits(value)) {
        response.send(Http::Code::Not_Found, "Value poate fi doar un numar intreg.\n");
        return;
    }

    if(option.compare("temperature") == 0) {
        wantedTemp = stoi(value);
    }
    if(option.compare("humidity") == 0) {
        wantedHumidity = stoi(value);
    }
    if(option.compare("light") == 0) {
        wantedLight = stoi(value);
    }

    response.send(Http::Code::Ok, "Wanted settings saved\n");
}

void getWantedValues(const Rest::Request& request, Http::ResponseWriter response) {
    auto option = request.param(":option").as<std::string>();

    if(option.compare("temperature") != 0 && option.compare("humidity") != 0 && option.compare("light") != 0) {
        response.send(Http::Code::Not_Found, "Option poate avea doar valorile temperature, humidity sau light.\n");
        return;
    }

    if(option.compare("temperature") == 0) {
        response.send(Http::Code::Ok, "Wanted " + option + " = " + to_string(wantedTemp) + "\n");
    }
    if(option.compare("humidity") == 0) {
        response.send(Http::Code::Ok, "Wanted " + option + " = " + to_string(wantedHumidity) + "\n");
    }
    if(option.compare("light") == 0) {
        response.send(Http::Code::Ok, "Wanted " + option + " = " + to_string(wantedLight) + "\n");
    }
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

    // Ruta de test pentru libraria Pistache
    router.get("/test", Routes::bind(testRoute));

    // Rute pentru setarea si returnarea valorilor senzorilor din interior si exterior
    router.get("/sensor/set/:location/:option/:value", Routes::bind(setSensorValue));
    router.get("/sensor/get/:location/:option", Routes::bind(getSensorValue));

    // Rute pentru setarea si returnarea valorilor dorite in interiorul camerei
    router.get("/wantedValues/set/:option/:value", Routes::bind(setWantedValues));
    router.get("/wantedValues/get/:option", Routes::bind(getWantedValues));

    // Rute pentru setarea si returnarea valorilor pozitiei ferestrei/perdelei
    router.get("/settings/get/:object", Routes::bind(getPositionValue));
    // router.get("/settings/set/:option", Routes::bind(setPositionValue));

    Address addr(Ipv4::any(), Port(9080));

    cout << "Serverul asteapta requesturi\n";

    auto opts = Http::Endpoint::options().threads(1);
    Http::Endpoint server(addr);
    server.init(opts);
    server.setHandler(router.handler());
    server.serve(); 
}