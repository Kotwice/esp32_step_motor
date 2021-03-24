#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <SM.h>

#include <vector>

const char* ssid = "Extruder";
const char* password = "TerraNocturne";

IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);

#define SM1_DIR 34
#define SM1_STP 33
#define SM1_SLP 25
#define SM1_RST 35

#define HTML_OK 200

SM_PROPORTIES engine = {1.8, {0.5, 0.1, 3}};

std::vector<SM> Engines {
    {engine, SM1_DIR, SM1_STP, SM1_SLP, SM1_RST}
};

struct WebFile {
    String url, path, type;
};

std::vector<String> pathes;
std::vector<String> url;
std::vector<String> type;


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

    SPIFFS.begin();

    get_pathes(SPIFFS, "/", 0);

    WebFile temporary; 

    for (String path: pathes) {
        temporary.path = path;
        temporary.url = path;
        url.push_back(path);
        if (path.substring(path.lastIndexOf(".") + 1, path.length()) == "html") {
            if (path == "/index.html") {
                temporary.url = "/";
                url.push_back("/");
            } 
            temporary.type = "text/html";
            type.push_back("text/html");
        }
        if (path.substring(path.lastIndexOf(".") + 1, path.length()) == "css") {
            temporary.type = "text/css";
            type.push_back("text/css");
        }
        if (path.substring(path.lastIndexOf(".") + 1, path.length()) == "js") {
            temporary.type = "text/javascript";
            type.push_back("text/javascript");
        }
        webfiles.push_back(temporary);
    }
}

void initiate_web () {

    Serial.begin(9600);

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
            if (request->hasParam("sm_freq_1")) {
                Engine->frequency = request->getParam("sm_freq_1")->value().toFloat();
                if (request->hasParam("sm_state_1")) {
                    Engine->state = request->getParam("sm_state_1")->value();
                    if (Engine->state == "ON") {
                        Engine->move();
                    }
                    else {
                        Engine->stop();
                    }
                }
            }
            if (request->hasParam("sm_state_1")) {
                Engine->state = request->getParam("sm_state_1")->value();
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

            String temporary = "{'sm_freq': " + String(Engine.frequency) + ", " + 
                "'sm_state': " + "'" + Engine.state + "'" + "}, ";
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

            String temporary = "{'sm_freq': " + String(Engine.frequency) + ", " + 
                "'sm_state': " + "'" + Engine.state + "'" + ", " + 
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

    server.begin();

}

void setup() {
    initiate_web();
}

void loop() {

}