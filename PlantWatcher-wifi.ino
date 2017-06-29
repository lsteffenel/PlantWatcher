

/*
  PlantWatcher
  Reads an analog input on pin 0, determining if the plant needs to be watered or not
  If true, disconnect the sensor (sparing energy) and turns a relay on (pin 5)
  that control the water pump.
  After, connects to to the Internet and sends the measures to ThingSpeak; disconnect after
  Makes an idle for a 2h time period than relaunch
*/

#include <ThingSpeak.h>
#include <Wire.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>


int status = WL_IDLE_STATUS;

// ThingSpeak Settings
unsigned long myChannelNumber = <yourChannelId>;
const char * myWriteAPIKey = "<your write key>";
unsigned int MoistureFieldNumber = <channel field number>;

//Relay Shield transistor closes relay when D1 is HIGH
const int relayPin = D1;
// we assume that moisture is powered through D5
const int moisturePinPower = D5;

// Initialize Arduino Ethernet Client
WiFiClient client;



// the setup routine runs once when you press reset:
void setup() {
  
  delay(2000);
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  Serial.println("Waking from sleep");

  delay(2000);

  pinMode(relayPin, OUTPUT);
  pinMode(moisturePinPower, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  int sensorValue = 0;

  digitalWrite(BUILTIN_LED, HIGH);  // turn on LED with voltage HIGH
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setTimeout(180);


  do {
    // First, ready moisture, light on the pump

    digitalWrite(moisturePinPower, HIGH);   // turn the sensor power on (HIGH is the voltage level)
    delay(1000);
    // read the input on analog pin 0:
    analogRead(A0);
    delay(1000);
    sensorValue = analogRead(A0);
    Serial.flush();
    delay(100);
    Serial.println(sensorValue);
    Serial.flush();
    delay(100);
    digitalWrite(moisturePinPower, LOW); // turn the sensor power off (to spare electricity and to avoid corroding the sensor plates)


    int h = sensorValue;


    if (isnan(h)) {
      Serial.println("Failed to read from sensor!");
      return;
    }

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.println(" /1024 (actually this is a resistance value)");

    if (sensorValue > 250)
    {
      digitalWrite(relayPin, HIGH);   // turn the relay on (HIGH is the voltage level)
      Serial.println("relay");
      delay(5000); // pumps water during 5 secondes
      digitalWrite(relayPin, LOW);   // turn the relay off
    }



    // send measures to Internet

    // check for the presence of the shield:
    if (WiFi.status() == WL_NO_SHIELD) {
      Serial.println("WiFi shield not present");
      // don't continue:
    } else {
      // attempt to connect to Wifi network:

      //reset settings - for testing
      //wifiManager.resetSettings();
      //fetches ssid and pass from eeprom and tries to connect
      //if it does not connect it starts an access point with the specified name
      Serial.println(WiFi.SSID());

      //here  "AutoConnectAP"
      //and goes into a blocking loop awaiting configuration
      if (wifiManager.autoConnect("WeatherStationAP", "password"))
      {
        //if you get here you have connected to the WiFi
        Serial.println("connected...yeey :)");
        Serial.println("local ip");
        Serial.println(WiFi.localIP());
        Serial.println(WiFi.SSID());


        // wait 5 seconds for connection:
        delay(5000);

        ThingSpeak.begin(client);
        ThingSpeak.setField(MoistureFieldNumber, h);
        ThingSpeak.writeFields(myChannelNumber,  myWriteAPIKey);
        Serial.println("Sent to ThingSpeak...");

        Serial.flush();
      }
      else {
        Serial.println("Unable to connect to Wifi, watering plants anyway...");
      }
    }

    delay (60000); // let soil absorb water before next measurement

  } while (sensorValue > 250); // keep reading/watering until the moisture level is correct

  WiFi.persistent(false);
  WiFi.disconnect();

  Serial.println("My work is done. Going to deepsleep for a while");
  digitalWrite(BUILTIN_LED, LOW); 
  // deepSleep time is defined in microseconds. Multiply
  // seconds by 1e6
  int sleepTimeS = 60 * 60 * 1; //1 hour
  ESP.deepSleep(sleepTimeS * 1000000);



}

// the loop routine runs over and over again forever:
void loop() {




}

