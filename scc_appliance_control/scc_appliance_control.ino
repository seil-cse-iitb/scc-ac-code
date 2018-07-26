#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>

const char* ssid = "SEIL";
const char* password = "deadlock123";
const char* mqtt_server = "10.129.149.9";
const char* mqtt_username = "<MQTT_BROKER_USERNAME>";
const char* mqtt_password = "<MQTT_BROKER_PASSWORD>";
const char* mqtt_topic = "nodemcu/SCC/app_control";
const char* client_id = "appliance_controller";
// MQTT msg  = F1N/FAF
int fans[] = {D0, D1, D2, D3};
int lights[] = {D4, D5, D6, D7};

const byte fanAddress[] = {1, 2, 3, 4};
const byte lightAddress = 0;

byte fansStatus[] = {0, 0, 0, 0};
byte lightsStatus = 0;

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
  Serial.print("Message from topic [");
  Serial.print(topic);
  Serial.println("]");

  char msg[10] = {0};

  // char* temp = char*(payload);
  for (int index = 0; index < length; index++)
  {
    Serial.print((char)payload[index]);
    msg[index] = (char)payload[index];
  }

  Serial.println();
  Serial.println(msg);

  char appliance = '\0';

  if (msg[0] == 'F')
  {
    if (msg[2] == 'N')
    {
      if (msg[1] == 'A')
      {
        Serial.println("Starting all fans");
        for (int i = 0; i < sizeof(fans) / sizeof(int); i++)
        {
          Serial.print(fans[i]);
          digitalWrite(fans[i], HIGH);
          EEPROM.write(fanAddress[i], (byte)1);
          EEPROM.commit();
        }
      }
      else
      {
        Serial.print("Starting fan number ");
        Serial.println(fans[(int)msg[1] - 48]);
        digitalWrite(fans[(int)msg[1] - 48], HIGH);
        EEPROM.write(fanAddress[(int)msg[1] - 48], (byte)1);
        EEPROM.commit();
      }
    }
    else if (msg[2] == 'F')
    {
      if (msg[1] == 'A')
      {
        Serial.println("Stopping all fans");
        for (int i = 0; i < sizeof(fans) / sizeof(int); i++)
        {
          Serial.print(fans[i]);
          digitalWrite(fans[i], LOW);
          EEPROM.write(fanAddress[i], (byte)0);
          EEPROM.commit();
        }
      }
      else
      {
        Serial.print("Turning off fan number ");
        Serial.println(fans[(int)msg[1] - 48]);
        digitalWrite(fans[(int)msg[1] - 48], LOW);
        EEPROM.write(fanAddress[(int)msg[1] - 48], (byte)0);
        EEPROM.commit();
      }
    }
  }
  else if (msg[0] == 'L')
  {
    if (msg[2] ==  'N')
    {
      Serial.println("Turning on all lights");
      for (int i = 0; i < sizeof(lights) / sizeof(int); i++)
      {
        Serial.print(lights[i]);
        digitalWrite(lights[i], HIGH);
      }
      EEPROM.write(0, (byte)1);
      EEPROM.commit();
      Serial.println();
    }
    else if (msg[2] == 'F')
    {
      Serial.println("Turning off all lights");
      for (int i = 0; i < sizeof(lights) / sizeof(int); i++)
      {
        Serial.print(lights[i]);
        digitalWrite(lights[i], LOW);
      }
      EEPROM.write(0, (byte)0);
      EEPROM.commit();
      Serial.println();
    }
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

void initLights()
{
  lightsStatus = EEPROM.read(0);

  Serial.print("EEPROM status is ");
  Serial.println(EEPROM.read(0));

  for (int i = 0; i < 4; i++) digitalWrite(lights[i], lightsStatus);

  if (lightsStatus ==  1)
  {
    Serial.println("Turning on all lights");
    for (int i = 0; i < sizeof(lights) / sizeof(int); i++) Serial.print(lights[i]);
    Serial.println();
  }
  else if (lightsStatus == 0)
  {
    Serial.println("Turning off all lights");
    for (int i = 0; i < sizeof(lights) / sizeof(int); i++) Serial.print(lights[i]);
    Serial.println();
  }
}

void initFans()
{
  for (int i = 0; i < sizeof(fans) / sizeof(int); i++)
  {
    fansStatus[i] = EEPROM.read(fanAddress[i]);
    digitalWrite(fans[i], fansStatus[i]);
    Serial.print(fansStatus[i]);
    Serial.print(",");
  }
  Serial.println();
}

void setup()
{
  EEPROM.begin(512);

  Serial.begin(115200);
  Serial.println("____Indevidual Appliance Control____");

  for (int i = 0; i < 8; i++) 
  {
    if(i < 4) pinMode(fans[i], OUTPUT);
    else pinMode(lights[i], OUTPUT);
    delay(50);
  }

  setupWifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Serial.println(mqtt_topic);

  Serial.println("Initializing lights and fans");
  initLights();

  initFans();
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}
