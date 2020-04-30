#include <SPI.h>
#include <LoRa.h>
#include "SSD1306.h"

SSD1306 display(0x3c, 4, 15);

#define SS 18
#define RST 14
#define DI0 26 //ldr
//#define DI1     22 //ledpin
#define BAND 915E6

//Constante para informar ao Slave que queremos os dados
const String GETDATA = "getdata";
//Constante que o Slave retorna junto com os dados para o Master
const String SETDATA = "setdata=";

const String SETSTATE = "setstate=";

typedef struct
{
  int nlight;
  bool nstate;
} Data;

void setupDisplay()
{
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);
  delay(50);
  digitalWrite(16, HIGH);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

void setupLoRa()
{
  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DI0);

  if (!LoRa.begin(BAND))
  {
    display.drawString(5, 25, "Starting LoRa failed!");
    while (1)
      ;
  }

  LoRa.enableCrc();
  LoRa.receive();
}

/*Begin Gateway Config*/
#include <WiFi.h>

#include "EEPROM.h"

#include <HTTPClient.h>
#include <MQTT.h>
#include <ArduinoJson.h>

//#include <ESPmDNS.h>
//#include <WebServer.h>

#define EEPROM_SIZE 64

//#define MQTT_SERVER "45.32.163.18"
//#define MQTT_SERVER "10.0.0.10"
#define MQTT_SERVER "192.168.1.100"

#define TOPIC_NAME "api/v1/nodes"
#define TOPIC_GATEWAY "api/v1/nodes/6104/data"
//#define TOPIC_NODE_1 "api/v1/nodes/6104/data"
#define TOPIC_NODE_1 "api/v1/nodes/update"

WiFiClient wifiClient;
MQTTClient client;

//MDNSResponder mdns;
//WebServer server(80);

const char *ssid = "AME";
//const char *passphrase = "votenesseprojeto";
String st;
String content;

#define INTERVAL 500
long lastSendTime = 0;
bool NODE_STATE;

int ledpin = 22;

Data data;

String createJsonString()
{
  String json = "{";
  json += "\"temp\":";
  json += String(123);
  json += ",";
  json += "\"humidity\":";
  json += String(data.nlight);
  json += "}";
  return json;
}

//mqtt.publish(topic+"/update", JSON.stringify({time:new Date()}))
//Published {"clientId":"mqttjs_cc064862","topic":"+/+/updated"}

void showData()
{
  String waiting = String(millis() - lastSendTime);
  display.clear();
  int glight = analogRead(A0);
  int gstate = digitalRead(ledpin);

  if (gstate == 0)
  {
    display.drawString(0, 0, "Gateway LED desligado");
  }
  else
  {
    display.drawString(0, 0, "Gateway LED ligado");
  }
  display.drawString(0, 16, String(glight) + " g-lux");

  if (data.nstate == 0)
  {
    display.drawString(0, 32, "Node LED ligado");
  }
  else
  {
    display.drawString(0, 32, "Node LED desligado");
  }
  display.drawString(0, 48, String(data.nlight) + " n-lux");
  display.drawString(70, 48, waiting + " ms");
  display.display();
}

void send()
{
  LoRa.beginPacket();
  LoRa.print(GETDATA);
  LoRa.endPacket();
}

void sendState(bool NODE_STATE)
{
  LoRa.beginPacket();
  LoRa.print(SETSTATE + NODE_STATE);
  LoRa.endPacket();
  Serial.println("Pacote enviado:");
  Serial.println(SETSTATE + NODE_STATE);
}

