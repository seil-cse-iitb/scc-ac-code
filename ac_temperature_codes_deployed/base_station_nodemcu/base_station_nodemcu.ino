#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(D1);
DallasTemperature sensors(&oneWire);

/*
  const char* ssid = "Pi_AP_305";
  const char* password = "Raspberry@305";
  const char* mqtt_server = "10.129.23.142";
*/

const char* ssid = "Pi_AP";
const char* password = "Raspberry";
//const char* mqtt_server = "10.129.26.243";
const char* mqtt_server = "10.129.149.64";

const char* mqtt_username = "<MQTT_BROKER_USERNAME>";
const char* mqtt_password = "<MQTT_BROKER_PASSWORD>";

const char* mqtt_topic = "nodemcu/SCC/AC";
const char* mqtt_topic_temp = "nodemcu/SCC/TEMP";
const char* mqtt_topic_sss = "nodemcu/SCC/SSS";
const char* client_id = "ACTempNode@205";

WiFiClient espClient;
PubSubClient client(espClient);

const int pinCE = D3;
const int pinCSN = D4;
RF24 radio(pinCE, pinCSN);

//const uint64_t wAddress = 0x00001E5000LL;
const uint64_t wAddress = 0x00001E5205LL;   // Check the address before putting code in different controllers
//const uint64_t wAddress = 0x0000E5201BLL;
//const uint64_t wAddress = 0x00001E5305LL;     // This is for classroom 305

char data[2];

const int ledPin = 4;

int count = 0;
long time1 = 0;

const int sssPin = D2;
int sss = 0;

void radioWrite()
{
  //Serial.println(data);
  int failCount = 0;

  for (int i = 0; i < 5; i++)
  {
    Serial.println("Inside radioWrite()");
    while (!radio.write(&data, sizeof(data)))
    {
      if (failCount < 15)
      {
        Serial.println("Failed");
        failCount = 0;
        break;
      }
      failCount++;
    }
    delay(100);
  }
}

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message from topic [");
  Serial.print(topic);
  Serial.println("]");
  Serial.println((char)payload[2]);
  data[0] = payload[2];

  if (sss == 0) radioWrite();
  else Serial.println("SSS = 1");
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Connecting to MQTT....");

    if (client.connect(client_id))//, mqtt_username, mqtt_password))
    {
      Serial.println("Connected");
      client.subscribe(mqtt_topic);
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

void setupRadio()
{
  radio.begin();
  //radio.setAutoAck(1);
  radio.setRetries(15, 15);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MIN);
  radio.setChannel(105);
  radio.openWritingPipe(wAddress);
  radio.stopListening();
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  setupWifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  setupRadio();

  sensors.begin();

  time1 = millis();

  pinMode(sssPin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected())
  {
    reconnect();
  }

  if (millis() - time1 > 2000)
  {
    sss = digitalRead(sssPin);
    String sssString = String(sss);
    char sssCharArray[2];

    sssString.toCharArray(sssCharArray, 2);

    client.publish(mqtt_topic_sss, sssCharArray);

    int sensorCount = sensors.getDeviceCount();
    Serial.println(sensorCount);
    sensors.requestTemperatures();

    float temperatureValue[sensorCount];
    char mqttData[25];
    char tempData[6];
    String addressString;
    char test[6];

    DeviceAddress sensorAddress[sensorCount];

    for (int i = 0; i < sensorCount; i++)
    {
      sensors.getAddress(sensorAddress[i], i);

      for (uint8_t j = 0; j < 8; j++)
      {
        if (j == 0)
        {
          addressString = String(sensorAddress[i][j], HEX);
          //Serial.print(sensorAddress[i][j], HEX);

          mqttData[j] = sensorAddress[i][j];
          mqttData[j + 1] = '-';
          //Serial.print("-");
          addressString += '-';
          //Serial.print(addressString);
        }
        else if (j != 1)
        {
          if (sensorAddress[i][8 - j] < 16)
          {
            //Serial.print("0");
            addressString += '0';
          }

          //Serial.print(sensorAddress[i][8 - j], HEX);
          addressString += String(sensorAddress[i][8 - j], HEX);

          mqttData[8 - j] = sensorAddress[i][8 - j];
        }

        delay(10);
      }

      float temp = sensors.getTempC(sensorAddress[i]);
      String tempString = String(temp);
      addressString += " ";
      addressString += tempString;

      Serial.print(addressString);
      addressString.toCharArray(mqttData, 25);
      Serial.print(" ");
      //Serial.print(sensors.getTempC(sensorAddress[i]));
      Serial.println();
      client.publish(mqtt_topic_temp, mqttData);
    }
    time1 = millis();
  }
  client.loop();
}
