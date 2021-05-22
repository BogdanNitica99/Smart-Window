#include <string>
#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <fstream>
 
using namespace std;
using namespace Pistache;
using namespace Pistache::Rest;
 
int tempInside, tempOutside, humInside, humOutside, lightInside, lightOutside, stressLevel;
int windowOpenness = 0, curtainOpenness = 100, netOpenness = 0, wantedTemp = 19, wantedHumidity = 40, wantedLight = 7500;
bool turnOnAlarm;
 
void log( string text ){
    string filename("log.txt");
    ofstream file_out;
 
    file_out.open(filename, std::ios_base::app);
    file_out << text << endl;
    file_out.close();   
}
 

bool is_digits(const std::string &str) {
    return str.find_first_not_of("0123456789") == std::string::npos;
}
 
void testRoute(const Rest::Request& request, Http::ResponseWriter response) {
    cout << "Received a request from IP address " << request.address().host() << '\n';
 
    response.send(Http::Code::Ok, "Test Route Ok!\n");
} 
 
void setSensorValue(const Rest::Request& request, Http::ResponseWriter response) {
    log("setSensorValue");
    auto location = request.param(":location").as<std::string>();
    auto option = request.param(":option").as<std::string>();
    auto value = request.param(":value").as<std::string>();
 
    if(location.compare("outside") != 0 && location.compare("inside") != 0) {
        response.send(Http::Code::Not_Found, "Location poate avea doar valorile outside sau inside.\n");
        return;
    }
    if(option.compare("temperature") != 0 && option.compare("humidity") != 0 && option.compare("light") != 0
    && option.compare("alarm") != 0) {
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
        if(option.compare("alarm") == 0) {
            stressLevel = stoi(value);
        }
    }
 
    response.send(Http::Code::Ok, "Sensor settings saved\n");
}

void setLocationValue(const Rest::Request& request, Http::ResponseWriter response) {
    auto action = request.param(":action").as<std::string>();
    auto object = request.param(":object").as<std::string>();
    auto value = request.param(":value").as<std::string>();

    if(action.compare("open") != 0 && action.compare("close") != 0 && action.compare("turn off") != 0) {
        response.send(Http::Code::Not_Found, "Poti doar sa deschizi/inchizi fereastra/perdeaua/plasa sau sa opresti alarma.\n");
        return;
    }
    if(object.compare("window") != 0 && object.compare("curtain") != 0 && object.compare("net") != 0
    && object.compare("alarm") != 0) {
        response.send(Http::Code::Not_Found, "Obiectul nu este fereasta/perdea/plasa/alarma.\n");
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
    if(object.compare("net") == 0) {
        if(action.compare("open") == 0) {
            netOpenness += stoi(value);
        }
        if(action.compare("close") == 0) {
            netOpenness -= stoi(value);
        }
    }
    if(object.compare("alarm") == 0) {
        if(action.compare("turn off") == 0) {
            if(stoi(value) == 0)
                turnOnAlarm = false;
            else{
                response.send(Http::Code::Not_Found, "Value poate fi doar 0.\n");
                return;
            }
        }
    }
    
    response.send(Http::Code::Ok, "Location settings saved\n");
}

void getSensorValue(const Rest::Request& request, Http::ResponseWriter response) {
    log("getSensorValue");
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

    if(object.compare("window") != 0 && object.compare("curtain") != 0 && object.compare("net") != 0) {
        response.send(Http::Code::Not_Found, "Location poate avea doar valorile window, curtain sau net.\n");
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
            response.send(Http::Code::Ok, "open " + to_string(curtainOpenness));
        }
    }

    if(object.compare("net") == 0) {
        if(netOpenness == 0) {
            response.send(Http::Code::Ok, "close");
        }
        else{
            response.send(Http::Code::Ok, "open " + to_string(netOpenness));
        }
    }
}

void setWantedValues(const Rest::Request& request, Http::ResponseWriter response) {
    log("setWantedValues");
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
    log("getWantedValues");
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
    log("giveCommand");
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

    if(stressLevel > 5)
        turnOnAlarm = true;

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
    router.get("/settings/set/:action/:object/:value", Routes::bind(setLocationValue));
    router.get("/settings/get/:object", Routes::bind(getPositionValue));
  
    Address addr(Ipv4::any(), Port(9080));
 
    cout << "Serverul asteapta requesturi\n";
 
    auto opts = Http::Endpoint::options().threads(1);
    Http::Endpoint server(addr);
    server.init(opts);
    server.setHandler(router.handler());
    server.serve(); 
}