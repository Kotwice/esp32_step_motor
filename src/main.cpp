#include <Arduino.h>
#include <SM.h>
#include <TC.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <vector>
#include <LCS.h>

//#include <WS.h>

const char* ssid = "Extruder";
const char* password = "TerraNocturne";

IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);

#define SM1_DIR 0
#define SM1_STP 27
#define SM1_SLP 26
#define SM1_RST 0

#define SM2_DIR 0
#define SM2_STP 12
#define SM2_SLP 14
#define SM2_RST 0

#define SPITC_MISO 25
#define SPITC_SCK 32
#define SPITC_CS_1 33
#define SPITC_CS_2 13

#define LCS_PWM 23
#define LCS_RELAY 13

#define HTML_OK 200

hw_timer_t * timer = NULL;

std::vector<TC> Thermocouples {
    {SPITC_SCK, SPITC_MISO, SPITC_CS_1}, {SPITC_SCK, SPITC_MISO, SPITC_CS_2}
};


TC tc(SPITC_SCK, SPITC_MISO, SPITC_CS_1);


SM_PROPORTIES engine = {1.8, {0.5, 0.1, 3}};

LCS Cooler (LCS_RELAY, LCS_PWM, 6);

std::vector<SM> Engines {
    {engine, SM1_DIR, SM1_STP, SM1_SLP, SM1_RST, "1", 0},
    {engine, SM2_DIR, SM2_STP, SM2_SLP, SM2_RST, "2", 8}
};

struct WebFile {
    String url, path, type;
};

std::vector<String> pathes;

std::vector<WebFile> webfiles;

void get_pathes(fs::FS &fs, const char * dirname, uint8_t levels){
    File root = fs.open(dirname);
    if(!root){
        return;
    }
    if(!root.isDirectory()){
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            if(levels){
                get_pathes(fs, file.name(), levels - 1);
            }
        } else {
            pathes.push_back(file.name());
        }
        file = root.openNextFile();
    }
}


void initiate_file_system () {

    Serial.println("start");

    SPIFFS.begin();

    Serial.println("load spiffs");

    get_pathes(SPIFFS, "/", 0);

    WebFile temporary; 

    for (String path: pathes) {
        temporary.path = path;
        temporary.url = path;
        if (path.substring(path.lastIndexOf(".") + 1, path.length()) == "html") {
            if (path == "/index.html") {
                temporary.url = "/";
            } 
            temporary.type = "text/html";
        }
        if (path.substring(path.lastIndexOf(".") + 1, path.length()) == "css") {
            temporary.type = "text/css";
        }
        if (path.substring(path.lastIndexOf(".") + 1, path.length()) == "js") {
            temporary.type = "text/javascript";
        }
        webfiles.push_back(temporary);
    }
}

void initiate_web () {

    initiate_file_system();

    WiFi.softAP(ssid, password);

    Serial.println("load wifi");

    IPAddress IP = WiFi.softAPIP();

    Serial.println(IP);
    
    for (WebFile webfile: webfiles) {
        
        char url[webfile.url.length() + 1];

        webfile.url.toCharArray(url, webfile.url.length() + 1);

        if (webfile.url == "/index.html") {
            server.on("/", HTTP_GET, [webfile](AsyncWebServerRequest *request) {
                request->send(SPIFFS, webfile.path, String(), false);
            });  
        }
        else {
            server.on(url, HTTP_GET, [webfile](AsyncWebServerRequest *request) {
                request->send(SPIFFS, webfile.path, webfile.type);
            }); 
        }

    }

    Serial.println("load data wifi");
    
    server.on("/sm", HTTP_GET, [](AsyncWebServerRequest *request) {

        for (std::vector<SM>::iterator Engine = Engines.begin(); Engine != Engines.end(); Engine++) {
            if (request->hasParam(Engine->name.frequency)) {
                Serial.println(Engine->name.frequency);
                Engine->frequency = request->getParam(Engine->name.frequency)->value().toFloat();
                if (Engine->state == "ON") {
                    Engine->move();
                }
                else {
                    Engine->stop();
                }
            }
            if (request->hasParam(Engine->name.state)) {
                Engine->state = request->getParam(Engine->name.state)->value();
                Serial.println(Engine->name.state);
                if (Engine->state == "ON") {
                    Engine->move();
                }
                else {
                    Engine->stop();
                }
            }
        }

        String response = "[";

        for (SM Engine: Engines) {

            String temporary = "{'frequency': " + String(Engine.frequency) + ", " + 
                "'state': " + "'" + Engine.state + "'" + "}, ";
            response = response + temporary;
        }

        response.remove(response.length() - 2, 2);

        response = response + "]";

        Serial.println(response);

        request->send(HTML_OK, "text/plain");
    });

    server.on("/sm_initiate", HTTP_GET, [](AsyncWebServerRequest *request) {

        String response = "[";

        for (SM Engine: Engines) {

            String temporary = "{'frequency': " + String(Engine.frequency) + ", " + 
                "'state': " + "'" + Engine.state + "'" + ", " + 
                "'min': " + "'" + String(Engine.range.min) + "'" + ", " +
                "'max': " + "'" + String(Engine.range.max) + "'" + ", " +
                "'step': " + "'" + String(Engine.range.step) + "'" + 
                "}, ";
            response = response + temporary;
        }

        response.remove(response.length() - 2, 2);

        response = response + "]";

        Serial.println(response);

        request->send(HTML_OK, "text/plain", response);
    });

    server.on("/lcs", HTTP_GET, [](AsyncWebServerRequest *request) {

        if (request->hasParam(Cooler.parameters.duty_cycle)) {

            Cooler.rotate(
                request->getParam(Cooler.parameters.duty_cycle)->value().toFloat()
            );

        }

        if (request->hasParam(Cooler.parameters.state)) {

            Cooler.power(
                request->getParam(Cooler.parameters.state)->value()
            );

        }

        String response = "[{'duty_cycle': " + String(Cooler.duty_cycle) + ", " +
            "'lcs_state': " + "'" + Cooler.state + "'}]";

        Serial.println(response);

        request->send(HTML_OK, "text/plain");

    });

    server.on("/lcs_initiate", HTTP_GET, [](AsyncWebServerRequest *request) {

        String response = "[{'duty_cycle': " + String(Cooler.duty_cycle) + ", " +
            "'lcs_state': "  + "'" + Cooler.state + "'" + ", " +
            "'min': " + "'" + String(Cooler.range.min) + "'" + ", " +
            "'step': " + "'" + String(Cooler.range.step) + "'" + ", " +
            "'max': " + "'" + String(Cooler.range.max) + "'}]";

        Serial.println(response);

        request->send(HTML_OK, "text/plain", response);

    });

    server.begin();

}

void IRAM_ATTR timerhandle () {

    for (std::vector<TC>::iterator Thermocouple = Thermocouples.begin(); 
        Thermocouple != Thermocouples.end(); Thermocouple++) {
            Thermocouple->probe();
            Serial.println(Thermocouple->value);
        }
    
}

void setup() {

    Serial.begin(9600);

    float duration = 2;

    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &timerhandle, true);
    timerAlarmWrite(timer, duration * 1e6, true);

    //timerAlarmEnable(timer);

    initiate_web();

}

void loop() {

}