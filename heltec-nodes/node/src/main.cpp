#include <SPI.h>
#include <LoRa.h>
#include "SSD1306.h"

SSD1306  display(0x3c, 4, 15);

#define SS      18
#define RST     14
#define DI0     26
#define BAND    915E6

//Constante para informar ao Slave que queremos os dados
const String GETDATA = "getdata";
//Constante que o Slave retorna junto com os dados para o Master
const String SETDATA = "setdata=";

const String SETSTATE = "setstate=";

typedef struct {
  int nlight;
  bool nstate;
}Data;

void setupDisplay() {
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);
  delay(50);
  digitalWrite(16, HIGH);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

void setupLoRa() {
  SPI.begin(5,19,27,18);
  LoRa.setPins(SS,RST,DI0);

  if (!LoRa.begin(BAND)) {
    display.drawString(5,25,"Starting LoRa failed!");
    while (1);
  }

  LoRa.enableCrc();
  LoRa.receive();
}


const int sensorPin = A0;
const int ledPin = 21;

double lightInit;  // initial value
double lightVal;   // light reading

bool node_state = false;

void setup(){
  Serial.begin(115200);
  setupDisplay();
  setupLoRa();

  lightInit = analogRead(sensorPin);
  pinMode(ledPin, OUTPUT);
  Serial.println(digitalRead(ledPin));
  digitalWrite(ledPin, HIGH);
  Serial.println(digitalRead(ledPin));

  /*  if (!analogRead(sensorPin))
  {
  display.clear();
  display.drawString(0, 0, "Sensor não encontrado");
  display.display();
  while(1);
}
*/lightInit = analogRead(sensorPin);
pinMode(ledPin, OUTPUT);
display.clear();
display.drawString(0, 0, "Node aguardando...");
display.display();
}

Data readData()
{
  Data data;
  data.nlight = analogRead(A0);
  data.nstate = digitalRead(21);

  Serial.println("Sainda analog");
  Serial.println(data.nlight * (5.0 / 1023.0));
  Serial.println("Sainda digital");
  Serial.println(data.nstate);

  return data;
}

void showSentData(Data data)
{
  //Mostra no display
  display.clear();
  display.drawString(0, 0, "Enviando:");
  if (data.nstate == 0)
  { display.drawString(0, 16, "Node LED ligado");}
  else {display.drawString(0, 16, "Node LED desligado");}
  display.drawString(0, 32,  String(data.nlight) + " lux");
  display.drawString(0, 48, "AME - Node 1");
  display.display();
}

void loop(){
  int packetSize = LoRa.parsePacket();


  if (packetSize == GETDATA.length())
  {
    String received = "";
    while(LoRa.available())
    {
      received += (char) LoRa.read();
    }
    Serial.println("\n"+received);

    if(received.equals(GETDATA))
    {
      Data data = readData();
      Serial.println("Criando pacote para envio");
      LoRa.beginPacket();
      LoRa.print(SETDATA);
      LoRa.write((uint8_t*)&data, sizeof(data));
      LoRa.endPacket();
      delay(2000);
      showSentData(data);
    }
  }

  if (packetSize > GETDATA.length())
  {
    String received = "";
    while(LoRa.available())
    {
      received += (char) LoRa.read();
    }
    Serial.println("\n"+received);

    if (strstr(received.c_str(), SETSTATE.c_str()))
    {
      Serial.println("Servidor enviou comando STATE");
      Data data = readData();
      LoRa.readBytes((uint8_t*)&data, sizeof(data));
      if (strstr(received.c_str(), "1"))
      {
        Serial.println("Acender led do node");
        digitalWrite (ledPin, LOW); //should be HIGH
        node_state = 1; //should be 1
      }
      else if (strstr(received.c_str(), "0"))
      {
        Serial.println("Apagar led do node");
        digitalWrite (ledPin, HIGH); //should be LOW
        node_state = 0; //should be 0
      }
      Serial.println(node_state);

    }
  }
}

/*Data data = readData();
if(data.light - lightInit <  50)
{
digitalWrite (ledPin, HIGH); // turn on light
}
else
{
digitalWrite (ledPin, LOW); // turn off light
}*/
