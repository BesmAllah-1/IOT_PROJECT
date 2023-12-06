/*
   -- pulse --
   
   This source code of graphical user interface 
   has been generated automatically by RemoteXY editor.
   To compile this code using RemoteXY library 3.1.11 or later version 
   download by link http://remotexy.com/en/library/
   To connect using RemoteXY mobile app by link http://remotexy.com/en/download/                   
     - for ANDROID 4.11.4 or later version;
     - for iOS 1.9.1 or later version;
    
   This source code is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.    
*/

//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__ESP8266_HARDSERIAL_CLOUD

#include <RemoteXY.h>

// RemoteXY connection settings 
#define REMOTEXY_SERIAL Serial
#define REMOTEXY_SERIAL_SPEED 115200
#define REMOTEXY_WIFI_SSID "OPPO A12"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_CLOUD_SERVER "cloud.remotexy.com"
#define REMOTEXY_CLOUD_PORT 6376
#define REMOTEXY_CLOUD_TOKEN "eded91617a26f5c853942b3dd5cb549f"


// RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =   // 37 bytes
  { 255,0,0,70,0,30,0,16,19,1,67,5,7,15,48,5,178,27,51,68,
  18,7,36,48,48,26,178,235,67,5,2,25,60,5,235,27,11 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // output variables
  char text_1[51];  // string UTF8 end zero 
  float onlineGraph_1_var1;
  float onlineGraph_1_var2;
  char text_2[11];  // string UTF8 end zero 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////


//////////////////////////////////////////////
//        MAX3010x include library          //
//////////////////////////////////////////////

#include <Wire.h>
#include "MAX30105.h"

#include "heartRate.h"

MAX30105 particleSensor;

const byte RATE_SIZE = 4;    //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE];       //Array of heart rates
byte rateSpot = 0;           // index for the rates[RATE_SIZE] array
long lastBeat = 0;           //Time at which the last beat occurred

float beatsPerMinute;   // BPM
int beatAvg;

/////////////////////////////////////////////
//           END MAX3010x include          //
/////////////////////////////////////////////


void setup() 
{
  RemoteXY_Init (); 
  
  
  // TODO you setup code

  // Initialize sensor
  // The sensor’s connection is checked, and if successful, an initialization sequence is run
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    while (1);   // Infinite loop if sensor not detected.
  }

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

}

void loop() 
{ 
  RemoteXY_Handler ();
  
  
  // TODO you loop code
  // use the RemoteXY structure for data transfer
  // do not call delay(), use instead RemoteXY_delay() 

  // infrared value reading
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;   // delta is the time difference between two successive beats
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    // The code also ensures the BPM falls within a realistic range before updating the average.
    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable : to ensure it has value only in the range: 0, 1, 2, 3 

      //Take average of readings over RATE_SIZE periods
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  
//  Serial.print(", BPM=");
//  Serial.print(beatsPerMinute);   // current single BPM
//  Serial.print(", Avg BPM=");
//  Serial.print(beatAvg);          // average BPM over the last RATE_SIZE periods

//  if (irValue < 50000)
//    Serial.print(" No finger?");

    sprintf (RemoteXY.text_1, "BPM is %d", (int)beatsPerMinute); // result: "Value is 1234"
    RemoteXY.onlineGraph_1_var1 = float(beatsPerMinute) /1000;
    sprintf (RemoteXY.text_2, "avg is %d", (int)beatAvg); // result: "Value is 1234"
    RemoteXY.onlineGraph_1_var2 = float(beatAvg)/1000;
    



//////////////////////////////////////////////////////////////////////////////////////////////////////
//           For moderate-intensity physical activity:
//           Your target heart rate should be between 64% and 76% of your maximum heart rate.       //
//           Your maximum heart rate can be estimates based on your age.                            //
//           max_heart_rate = 220 - age          (beats/minute)                                     //
//////////////////////////////////////////////////////////////////////////////////////////////////////



}
