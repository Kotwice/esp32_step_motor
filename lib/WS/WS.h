#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <vector>

#include <SM.h>
#include <TC.h>

typedef struct {
    String url, path, type;
} WebFile;

typedef struct {
    std::vector<SM> Engines;
    std::vector<TC> Thermocouples;
} Periphery;

class WS {
    public:
        WS(Periphery periphery);
        std::vector<String> pathes;
        std::vector<WebFile> webfiles;
        std::vector<SM> Engines;
        std::vector<TC> Thermocouples;
        void get_pathes (fs::FS &fs, const char * dirname, uint8_t levels);
        void initate_file_system ();
        void initialte_web ();
        Periphery periphery;
        const char* ssid = "Extruder";
        const char* password = "TerraNocturne";

        IPAddress local_ip(192, 168, 1, 1);
        IPAddress gateway(192, 168, 1, 1);
        IPAddress subnet(255, 255, 255, 0);
        AsyncWebServer server(80);
};


void WS::get_pathes(fs::FS &fs, const char * directory, uint8_t levels){

    File root = fs.open(directory);

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


void WS::initiate_file_system () {

    SPIFFS.begin();

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

    server.begin();

}