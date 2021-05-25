#include <string>
#include <regex>
#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <fstream>
#include <mqtt/client.h>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace std;
using namespace Pistache;
using namespace Pistache::Rest;

using json = nlohmann::json;

// sensor values
int tempInside, tempOutside, humInside, humOutside, lightInside, lightOutside, stressLevel, curHour, curMinute;
// user values
int windowOpenness = 0, curtainOpenness = 100, netOpenness = 0, wantedTemp, wantedHumidity, wantedLight, wantedHour, wantedMinute;
bool turnOnSecurityAlarm, wakeUpAlarm;

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

void giveCommand() {
    string msg;

    if((humInside < wantedHumidity) && (humOutside >= wantedHumidity)) {
        windowOpenness += 50;
        msg = "A fost deschisa fereastra cu 50 de procente.\n";
    }
    if((humInside > wantedHumidity) && (humOutside <= wantedHumidity)) {
        windowOpenness += 50;
        msg = "A fost deschisa fereastra cu 50 de procente.\n";
    }
    if((tempInside < wantedTemp) && (tempOutside >= wantedTemp)) {
        windowOpenness += 50;
        msg = "A fost deschisa fereastra cu 50 de procente.\n";
    }
    if((tempInside > wantedTemp) && (tempOutside <= wantedTemp)) {
        windowOpenness += 50;
        msg = "A fost deschisa fereastra cu 50 de procente.\n";
    }


    if((humInside < wantedHumidity) && (humOutside < wantedHumidity)) {
        windowOpenness = 0;
        msg = "A fost inchisa fereastra.\n";
    }
    if((humInside > wantedHumidity) && (humOutside > wantedHumidity)) {
        windowOpenness = 0;
        msg = "A fost inchisa fereastra.\n";
    }

    if((tempInside < wantedTemp) && (tempOutside < wantedTemp)) {
        windowOpenness = 0;
        msg = "A fost inchisa fereastra.\n";
    }
    if((tempInside > wantedTemp) && (tempOutside > wantedTemp)) {
        windowOpenness = 0;
        msg = "A fost inchisa fereastra.\n";
    }

    if((lightInside < wantedLight) && (lightOutside >= wantedLight)) {
        curtainOpenness = 100;
        msg = "A fost deschisa perdeaua.\n";
    }
    if(lightInside > wantedLight) {
        curtainOpenness -= 75;
        msg = "A fost inchisa perdeaua cu 75 de procente.\n";
    }

    if(wakeUpAlarm == true) {
        if(curHour >= wantedHour) {
            if(curMinute >= wantedMinute) {
                curtainOpenness = 100;
                wakeUpAlarm = false;
                msg = "A fost deschisa perdeaua ca alarma de dimineata.\n";
            }
        }
    }

    if(windowOpenness > 100)
        windowOpenness = 100;
    if(curtainOpenness < 0)
        curtainOpenness = 0;

    if(stressLevel > 5){
        turnOnSecurityAlarm = true;
        msg = "A pornit alarma!\n";
    }

    logActivity(msg);
}

void mqttPublish(const string topic, const string payload) {
    const string address = "localhost";
    const string clientId = "sensor";

    // Create a client
    mqtt::client client(address, clientId);

    mqtt::connect_options options;
    options.set_keep_alive_interval(20);
    options.set_clean_session(true);

    try {
        // Connect to the client
        client.connect(options);

        // Create a message
        auto msg = mqtt::make_message(topic, payload);

        // Publish it to the server
        client.publish(msg);

        // Disconnect
        client.disconnect();
    }
    catch (const mqtt::exception& exc) {
        cerr << exc.what() << " [" << exc.get_reason_code() << "]" << endl;
    }
}

// Sensor Settings

