#include <SoftwareSerial.h>
#include <ThingSpeak.h>
#include <ESP8266WiFi.h>

#define FLOW_SENSOR1 2  // Water flow sensor 1 connected to pin 2
#define FLOW_SENSOR2 3  // Water flow sensor 2 connected to pin 3
#define FLOW_SENSOR3 4  // Water flow sensor 3 connected to pin 4

const char *ssid = "Redmi Note 11T 5G";
const char *password = "12345678";
const char *thingSpeakApiKey = "9VYBZ8DIU84OP2RO";

// Initialize the ESP8266 client
WiFiClient client;

// Initialize the software serial for ESP8266 communication
SoftwareSerial esp8266(6, 7); // RX, TX

// Initialize the flow rate variables
volatile int flowRate1;
volatile int flowRate2;
volatile int flowRate3;
float flowRateTotal;

// Variables for calculations
unsigned long currentTime;
unsigned long previousTime = 0;
const int calibrationFactor1 = 4.5; // Adjust this value based on your sensor's calibration
const int calibrationFactor2 = 4.5; // Adjust this value based on your sensor's calibration
const int calibrationFactor3 = 4.5; // Adjust this value based on your sensor's calibration

void setup() {
  Serial.begin(9600);

  pinMode(FLOW_SENSOR1, INPUT);
  pinMode(FLOW_SENSOR2, INPUT);
  pinMode(FLOW_SENSOR3, INPUT);

  esp8266.begin(9600);
  ThingSpeak.begin(client);

  connectWiFi();
}

void loop() {
  currentTime = millis();

  // Read water flow rate from sensor 1
  if (digitalRead(FLOW_SENSOR1) == HIGH) {
    flowRate1++;
  }

  // Read water flow rate from sensor 2
  if (digitalRead(FLOW_SENSOR2) == HIGH) {
    flowRate2++;
  }

  // Read water flow rate from sensor 3
  if (digitalRead(FLOW_SENSOR3) == HIGH) {
    flowRate3++;
  }

  // Calculate total flow rate
  if (currentTime - previousTime >= 1000) {
    flowRateTotal = (flowRate1 / calibrationFactor1) + (flowRate2 / calibrationFactor2) + (flowRate3 / calibrationFactor3);

    // Update ThingSpeak
    updateThingSpeak(flowRateTotal);

    flowRate1 = 0;
    flowRate2 = 0;
    flowRate3 = 0;
    previousTime = currentTime;
  }
}

void connectWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }

  Serial.println("WiFi connected!");
}

void updateThingSpeak(float flowRate) {
  Serial.print("Updating ThingSpeak...Flow Rate: ");
  Serial.println(flowRate);

  ThingSpeak.setField(1, flowRate);
  int statusCode = ThingSpeak.writeFields(1, thingSpeakApiKey);

  if (statusCode == 200) {
    Serial.println("Update successful!");
  } else {
    Serial.print("Update failed, status code: ");
    Serial.println(statusCode);
  }

  delay(20000); // Update every 20 seconds
}
