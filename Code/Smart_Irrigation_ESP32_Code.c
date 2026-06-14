/********************************************************
 SMART IRRIGATION SYSTEM USING IoT
 Developed By: Rohit Arya
 Controller: ESP32
********************************************************/

#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Smart Irrigation"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

/******** Pin Definitions ********/
#define SOIL_SENSOR 34
#define RELAY_PIN 26
#define DHTPIN 4
#define DHTTYPE DHT11

/********************************/
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

/******** Variables ********/
int moistureValue;
int threshold = 2000;
bool manualMode = false;

/***************************/
/******** Manual Pump Control ********/
BLYNK_WRITE(V4)
{
  int state = param.asInt();
  manualMode = true;
  if(state == 1)
  {
    digitalWrite(RELAY_PIN, LOW);
    Blynk.virtualWrite(V3, 255);
    Serial.println("Pump ON (Manual)");
  }
  else
  {
    digitalWrite(RELAY_PIN, HIGH);
    Blynk.virtualWrite(V3, 0);
    Serial.println("Pump OFF (Manual)");
  }
}

/**************************************/
void sendSensorData()
{
  /******** DHT11 ********/
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  /******** Soil Moisture ********/
  moistureValue = analogRead(SOIL_SENSOR);
  /******** Serial Monitor ********/
  Serial.println("===========");
  Serial.print("Moisture : ");
  Serial.println(moistureValue);
  Serial.print("Temperature : ");
  Serial.println(temperature);
  Serial.print("Humidity : ");
  Serial.println(humidity);
  /******** Blynk Upload ********/
  Blynk.virtualWrite(V0, moistureValue);
  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, humidity);
  /******** Automatic Irrigation ********/
  if(!manualMode)
  {
    if(moistureValue > threshold)
    {
      digitalWrite(RELAY_PIN, LOW);
      Blynk.virtualWrite(V3, 255);
      Serial.println("Dry Soil");
      Serial.println("Pump ON");
    }
    else
    {
      digitalWrite(RELAY_PIN, HIGH);
      Blynk.virtualWrite(V3, 0);
      Serial.println("Wet Soil");
      Serial.println("Pump OFF");
    }
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  dht.begin();
  Blynk.begin(
      BLYNK_AUTH_TOKEN,
      ssid,
      pass);
  timer.setInterval(
      2000L,
      sendSensorData);
}

void loop()
{
  Blynk.run();
  timer.run();
}