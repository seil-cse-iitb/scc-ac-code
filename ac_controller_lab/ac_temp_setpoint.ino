#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "lab_ac_codes.h"
//#include "scc_ac_codes.h"

const char* ssid = "SEIL";
const char* password = "deadlock123";
const char* mqtt_server = "10.129.149.9";
const char* mqtt_username = "<MQTT_BROKER_USERNAME>";
const char* mqtt_password = "<MQTT_BROKER_PASSWORD>";
const char* mqtt_topic_ac_setpoint = "kresit/seil/ac/set_point";
const char* client_id = "AC_SET_POINT_NODE_1";

const int ac_id = 1;

WiFiClient espClient;
PubSubClient client(espClient);

IRsend irsend(D2);

int khz = 38;

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message from topic [");
  Serial.print(topic);
  Serial.print("] : ");
  Serial.print(payload[0]);
  Serial.println(payload[1]);
  
  int ac_id_recv = char(payload[0]) - '0';
  int set_point = char(payload[1] - '0');

  if(ac_id_recv == ac_id) ac_setpoint(set_point);
}

void ac_setpoint(char setPoint)
{
  //  if (setPoint != 0)
  //  {
  //    Serial.println("Turning ON AC");
  //    irsend.sendRaw(Signal_ON, sizeof(Signal_ON) / sizeof(uint16_t), khz);
  //    delay(2000);
  //  }
  switch (setPoint)
  {
    case 0:
      Serial.println("Turning OFF AC");
      irsend.sendRaw(Signal_OFF, sizeof(Signal_OFF) / sizeof(uint16_t), khz);
      break;

    case 1:
      Serial.println("Turning ON at 21");
      irsend.sendRaw(Signal_ON_21, sizeof(Signal_ON_21) / sizeof(uint16_t), khz); // Sends ON code;
      Serial.println("Turning ON");
      break;

    case 2:
      Serial.println("Turning ON at 22");
      irsend.sendRaw(Signal_ON_22, sizeof(Signal_ON_22) / sizeof(uint16_t), khz); // Sends ON code;
      Serial.println("Turning ON");
      break;

    case 3:
      Serial.println("Turning ON at 23");
      irsend.sendRaw(Signal_ON_23, sizeof(Signal_ON_23) / sizeof(uint16_t), khz); // Sends ON code;
      Serial.println("Turning ON");
      break;

    case 4:
      Serial.println("Turning ON at 24");
      irsend.sendRaw(Signal_ON_24, sizeof(Signal_ON_24) / sizeof(uint16_t), khz); // Sends ON code;
      Serial.println("Turning ON");
      break;

    case 5:
      Serial.println("Turning ON at 25");
      irsend.sendRaw(Signal_ON_25, sizeof(Signal_ON_25) / sizeof(uint16_t), khz); // Sends ON code;
      Serial.println("Turning ON");
      break;

    case 6:
      Serial.println("Turning ON at 26");
      irsend.sendRaw(Signal_ON_26, sizeof(Signal_ON_26) / sizeof(uint16_t), khz); // Sends ON code;
      Serial.println("Turning ON");
      break;


    case 7:
      Serial.println("Turning ON at 27");
      irsend.sendRaw(Signal_ON_27, sizeof(Signal_ON_27) / sizeof(uint16_t), khz); // Sends ON code;
      Serial.println("Turning ON");
      break;

    case 8:
      Serial.println("Turning ON at 28");
      irsend.sendRaw(Signal_ON_28, sizeof(Signal_ON_28) / sizeof(uint16_t), khz); // Sends ON code;
      Serial.println("Turning ON");
      break;

    default:
      Serial.println("Not a proper setpoint");
      break;
  }
}

void setupWifi()
{
  delay(10);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to WiFi with IP ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Connecting to MQTT....");

    if (client.connect(client_id))//, mqtt_username, mqtt_password))
    {
      Serial.println("Connected");
      client.subscribe(mqtt_topic_ac_setpoint);
      //Serial.println(mqtt_topic);
    }
    else
    {
      Serial.print("Failed to connect");
      //Serial.println(client.state());
      delay(5000);
    }
  }
}

void setup()
{
  irsend.begin();

  Serial.begin(115200);
  Serial.println("____ESP IR TESTING____");

  setupWifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();
}
