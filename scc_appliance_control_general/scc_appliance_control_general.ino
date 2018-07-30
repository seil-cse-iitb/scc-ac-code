#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include "config.h"

const char* ssid = "SEIL";
const char* password = "deadlock123";
const char* mqtt_server = "10.129.149.9";
const char* mqtt_username = "<MQTT_BROKER_USERNAME>";
const char* mqtt_password = "<MQTT_BROKER_PASSWORD>";
//const char* mqtt_topic = "nodemcu/SCC/app_control";
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

void actuation()
{
  Serial.println("Switching appliance");
  for (int i = 0; i < 8; i++)
  {
    Serial.print(config205.appliance_status[i]);
    Serial.print(",");
    if (config205.appliance_status[i] == 1) digitalWrite(config205.appliance_pins[i], LOW);
    else if (config205.appliance_status[i] == 0) digitalWrite(config205.appliance_pins[i], HIGH);
    EEPROM.write(config205.eeprom_address[i], config205.appliance_status[i]);
    EEPROM.commit();
  }
  Serial.println("\n");
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

  //  appliance = tolower(appliance_location[0]);
  appliance_number = (int)appliance_location[1] - 49;
  appliance = appliance_location;
  action = tolower((char)payload[0]);
  all_appliances = tolower((char)payload[1]);
  command = (int)payload[2] - 48;



  if (appliance_location != NULL)   // Controlling single appliance
  {
    for (int i = 0; i < 8; i++)
    {
      if (appliance == config205.appliances[i])
      {
        Serial.print("Configuring appliance ");
        Serial.println(config205.appliances[i]);
        appliance_index = i;
        if (action == 's' && command == 1)
          config205.appliance_status[i] = 1;
        else if (action == 's' && command == 0)
          config205.appliance_status[i] = 0;
        else if (action == 'r')
          Serial.println("In regulation mode");
        else
          Serial.println("Enter a Valid Mode");
      }
    }
  }
  else
  {
    if (all_appliances == 'f')
    {
      Serial.println("Burst Mode Bitches, FANS");
      if (command == 1)
      {
        for (int i = 4; i < 4 + config205.total_fans; i++) config205.appliance_status[i] = 1;
      }
      else
      {
        for (int i = 4; i < 4 + config205.total_fans; i++) config205.appliance_status[i] = 0;
      }
    }
    else if (all_appliances == 'l')
    {
      Serial.println("Burst Mode Bitches, LILGHTS");
      if (command == 1)
      {
        for (int i = 0; i < config205.total_lights; i++) config205.appliance_status[i] = 1;
      }
      else
      {
        for (int i = 0; i < config205.total_lights; i++) config205.appliance_status[i] = 0;
      }
    }
  }

  actuation();
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

  for (int i = 0; i < 8; i++) pinMode(config205.appliance_pins[i], OUTPUT);
  delay(100);

  for (int i = 0; i < 8; i++)
  {
    if (EEPROM.read(config205.eeprom_address[i]) == 1)
      digitalWrite(config205.appliance_pins[i], LOW);
    if (EEPROM.read(config205.eeprom_address[i]) == 0)
      digitalWrite(config205.appliance_pins[i], HIGH);
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