// Citit din fisier JSON, numele fisierului trimis prin HTTP
// http://localhost:9080/sensor/set/sensorValues.json
void setSensorValueJson(const Rest::Request& request, Http::ResponseWriter response) {
    auto filename = request.param(":filename").as<std::string>();

    std::ifstream sensorParams(filename, std::ifstream::binary);
    json params;
    sensorParams >> params;

    tempInside = params["inside"]["temperature"];
    tempOutside = params["outside"]["temperature"];

    humInside = params["inside"]["humidity"];
    humOutside = params["outside"]["humidity"];

    lightInside = params["inside"]["light"];
    lightOutside = params["outside"]["light"];

    curHour = params["hour"];
    curMinute = params["minute"];

    stressLevel = params["stressLevel"];

    giveCommand();
    response.send(Http::Code::Ok, "Sensor settings saved\n");
}

// Trimis prin MQTT publish
// http://localhost:9080/sensor/get/:location/:option
void getSensorValue(const Rest::Request& request, Http::ResponseWriter response) {
    auto location = request.param(":location").as<std::string>();
    auto option = request.param(":option").as<std::string>();

    string msg;
    string topic;
 
    if(location.compare("outside") != 0 && location.compare("inside") != 0) {
        response.send(Http::Code::Not_Found, "Location poate avea doar valorile outside sau inside.\n");
        return;
    }
    if(option.compare("temperature") != 0 && option.compare("humidity") != 0 && option.compare("light") != 0) {
        response.send(Http::Code::Not_Found, "Option poate avea doar valorile temperature, humidity sau light.\n");
        return;
    }

    msg = location + " + " + option + " = ";


    if(location.compare("inside") == 0) {
        if(option.compare("temperature") == 0) {
            msg += to_string(tempInside);
            topic = "insideTemperature";
        }
        if(option.compare("humidity") == 0) {
            msg += to_string(humInside);
            topic = "insideHumidity";
        }
        if(option.compare("light") == 0) {
            msg += to_string(lightInside);
            topic = "insideLight";
        }
    }
    
    if(location.compare("outside") == 0) {
        if(option.compare("temperature") == 0) {
            msg += to_string(tempOutside);
            topic = "outsideTemperature";
        }
        if(option.compare("humidity") == 0) {
            msg += to_string(humOutside);
            topic = "outsideHumidity";
        }
        if(option.compare("light") == 0) {
            msg += to_string(lightOutside);
            topic = "outsideLight";
        }
    }

    mqttPublish(topic, msg);
}

// User Settings

// http://localhost:9080/settings/set/:action/:object/:value
void setStateObject(const Rest::Request& request, Http::ResponseWriter response) {
    auto action = request.param(":action").as<std::string>();
    auto object = request.param(":object").as<std::string>();
    auto value = request.param(":value").as<std::string>();

    string msg;


    if(action.compare("open") != 0 && action.compare("close") != 0 && action.compare("turnOff") != 0) {
        msg = "Gresit! Poti doar sa deschizi/inchizi fereastra/perdeaua/plasa sau sa opresti alarma.\n";
        logActivity(msg);
        response.send(Http::Code::Not_Found, msg);
        return;
    }
    if(object.compare("window") != 0 && object.compare("curtain") != 0 && object.compare("net") != 0
    && object.compare("alarm") != 0) {
        msg = "Gresit! Obiectul nu este fereasta/perdea/plasa/alarma.\n";
        logActivity(msg);
        response.send(Http::Code::Not_Found, msg);
        return;
    }
    if(!is_digits(value)) {
        msg = "Gresit! Value poate fi doar un numar intreg.\n";
        logActivity(msg);
        response.send(Http::Code::Not_Found, msg);
        return;
    }

    if(object.compare("window") == 0) {
        if(action.compare("open") == 0) {
            windowOpenness += stoi(value);
            msg = "Ok! Ati deschisa fereastra cu " + value + " procente.\n";
        }
        if(action.compare("close") == 0) {
            windowOpenness -= stoi(value);
            msg = "Ok! Ati inchisa fereastra cu " + value + " procente.\n";
        }
    }
    if(object.compare("curtain") == 0) {
        if(action.compare("open") == 0) {
            curtainOpenness += stoi(value);
            msg = "Ok! Ati deschisa perdeaua cu " + value + " procente.\n";
        }
        if(action.compare("close") == 0) {
            curtainOpenness -= stoi(value);
            msg = "Ok! Ati inchisa perdeaua cu " + value + " procente.\n";
        }
    }
    if(object.compare("net") == 0) {
        if(action.compare("open") == 0) {
            netOpenness += stoi(value);
            msg = "Ok! Ati deschisa plasa cu " + value + " procente.\n";
        }
        if(action.compare("close") == 0) {
            netOpenness -= stoi(value);
            msg = "Ok! Ati inchisa plasa cu " + value + " procente.\n";
        }
    }
    if(object.compare("alarm") == 0) {
        if(action.compare("turnOff") == 0) {
            if(stoi(value) == 0){
                turnOnSecurityAlarm = false;
                msg = "Ok! Ati oprit alarma.\n";
            }
            else{
                response.send(Http::Code::Not_Found, "Value poate fi doar 0.\n");
                return;
            }
        }
    }

    if(windowOpenness > 100)
        windowOpenness = 100;
    if(windowOpenness < 0)
        windowOpenness = 0;
    if(curtainOpenness > 100)
        curtainOpenness = 100;
    if(curtainOpenness < 0)
        curtainOpenness = 0;
    if(netOpenness > 100)
        netOpenness = 100;
    if(netOpenness < 0)
        netOpenness = 0;

    logActivity(msg);
    response.send(Http::Code::Ok, msg);
}

