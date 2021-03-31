#include <Arduino.h>
#include <SM.h>
#include <TC.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <vector>
#include <LCS.h>
#include <HT.h>
#include <PID.h>

//#include <WS.h>

const char* ssid = "Extruder";
const char* password = "TerraNocturne";

IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);

#define SM1_PIN_STEP 27
#define SM1_PIN_SLEEP 26
#define SM1_PWM_CHANNEL 0

#define SM2_PIN_STEP 12
#define SM2_PIN_SLEEP 14
#define SM2_PWM_CHANNEL 10

#define SPITC_MISO 25
#define SPITC_SCK 32
#define SPITC_CS_1 33
#define SPITC_CS_2 13

#define LCS_PWM 23
#define LCS_RELAY 13
#define LCS_PWM_CHANNEL 4

#define HT1_PIN_PWM 18
#define HT2_PIN_PWM 19

#define HT1_PWM_CHANNEL 6
#define HT2_PWM_CHANNEL 8

#define HTML_OK 200

hw_timer_t * timer = NULL;

LCS Cooler (LCS_RELAY, LCS_PWM, LCS_PWM_CHANNEL);

std::vector<TC> Thermocouples {
    {SPITC_SCK, SPITC_MISO, SPITC_CS_1, "1"}, 
    {SPITC_SCK, SPITC_MISO, SPITC_CS_2, "2"}
};

std::vector<SM> Engines {
    {SM1_PIN_STEP, SM1_PIN_SLEEP, SM1_PWM_CHANNEL, "1"},
    {SM2_PIN_STEP, SM2_PIN_SLEEP, SM2_PWM_CHANNEL, "2"}
};

std::vector<HT> Heaters {
    {HT1_PIN_PWM, HT1_PWM_CHANNEL, "1"},
    {HT2_PIN_PWM, HT2_PWM_CHANNEL, "2"}
};

std::vector<PID> Regulators {
    {200, 1, 0.5, 0.5, 0.5, "1"}, {180, 1, 0.5, 0.5, 0.5, "2"}
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

    IPAddress IP = WiFi.softAPIP();
    
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
    
    server.on("/sm", HTTP_GET, [](AsyncWebServerRequest *request) {

        for (std::vector<SM>::iterator Engine = Engines.begin(); Engine != Engines.end(); Engine++) {

            if (request->hasParam(Engine->parameters.frequency)) {
                Engine->rotate(
                    request->getParam(Engine->parameters.frequency)->value().toFloat()
                );
            }

            if (request->hasParam(Engine->parameters.state)) {
                Engine->power(
                    request->getParam(Engine->parameters.state)->value()
                );
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
                "'step': " + "'" + String(Engine.range.step) + "'" + ", " +
                "'max': " + "'" + String(Engine.range.max) + "'" + 
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
            "'state': "  + "'" + Cooler.state + "'" + ", " +
            "'min': " + "'" + String(Cooler.range.min) + "'" + ", " +
            "'step': " + "'" + String(Cooler.range.step) + "'" + ", " +
            "'max': " + "'" + String(Cooler.range.max) + "'}]";

        Serial.println(response);

        request->send(HTML_OK, "text/plain", response);

    });

    server.on("/hs_initiate", HTTP_GET, [](AsyncWebServerRequest *request) {

        String response = "[";

        for (PID Regulator: Regulators) {

            String temporary = "{'i': " + String(Regulator.coefficients.i) + ", " + 
                "'p': " + String(Regulator.coefficients.p) + ", " + 
                "'d': " + String(Regulator.coefficients.d) + ", " + 
                "'destination': " + String(Regulator.destination) + ", " + 
                "'state': " + String(Regulator.state) + "'" + 
                "}, ";
            response = response + temporary;
        }

        response.remove(response.length() - 2, 2);

        response = response + "]";

        Serial.println(response);

        request->send(HTML_OK, "text/plain", response);

    });

    server.on("/hs", HTTP_GET, [](AsyncWebServerRequest *request) {

        for (std::vector<PID>::iterator Regulator = Regulators.begin(); 
            Regulator != Regulators.end(); Regulator++) {

            if (request->hasParam(Regulator->parameters.i)) {
                Regulator->coefficients.i = request->getParam(Regulator->parameters.i)->value().toFloat();
            }

            if (request->hasParam(Regulator->parameters.p)) {
                Regulator->coefficients.p = request->getParam(Regulator->parameters.p)->value().toFloat();
            }

            if (request->hasParam(Regulator->parameters.d)) {
                Regulator->coefficients.d = request->getParam(Regulator->parameters.d)->value().toFloat();
            }

            if (request->hasParam(Regulator->parameters.destination)) {
                Regulator->destination = request->getParam(Regulator->parameters.destination)->value().toFloat();
            }

            if (request->hasParam(Regulator->parameters.state)) {
                Regulator->state = request->getParam(Regulator->parameters.state)->value();
            }

        }

        request->send(HTML_OK, "text/plain");

    });

    server.begin();

}

void IRAM_ATTR timerhandle () {

    for (int i = 0; i < Regulators.size(); i++) {
        Thermocouples[i].probe();
        Regulators[i].calculate(Thermocouples[i].mesument);
        Heaters[i].heat(Regulators[i].control);
    }
    /*
    for (std::vector<TC>::iterator Thermocouple = Thermocouples.begin(); 
        Thermocouple != Thermocouples.end(); Thermocouple++) {
            Thermocouple->probe();
            //Regulator->calculate(Thermocouple->mesument);
            //Heater->heat(Regulator->control);
        }
        */
    
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