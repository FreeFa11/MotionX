#include <Arduino.h>
#include <freertos/FreeRTOS.h>


#include <State.h>
#include <Sensor.h>
#include <Haptic.h>
#include <Wireless.h>
#include <ArduinoJson.h>

BLEHID myHID;
Sensor mySensor;
Haptic myHaptic;
StateMachine myState;



void setup()
{
  USBSerial.begin(115200);
 
  myHID.Initialize();
  myHaptic.Initialize();
  mySensor.InitializeIMU();
  mySensor.InitializeHall();
  myState.state = STATE::Default;

  // pinMode(2, INPUT);
  // gpio_pulldown_en(GPIO_NUM_2);
}

void loop()
{
  // State based on fingers data
  RunStateMachine(myState, mySensor, myHID, myHaptic);
  Map2D(mySensor, myHID);

// ***************************************************Testing****************************************//
  // USBSerial.print(">P:");
  // USBSerial.println(VA);
  // USBSerial.print(">R:");
  // USBSerial.println(VB);
  // USBSerial.print(">Y:");
  // USBSerial.println(VC);
  // USBSerial.print(">VX:");
  // USBSerial.println(DA);
  // USBSerial.print(">VY:");
  // USBSerial.println(DB);
  // USBSerial.print(">VZ:");
  // USBSerial.println(DC);

  // JsonDocument DataToApp;
  // DataToApp["Pitch"] = VA;
  // DataToApp["Roll"] = VB;
  // DataToApp["Yaw"] = VC;

  // std::string Datastring;
  // serializeJson(DataToApp, Datastring);
  // myHID.WriteToApp(Datastring);

  vTaskDelay(10 / portTICK_RATE_MS);
}



// ****************************************************Collection***************************************************************//

// #include <Arduino.h>
// #include <freertos/FreeRTOS.h>

// #include <ArduinoJson.h>
// #include <Sensor.h>


// Sensor mySensor;

// void setup()
// {
//   USBSerial.begin(115200);

//   mySensor.InitializeIMU();
//   mySensor.InitializeHall();
// }

// void loop()
// {
//   JsonDocument myData;

//   static float GX, GY, GZ;
//   static float DA, DB, DC;

//   mySensor.UpdateData();
//   mySensor.GetRawData(&DA, &DB, &DC, &GX, &GY, &GZ);

//   myData["IF"] = analogRead(IndexFingerPin);
//   myData["MF"] = analogRead(MiddleFingerPin);
//   myData["RF"] = analogRead(RingFingerPin);
//   myData["GX"] = GX;
//   myData["GY"] = GY;
//   myData["GZ"] = GZ;
//   myData["AX"] = DA;
//   myData["AY"] = DB;
//   myData["AZ"] = DC;


//   serializeJson(myData, USBSerial);
//   USBSerial.println();

//   vTaskDelay(10 / portTICK_RATE_MS);
// }


// ****************************************************Calibration***************************************************************//

// #include <Arduino.h>
// #include <freertos/FreeRTOS.h>

// #include <ArduinoJson.h>
// #include <Sensor.h>


// Sensor mySensor;

// void setup()
// {
//   USBSerial.begin(115200);

//   mySensor.InitializeIMU();
// }

// void loop()
// {
//   JsonDocument myData;

//   static float AX, AY, AZ;
//   static float GX, GY, GZ;

//   mySensor.UpdateData();
//   mySensor.GetRawData(&AX, &AY, &AZ, &GX, &GY, &GZ);

//   myData["AX"] = AX;
//   myData["AY"] = AY;
//   myData["AZ"] = AZ;
//   myData["GX"] = GX;
//   myData["GY"] = GY;
//   myData["GZ"] = GZ;


//   serializeJson(myData, USBSerial);
//   USBSerial.println();

//   vTaskDelay(10 / portTICK_RATE_MS);
// }