void receive()
{
  //Tentamos ler o pacote
  int packetSize = LoRa.parsePacket();

  if (packetSize > SETDATA.length())
  {
    String received = "";
    for (int i = 0; i < SETDATA.length(); i++)
    {
      received += (char)LoRa.read();
    }

    if (received.equals(SETDATA))
    {

      //Fazemos a leitura dos dados
      LoRa.readBytes((uint8_t *)&data, sizeof(data));
      //Mostramos os dados no display
      showData();

      Serial.print("Publish message: ");
      //Criamos o json que enviaremos para o server mqtt
      String msg = createJsonString();
      Serial.println(msg);
      //Publicamos no tópico onde o servidor espera para receber
      //e gerar o gráfico
      //client.subscribe("a663ab60-80b0-11e9-9375-05b17602acf8/9606f130-8733-11e9-a915-c31d0eed4cca/update");
      //client.publish("api/v1/gateway/6104", msg.c_str());

      client.publish("de6f0ee0-87c8-11e9-b83e-6f3b2914d082/9606f130-8733-11e9-a915-c31d0eed4cca/updated", msg.c_str());
    }
    /*
    if(received.equals(SETSTATE)){
      LoRa.readBytes((uint8_t*)&data, sizeof(data));
      Serial.print("mqtt send state change");
      NODE_STATE = data.state;
      sendState(NODE_STATE);
    }*/
  }
}

void setupWiFi()
{
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);

  //WiFi.begin(ssid, passphrase);
  WiFi.begin(ssid);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void messageReceived(String &topic, String &payload)
{
  Serial.println(payload);
  Serial.println("reading EEPROM gateway_id");
  String gateway_id = "";
  for (int i = 100; i < 173; ++i)
  {
    gateway_id += char(EEPROM.read(i));
  }
  //a663ab60-80b0-11e9-9375-05b17602acf8/f6b00f80-86ca-11e9-adbc-7b8e4b4feb60";
  gateway_id = "de6f0ee0-87c8-11e9-b83e-6f3b2914d082/f6b00f80-86ca-11e9-adbc-7b8e4b4feb60";
  Serial.println(digitalRead(ledpin));
  Serial.println(gateway_id);
  Serial.println("LDR GATEWAY:");
  Serial.println(analogRead(A0));
  if (strstr(topic.c_str(), gateway_id.c_str()))
  {
    if (strstr(payload.c_str(), "true"))
    {
      Serial.println("Acender led do gateway");
      digitalWrite(ledpin, HIGH); //should be HIGH
    }
    else if (strstr(payload.c_str(), "false"))
    {
      Serial.println("Apagar led do gateway");
      digitalWrite(ledpin, LOW); //should be LOW
    }
  }
  else
  {
    Serial.println("Send to node");
    if (strstr(payload.c_str(), "true"))
    {
      Serial.println("Acender led do node");
      sendState(true);
    }
    else if (strstr(payload.c_str(), "false"))
    {
      Serial.println("Apagar led do node");
      sendState(false);
    }
  }
  Serial.println("received from mqtt: " + topic + " - " + payload);
}

void connectMQTTServer(String device_id)
{
  delay(500);

  Serial.print("connecting mqtt...");
  if (!client.connected())
  {
    while (!client.connect(device_id.c_str()))
    {
      Serial.print(".");
    }
  }
  Serial.println("\nconectado!");
  client.subscribe(device_id + "/update"); //gateway ou node
  Serial.println(device_id);
}

