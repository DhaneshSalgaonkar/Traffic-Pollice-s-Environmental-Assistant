
//Include Lib for Arduino to Nodemcu
#include <SoftwareSerial.h>
#include <ArduinoJson.h>


//D6 = Rx & D5 = Tx
SoftwareSerial nodemcu(D6, D5);

/* 0- Reset Board */
    
    int countRST = 0;                                         /* Counting variable for reset function to trigger */
    int ResetPin = 16;                                        // Have to unpluged the cable to RST pin during uploading code !!! 
                                                              // GPIO 16 equals to pin D0 of Nodemcu board. 
    
/* 1- WSP8266 WiFi connection */
    #include <ESP8266WiFi.h>                                  // library for wifi configuration codes. Need to pre-install the Nodemcu board via board manager
                                                              // Kindly check instruction to install Nodemcu board at Solarduino website 
    String apiKey = "NEAPQV4ROXONQJWC";                       // Enter your Write API key from ThingSpeak
    const char* server = "api.thingspeak.com";                // In this project we are using MQTT broker: thingspeak.com
                                                              // key in your MQTT broker's server address. Do not edit if you are using thingspeak.                  
    const char* ssid = "sai";                 // key in your wifi-router name (can use handphone to check your wifi full name)
    const char* password =  "9757064466";                       // key in your wifi-router password
    WiFiClient client;                                        /* define the Node Mcu variable for wifi communication*/



    
void setup() 
{
    Serial.begin(9600);                                                     /* initiate Serial monitor with baud rate 9600 */
      nodemcu.begin(9600);
  while (!Serial) continue;   
                            
    /* 1- WSP8266 WiFi connection */

    WiFi.disconnect();                                                      /* disconnects any previous Wifi connection*/
    WiFi.begin(ssid, password);                                             /* initiate Wifi connection with your router */
    while (WiFi.status() != WL_CONNECTED)                                   /* if connection not completed, keep run the codes */
      {   
        delay(1000); Serial.println("Connecting to WiFi.."); 
            
        /* 0- Reset Board */ 
        countRST = countRST + 1;                                            /* add counting time, if waited too long (by default 1 minute), reset the board */
        if (countRST == 60)
        { digitalWrite(ResetPin, LOW); countRST = 0; }  
      }

    Serial.println("Connected to the WiFi network");                        /* once connection completed, continue do the following */              
    Serial.println(WiFi.localIP());                                         /* print IP address of Node Mcu on Serial Monitor */

}

 
void loop() 
{

    /* 1- WSP8266 WiFi connection */
          
    if (WiFi.status() != WL_CONNECTED)                                      /* if wifi connection not completed or disconnection */
          { 
            WiFi.begin(ssid, password);                                     /* initiate Wifi connection with your router */
            Serial.println("Reconnecting to WiFi.."); 
            delay(10000);                                                   /* let it has 10 second time for processing */
             
            /* 0- Reset Board */
            countRST = countRST + 1;                                        /* add counting time, if waited too long (by default 5 minutes), reset the board */
            if (countRST == 30)
            { digitalWrite(ResetPin, LOW); countRST = 0; }
          }

  StaticJsonDocument<1000> doc;
  DeserializationError error = deserializeJson(doc, nodemcu);

  Serial.println("JSON Object Recieved");
  Serial.print("Recieved SoundValue:  ");
  float adb = doc["Sound"];
  Serial.println(adb);
  Serial.print("Recieved AirValue:  ");
  float agc = doc["AirQuality"];
  Serial.println(agc);
  Serial.println("-----------------------------------------");

         
          /* 1- WSP8266 WiFi connection */
          
          if (client.connect(server,80))                                            // "184.106.153.149" or api.thingspeak.com
              {  
                String postStr = apiKey;
                postStr +="&field1=";
                postStr += String(adb);                                     /* If you have more than 1 sensor or field, kindly add like the example code */
                postStr +="&field2=";
                postStr += String(agc);           
                postStr += "\r\n\r\n";
             
                client.print("POST /update HTTP/1.1\n");                            // proper code that need to send to ThinkSpeak via REST API protocol 
                client.print("Host: api.thingspeak.com\n");
                client.print("Connection: close\n");
                client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                client.print("Content-Type: application/x-www-form-urlencoded\n");
                client.print("Content-Length: ");
                client.print(postStr.length());
                client.print("\n\n");
                client.print(postStr);        
              }
              
         client.stop();                                                             /* to terminate the send command */
         
}
