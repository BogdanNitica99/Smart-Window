#include <string>
#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <fstream>

using namespace std;
using namespace Pistache;
using namespace Pistache::Rest;

int tempInside, tempOutside, humInside, humOutside, lightInside, lightOutside;
int windowOpenness = 0, curtainOpenness = 100, wantedTemp = 19, wantedHumidity = 40, wantedLight = 7500;

 void logActivity(string msg){
    string filename("log.txt");
    ofstream file_out;
 
    file_out.open(filename, ios_base::app);
    file_out << msg << endl;
    file_out.close();   
}

bool is_digits(const string &str) {
    return str.find_first_not_of("0123456789") == string::npos;
}

void testRoute(const Rest::Request& request, Http::ResponseWriter response) {
    cout << "Received a request from IP address " << request.address().host() << '\n';
 
    response.send(Http::Code::Ok, "Test Route Ok!\n");
} 

// Sensor Settings

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

    string msgToReturn = location + " + " + option + " = ";
 
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
            msgToReturn += to_string(tempInside);
        }
        if(option.compare("humidity") == 0) {
            msgToReturn += to_string(humInside);
        }
        if(option.compare("light") == 0) {
            msgToReturn += to_string(lightInside);
        }
    }
    
    if(location.compare("outside") == 0) {
        if(option.compare("temperature") == 0) {
            msgToReturn += to_string(tempOutside);
        }
        if(option.compare("humidity") == 0) {
            msgToReturn += to_string(humOutside);
        }
        if(option.compare("light") == 0) {
            msgToReturn += to_string(lightOutside);
        }
    }

    response.send(Http::Code::Ok, msgToReturn + "\n");
}

// User Settings

void setStateObject(const Rest::Request& request, Http::ResponseWriter response) {
    auto action = request.param(":action").as<std::string>();
    auto object = request.param(":object").as<std::string>();
    auto value = request.param(":value").as<std::string>();

    if(action.compare("open") != 0 && action.compare("close") != 0) {
        response.send(Http::Code::Not_Found, "Poti doar sa deschizi/inchizi.\n");
        return;
    }
    if(object.compare("window") != 0 && object.compare("curtain") != 0) {
        response.send(Http::Code::Not_Found, "Obiectul nu este fereasta/perdea.\n");
        return;
    }
    if(!is_digits(value)) {
        response.send(Http::Code::Not_Found, "Value poate fi doar un numar intreg.\n");
        return;
    }

    if(object.compare("window") == 0) {
        if(action.compare("open") == 0) {
            windowOpenness += stoi(value);
        }
        if(action.compare("close") == 0) {
            windowOpenness -= stoi(value);
        }
    }
    if(object.compare("curtain") == 0) {
        if(action.compare("open") == 0) {
            windowOpenness += stoi(value);
        }
        if(action.compare("close") == 0) {
            windowOpenness -= stoi(value);
        }
    }
    
    response.send(Http::Code::Ok, "Object state settings saved\n");
}

void getStateObject(const Rest::Request& request, Http::ResponseWriter response) {
    auto object = request.param(":object").as<std::string>();

    if(object.compare("window") != 0 && object.compare("curtain") != 0) {
        response.send(Http::Code::Not_Found, "Obiectul poate avea doar valorile window sau curtain.\n");
        return;
    }

    if(object.compare("window") == 0) {
        if(windowOpenness == 0) {
            response.send(Http::Code::Ok, "close");
        }
        else{
            response.send(Http::Code::Ok, "open " + to_string(windowOpenness));
        }
    }

    if(object.compare("curtain") == 0) {
        if(curtainOpenness == 0) {
            response.send(Http::Code::Ok, "close");
        }
        else{
            response.send(Http::Code::Ok, "open " + to_string(windowOpenness));
        }
    }
}

// Setarea valorilor pentru geam

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
    auto option = request.param(":option").as<string>();
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
    router.get("/settings/set/:action/:object/:value", Routes::bind(setStateObject));
    router.get("/settings/get/:object", Routes::bind(getStateObject));

    Address addr(Ipv4::any(), Port(9080));

    cout << "Serverul asteapta requesturi\n";

    auto opts = Http::Endpoint::options().threads(1);
    Http::Endpoint server(addr);
    server.init(opts);
    server.setHandler(router.handler());
    server.serve(); 
}