/*
bool testWifi(void) {
int c = 0;
Serial.println("Waiting for Wifi to connect");
while ( c < 20 ) {
if (WiFi.status() == WL_CONNECTED) { return true; }
delay(500);
Serial.print(WiFi.status());
c++;
}
Serial.println("");
Serial.println("Connect timed out, opening AP");
return false;
}

void createWebServer(int webtype)
{

if ( webtype == 1 ) {
server.on("/", []() {
IPAddress ip = WiFi.softAPIP();
String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
content = "<!DOCTYPE HTML>\r\n<html>Olá, Esse é o servidor AURORA no ip: ";
content += ipStr;
content += "<p>";
content += st;
content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><label>CÓDIGO: </label><input name='code' length=4><input type='submit'></form>";
content += "</html>";
server.send(200, "text/html", content);
});
server.on("/setting", []() {
String qsid = server.arg("ssid");
String qpass = server.arg("pass");
String qcode = server.arg("code");
if (qsid.length() > 0 && qpass.length() > 0) {
Serial.println("clearing eeprom");
for (int i = 0; i < 100; ++i) { EEPROM.write(i, 0); }
Serial.println(qsid);
Serial.println("");
Serial.println(qpass);
Serial.println("");
Serial.println(qcode);
Serial.println("");

Serial.println("writing eeprom ssid:");
for (int i = 0; i < qsid.length(); ++i)
{
EEPROM.write(i, qsid[i]);
Serial.print("Wrote: ");
Serial.println(qsid[i]);
}
Serial.println("writing eeprom pass:");
for (int i = 0; i < qpass.length(); ++i)
{
EEPROM.write(32+i, qpass[i]);
Serial.print("Wrote: ");
Serial.println(qpass[i]);
}
Serial.println("writing eeprom code:");
for (int i = 0; i < qcode.length(); ++i)
{
EEPROM.write(96+i, qcode[i]);
Serial.print("Wrote: ");
Serial.println(qcode[i]);
}
EEPROM.commit();
content = "<!DOCTYPE HTML>\r\n<html>";
content += "<p>saved to eeprom... reset to boot into new wifi</p></html>";
} else {
content = "Error";
Serial.println("Sending 404");
}
server.send(200, "text/html", content);
});
} else {
server.on("/", []() {
server.send(200, "text/plain", "this works as well");
});
server.on("/setting", []() {
server.send(200, "text/plain", "setting.");
});
server.on("/cleareeprom", []() {
content = "<!DOCTYPE HTML>\r\n<html>";
content += "<p>Clearing the EEPROM</p></html>";
server.send(200, "text/html", content);
Serial.println("clearing eeprom");
for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }
EEPROM.commit();
});
}
}

void launchWeb(int webtype) {
Serial.println("");
Serial.println("WiFi connected");
Serial.print("Local IP: ");
Serial.println(WiFi.localIP());
Serial.print("SoftAP IP: ");
Serial.println(WiFi.softAPIP());
if (!mdns.begin("esp32")) {
Serial.println("Error setting up MDNS responder!");
while(1) {
delay(1000);
}
}
Serial.println("mDNS responder started");
createWebServer(webtype);
// Start the server
server.begin();
Serial.println("Server started");
}

void setupAP(void) {
WiFi.mode(WIFI_STA);
WiFi.disconnect();
delay(100);
int n = WiFi.scanNetworks();
Serial.println("scan done");
if (n == 0)
Serial.println("no networks found");
else
{
Serial.print(n);
Serial.println(" networks found");
for (int i = 0; i < n; ++i)
{
// Print SSID and RSSI for each network found
Serial.print(i + 1);
Serial.print(": ");
Serial.print(WiFi.SSID(i));
Serial.print(" (");
Serial.print(WiFi.RSSI(i));
Serial.print(")");
Serial.println((WiFi.encryptionType(i) == 7)?" ":"*"); // 7 = NONE https://www.arduino.cc/en/Reference/WiFiEncryptionType
delay(10);
}
}
Serial.println("");
st = "<ol>";
for (int i = 0; i < n; ++i)
{
// Print SSID and RSSI for each network found
st += "<li>";
st += WiFi.SSID(i);
st += " (";
st += WiFi.RSSI(i);
st += ")";
st += (WiFi.encryptionType(i) == 7)?" ":"*";
st += "</li>";
}
st += "</ol>";
delay(100);
WiFi.softAP(ssid);
Serial.println("softap");
launchWeb(1);
Serial.println("over");
}
*/

