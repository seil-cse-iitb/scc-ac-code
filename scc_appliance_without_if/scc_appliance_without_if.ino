#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include "config_205.h"

const char* ssid = "SEIL";
const char* password = "deadlock123";
const char* mqtt_server = "10.129.149.9";
const char* mqtt_username = "<MQTT_BROKER_USERNAME>";
const char* mqtt_password = "<MQTT_BROKER_PASSWORD>";
const char* mqtt_topic = "actuation/kresit/2/205/#";
const char* client_id = "appliance_controller";
// MQTT msg  = F1N/FAF

WiFiClient espClient;
PubSubClient client(espClient);

void setupWifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to WiFi with IP address ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length)
{
  /*
    Serial.print("Message from topic [");
    Serial.print(topic);
    Serial.print("] : ");
    Serial.println(strlen(topic));
  */
  int topicLength = strlen(topic);

  char action;
  //  char appliance;
  int appliance_number;
  int command;
  String appliance;
  int appliance_index;
  char all_appliances;

  char* function;
  char* location;
  char* wing;
  char* roomID;
  char* appliance_location;

  function = strtok(topic, "/");
  location = strtok(NULL, "/");
  wing = strtok(NULL, "/");
  roomID = strtok(NULL, "/");
  appliance_location = strtok(NULL, "/");

  Serial.println("Decoding topic");
  Serial.print("\tfunction: ");
  Serial.println(function);
  Serial.print("\tlocation: ");
  Serial.println(location);
  Serial.print("\twing: ");
  Serial.println(wing);
  Serial.print("\troomID: ");
  Serial.println(roomID);
  Serial.print("\tappliance_location: ");
  Serial.println(appliance_location);
  Serial.println("Done decoding");

  action = tolower((char)payload[0]);
  command = (int)payload[1] - 48;

  int index = 0;

  while (strcmp(appliance_location, appliances[index].locations) && (index < 10))
  {
    index++;
  }

  for (int i = 0; appliances[index].pins[i] != 0; i++)
  {
    if (command == 1) digitalWrite(appliances[index].pins[i], LOW);
    if (command == 0) digitalWrite(appliances[index].pins[i], HIGH);
    appliances[index].states[i] = command;
    if (index == 9)
      EEPROM.write(i + 4, (byte)command);
    else
      EEPROM.write(i, (byte)command);
    EEPROM.commit();
  }
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Connecting to MQTT...");

    if (client.connect(client_id))
    {
      Serial.println("Connected");
      client.subscribe(mqtt_topic);
    }
    else
    {
      Serial.print("Failed to connect");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void setup()
{
  EEPROM.begin(512);

  Serial.begin(115200);
  Serial.println("____Indevidual Appliance Control____");

  int index = 0;
  while (index < 7)
  {
    pinMode(appliances[index].pins[0], OUTPUT);
    index++;
  }
  delay(100);

  for (int i = 0; i < 8; i++)
  {
    if (EEPROM.read(i) == 1)
      digitalWrite(appliances[i].pins[0], LOW);
    if (EEPROM.read(i) == 0)
      digitalWrite(appliances[i].pins[0], HIGH);
  }

  setupWifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Serial.println(mqtt_topic);

  Serial.println("Initializing lights and fans");
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}
