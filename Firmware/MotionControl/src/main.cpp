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
  mySensor.InitializeIMU();
  mySensor.InitializeHall();
  myState.state = STATE::Default;

  pinMode(2, INPUT);
  gpio_pulldown_en(GPIO_NUM_2);
}

void loop()
{
  static float VA, VB, VC;
  static float DA, DB, DC;
  static int8_t XX,YY;

  mySensor.UpdateData();
  mySensor.CalculateOrientation(&VA, &VB, &VC);
  mySensor.GetRawData(&DC, &DA, &DB);
  // mySensor.CalculateVelocity(&DA, &DB, &DC);
  
  DA /= 600;
  DB /= 600;
  if ((DA < 127.) && (DA > -127.))
  {
    YY = int8_t(-DA);
  }
  else if (DA)
  {
    YY = -127;
  }
  else
  {
    YY = 127;
  }
  if ((DB < 127.) && (DB > -127.))
  {
    XX = int8_t(-DB);
  }
  else if (DB)
  {
    XX = -127;
  }
  else
  {
    XX = 127;
  }


  RunStateMachine(myState, myHID);
  myHID.Move(XX, YY, 0);




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
//   mySensor.GetRawData(&GX, &GY, &GZ);
//   mySensor.CalculateOrientation(&DA, &DB, &DC);
//   // mySensor.CalculateVelocity(&DA, &DB, &DC);

//   myData["IF"] = analogRead(IndexFingerPin);
//   myData["MF"] = analogRead(MiddleFingerPin);
//   myData["RF"] = analogRead(RingFingerPin);
//   myData["GX"] = GX;
//   myData["GY"] = GY;
//   myData["GZ"] = GZ;
//   myData["PI"] = DA;
//   myData["RO"] = DB;
//   myData["YA"] = DC;


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