void setup()
{
  Serial.begin(115200);
  pinMode(22, OUTPUT);
  /*
  EEPROM.begin(512);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println("Startup");
  // read eeprom for ssid and pass
  Serial.println("Reading EEPROM ssid");
  String esid = "";
  for (int i = 0; i < 32; ++i)
  {
  esid += char(EEPROM.read(i));
}
Serial.print("SSID: ");
Serial.println(esid);
Serial.println("Reading EEPROM pass");
String epass = "";
for (int i = 32; i < 96; ++i)
{
epass += char(EEPROM.read(i));
}
Serial.print("PASS: ");
Serial.println(epass);*/
  Serial.println("Reading EEPROM code");
  String ecode = "";
  for (int i = 96; i < 100; ++i)
  {
    ecode += char(EEPROM.read(i));
  }
  Serial.print("CODE: ");
  Serial.println(ecode);
  /*if ( esid.length() > 1 ) {
// test esid
Serial.println("TEST EEPROM SSID");
WiFi.begin(esid.c_str(), epass.c_str());
if (testWifi()) {
launchWeb(0);
return;
}
}
Serial.println("SETUP AURORA AP");
setupAP();*/

  setupDisplay();
  setupLoRa();

  display.clear();
  display.drawString(0, 0, "AME");
  display.drawString(0, 16, "AUTOMAÇÃO");
  display.drawString(0, 26, "MODULAR");
  display.drawString(0, 36, "ESCALÁVEL.");
  display.display();

  setupWiFi();

  if ((WiFi.status() == WL_CONNECTED))
  { //Check the current connection status

    HTTPClient http;
    client.begin(MQTT_SERVER, 1883, wifiClient);
    client.onMessage(messageReceived);

    /*ecode="";
  if (ecode.length() != 4)
  ecode = "2186";*/

    String service_url = "http://192.168.1.100:3456/api/v1/nodes/register/6494"; //+ecode;
    Serial.println(service_url);
    http.begin(service_url);   //Specify the URL and certificate
    int httpCode = http.GET(); //Make the request

    if (httpCode > 0)
    { //Check for the returning code

      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);

      const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
      DynamicJsonBuffer jsonBuffer(capacity);

      // Parse JSON object
      JsonObject &gateway_json = jsonBuffer.parseObject(payload);
      if (!gateway_json.success())
      {
        Serial.println(F("Parsing failed!"));
        return;
      }

      String gateway_id = gateway_json["user"].as<String>() + "/" + gateway_json["uuid"].as<String>();
      Serial.println("writing eeprom gateway_id:");
      for (int i = 0; i < gateway_id.length(); ++i)
      {
        EEPROM.write(100 + i, gateway_id[i]);
        Serial.print("Wrote Gateway: ");
        Serial.println(gateway_id[i]);
      }
      connectMQTTServer(gateway_id);
    }
    else
    {
      Serial.println("Error on HTTP request");
    }
    http.end();

    service_url = "http://192.168.1.100:3456/api/v1/nodes/register/6104"; //+ecode;
    Serial.println(service_url);
    http.begin(service_url); //Specify the URL and certificate
    httpCode = http.GET();   //Make the request

    if (httpCode > 0)
    { //Check for the returning code

      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);

      const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
      DynamicJsonBuffer jsonBuffer(capacity);

      // Parse JSON object
      JsonObject &node_json = jsonBuffer.parseObject(payload);
      if (!node_json.success())
      {
        Serial.println(F("Parsing failed!"));
        return;
      }

      String node_id = node_json["user"].as<String>() + "/" + node_json["uuid"].as<String>();

      Serial.println("writing eeprom node_id:");
      for (int i = 0; i < node_id.length(); ++i)
      {
        EEPROM.write(173 + i, node_id[i]);
        Serial.print("Wrote Node: ");
        Serial.println(node_id[i]);
      }
      EEPROM.commit();
      connectMQTTServer(node_id);
    }
    else
    {
      Serial.println("Error on HTTP request");
    }

    http.end();
  }
  else
  {
    Serial.println("Error on HTTP request");
  }
}

void loop()
{
  //server.handleClient();
  delay(10);
  client.loop();

  if (!client.connected())
  {
    /*Serial.println("reading EEPROM gateway_id");
    String gateway_id = "";
    for (int i = 100; i < 173; ++i)
    {
      gateway_id += char(EEPROM.read(i));
    }*/
    String gateway_id = "de6f0ee0-87c8-11e9-b83e-6f3b2914d082/f6b00f80-86ca-11e9-adbc-7b8e4b4feb60";
    connectMQTTServer(gateway_id);
    Serial.println("reading EEPROM node_id");
    /*String node_id = "";
    for (int i = 173; i < 246; ++i)
    {
      node_id += char(EEPROM.read(i));
    }*/
  }

  if (millis() - lastSendTime > INTERVAL)
  {
    lastSendTime = millis();
    send();
  }
  receive();
}

/*End Master*/
