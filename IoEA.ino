//ThatsEngineering
//Sending Data from Arduino to NodeMCU Via Serial Communication
//Arduino code

//DHT11 Lib
//#include <DHT.h>

//Arduino to NodeMCU Lib
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//Initialise Arduino to NodeMCU (5=Rx & 6=Tx)
SoftwareSerial nodemcu(5, 6);

const int sampleWindow = 50;                              // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

float mq2 = A0;
float lm393 = A1;
//Initialisation of DHT11 Sensor
//#define DHTPIN 4
//DHT dht(DHTPIN, DHT11);
float adb;
float agc;

void setup() {
  Serial.begin(9600);

  //dht.begin();
  nodemcu.begin(9600);
  delay(1000);

  Serial.println("Program started");
}

void loop() {

  //StaticJsonBuffer<1000> jsonBuffer;
  //JsonObject& data = jsonBuffer.createObject();

  StaticJsonDocument<1000> doc;

  //Obtain Temp and Hum data
  getdata_func();


  //Assign collected data to JSON Object
  doc["Sound"] = adb;
  doc["AirQuality"] = agc; 

  //Send data to NodeMCU
  //data.printTo(nodemcu);
  //jsonBuffer.clear();

  serializeJson(doc, nodemcu);

  delay(2000);
}

void getdata_func() {

  //hum = dht.readHumidity();
  //temp = dht.readTemperature();
  //Serial.print("Humidity: ");
  //Serial.println(hum);
  //Serial.print("Temperature: ");
  //Serial.println(temp);

  unsigned long startMillis= millis();                   // Start of sample window
  float peakToPeak = 0;                                  // peak-to-peak level
 
  unsigned int signalMax = 0;                            //minimum value
  unsigned int signalMin = 1024;                         //maximum value
 
                                                          // collect data for 50 mS
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(lm393);                             //get reading from microphone
      if (sample < 1024)                                  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;                           // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;                           // save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;                    // max - min = peak-peak amplitude
   adb = map(peakToPeak,20,900,49.5,90);             //calibrate for deciBels
   //adb = sample;
  float sensorValue = analogRead(mq2);
  agc = sensorValue;
  
  //adb = db;
  //agc = gc

  Serial.print("Sound: ");
  Serial.println(adb);
  Serial.print("Air: ");
  Serial.println(agc);
}