// http://localhost:9080/settings/get/:object
void getStateObject(const Rest::Request& request, Http::ResponseWriter response) {
    auto object = request.param(":object").as<std::string>();

    string msg;

    if(object.compare("window") != 0 && object.compare("curtain") != 0 && object.compare("net") != 0) {
        msg = "Gresit! Object poate avea doar valorile window, curtain sau net.\n";
        logActivity(msg);
        response.send(Http::Code::Not_Found, msg);
        return;
    }

    if(object.compare("window") == 0) {
        if(windowOpenness == 0) {
            msg = object + " este inchis\n";
        }
        else{
            msg = object + " este deschis " + to_string(windowOpenness) + "\n";
        }
    }

    if(object.compare("curtain") == 0) {
        if(curtainOpenness == 0) {
            msg = object + " este inchis\n";
        }
        else{
            msg = object + " este deschis " + to_string(curtainOpenness) + "\n";
        }
    }

    if(object.compare("net") == 0) {
        if(netOpenness == 0) {
            msg = object + " este inchis\n";
        }
        else{
            msg = object + " este deschis " + to_string(netOpenness) + "\n";
        }
    }

    logActivity(msg);
    response.send(Http::Code::Ok, msg);
}

// Setarea valorilor dorite

// Setarea valorilor dorite prin JSON direct din cURL
// curl --header "Content-Type: application/json"   
//      --request POST   
//      --data '{"option":"temperature/humidity/light/alarm","value":"[0-9]* | ([0-9]{2}):([0-9]{2})"}'   
//      http://localhost:9080/wantedValues/set
void setWantedValues(const Rest::Request& request, Http::ResponseWriter response) {
    const auto params = json::parse(request.body());

    auto option = to_string(params["option"]);
    option = option.substr(1,option.length()-2);

    auto value = to_string(params["value"]);
    value = value.substr(1,value.length()-2);

    string msg;

    regex reg_hours("([0-9]{2}):([0-9]{2})");

    if(option.compare("temperature") != 0 && option.compare("humidity") != 0 && option.compare("light") != 0 && option.compare("alarm") != 0) {
        msg = "Gresit! Option poate avea doar valorile temperature, humidity, light sau alarm.\n";
        logActivity(msg);
        response.send(Http::Code::Not_Found, msg);
        return;
    }

    if(is_digits(value)) {
        if(option.compare("temperature") == 0) {
            wantedTemp = stoi(value);
            msg = "Temperatura dorita a fost setata la: " + value + ".\n";
        }
        if(option.compare("humidity") == 0) {
            wantedHumidity = stoi(value);
            msg = "Umiditatea dorita a fost setata la: " + value + ".\n";
        }
        if(option.compare("light") == 0) {
            wantedLight = stoi(value);
            msg = "Luminozitatea dorita a fost setata la: " + value + ".\n";
        }
    }
    else if(regex_match(value, reg_hours) && option.compare("alarm") == 0){
        wakeUpAlarm = true;
        wantedHour = stoi(value.substr(0, 2));
        wantedMinute = stoi(value.substr(3, 2));
        msg = "Ora de alarma a fost setata la: " + value.substr(0, 2) + ":" + value.substr(3, 2) + ".\n";
    }
    else{
        msg = "Gresit! Value poate fi doar valoare numerica, sau string de tipul HH:MM.\n";
        logActivity(msg);
        response.send(Http::Code::Not_Found, msg);
        return;
    }

    giveCommand();
    logActivity(msg);
    response.send(Http::Code::Ok, msg);
}

