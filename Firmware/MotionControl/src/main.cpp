#include <Arduino.h>
#include <freertos/FreeRTOS.h>

#include <ArduinoJson.h>



#include <Wireless.h>
#include <Sensor.h>
#include <Haptic.h>



BLEHID myHID;
Sensor mySensor;
Haptic myHaptic;


void setup()
{
  USBSerial.begin(115200);

  mySensor.InitializeIMU();
  // mySensor.CalibrateIMU();
  // myHID.InitHID();
}

void loop()
{
  JsonDocument myData;

  static float VA, VB, VC;
  static float DA, DB, DC;
  // static int8_t XX,YY;

  mySensor.UpdateData();
  // mySensor.GetRawData(&VA, &VB, &VC);
  // mySensor.CalculateOrientation();
  mySensor.CalculateOrientation(&DA, &DB, &DC);
  // mySensor.CalculateVelocity(&DA, &DB, &DC);

  

  // VA /= 600;
  // VB /= 600;

  // if ((VA < 127.) && (VA > -127.))
  // {
  //   XX = int8_t(-VA);
  // }
  // else if (VA)
  // {
  //   XX = -127;
  // }
  // else
  // {
  //   XX = 127;
  // }
  // if ((VB < 127.) && (VB > -127.))
  // {
  //   YY = int8_t(VB);
  // }
  // else if (VB)
  // {
  //   YY = 127;
  // }
  // else
  // {
  //   YY = -127;
  // }

  // myHID.Move(XX, YY, 0);


  USBSerial.print(">Roll:");
  USBSerial.println(DA);
  USBSerial.print(">Pitch:");
  USBSerial.println(DB);
  USBSerial.print(">Yaw:");
  USBSerial.println(DC);



  vTaskDelay(10 / portTICK_RATE_MS);
}



// ****************************************************Collection***************************************************************//

// #include <Arduino.h>
// #include <freertos/FreeRTOS.h>


// #include <ArduinoJson.h>

// #include <Wireless.h>
// #include <Sensor.h>


// Sensor mySensor;


// void setup()
// {
//   USBSerial.begin(115200);

//   mySensor.InitializeIMU();
//   // mySensor.CalibrateIMU();

//   // 11 16 17
//   analogSetAttenuation(ADC_2_5db);
  
//   pinMode(11, INPUT);
//   pinMode(16, INPUT);
//   pinMode(17, INPUT);

// }

// void loop()
// {
//   JsonDocument myData;

//   static float GA, GB, GC;
//   static float DA, DB, DC;

//   mySensor.UpdateData();
//   mySensor.GetRawData(&GA, &GB, &GC);
//   mySensor.CalculateOrientation(&DA, &DB, &DC);
//   // mySensor.CalculateVelocity(&DA, &DB, &DC);


//   myData["IF"] = analogRead(11);
//   myData["MF"] = analogRead(16);
//   myData["RF"] = analogRead(17);
//   myData["GX"] = GA;
//   myData["GY"] = GB;
//   myData["GZ"] = GC;
//   myData["RO"] = DB;
//   myData["PI"] = DA;
//   myData["YA"] = DC;


//   serializeJson(myData, USBSerial);
//   USBSerial.println();

//   vTaskDelay(10 / portTICK_RATE_MS);
// }