// Returnarea valorilor dorite

// Returnarea valorilor dorite prin JSON direct din cURL
// http://localhost:9080/wantedValues/get/:option
void getWantedValues(const Rest::Request& request, Http::ResponseWriter response) {
    auto option = request.param(":option").as<string>();

    string msg;
    json j;

    if(option.compare("temperature") != 0 && option.compare("humidity") != 0 && option.compare("light") != 0 && option.compare("alarm") != 0) {
        msg = "Gresit! Option poate avea doar valorile temperature, humidity, light sau alarm.\n";
        logActivity(msg);
        response.send(Http::Code::Not_Found, msg);
        return;
    }

    if(option.compare("temperature") == 0) {
        j["temperature"] = to_string(wantedTemp);
        msg = "Temperatura dorita este: " + to_string(wantedTemp) + "\n";
    }
    if(option.compare("humidity") == 0) {
        j["humidity"] = to_string(wantedHumidity);
        msg = "Umiditatea dorita este: " + to_string(wantedHumidity) + "\n";
    }
    if(option.compare("light") == 0) {
        j["light"] = to_string(wantedLight);
        msg = "Luminozitatea dorita este: " + to_string(wantedLight) + "\n";
    }
    if(option.compare("alarm") == 0) {
        j["alarm"] = to_string(wantedHour) + ":" + to_string(wantedMinute);
        msg = "Alarma de trezire a fost setata la ora: " + to_string(wantedHour) + ":" + to_string(wantedMinute) + "\n";
    }

    logActivity(msg);
    response.send(Http::Code::Ok, j.dump());
}

int main() {
    Router router;

    // Ruta de test pentru libraria Pistache
    router.get("/test", Routes::bind(testRoute));

    // Rute pentru setarea si returnarea valorilor senzorilor din interior si exterior
    router.get("/sensor/set/:filename", Routes::bind(setSensorValueJson)); // din fisier JSON
    router.get("/sensor/get/:location/:option", Routes::bind(getSensorValue)); // publish to MQTT

    // Rute pentru setarea si returnarea valorilor pozitiei ferestrei/perdelei
    router.get("/settings/set/:action/:object/:value", Routes::bind(setStateObject));
    router.get("/settings/get/:object", Routes::bind(getStateObject));

    // Rute pentru setarea si returnarea valorilor dorite in interiorul camerei
    router.post("/wantedValues/set", Routes::bind(setWantedValues)); // direct din JSON
    router.get("/wantedValues/get/:option", Routes::bind(getWantedValues)); // JSON.dump

    Address addr(Ipv4::any(), Port(9080));

    cout << "Serverul asteapta requesturi\n";

    auto opts = Http::Endpoint::options().threads(1);
    Http::Endpoint server(addr);
    server.init(opts);
    server.setHandler(router.handler());
    server.serve(); 
}
