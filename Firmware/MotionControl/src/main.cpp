// Includes
#include <Arduino.h>
#include <freertos/FreeRTOS.h>

#include <Sensor.h>
#include <Haptic.h>
#include <Wireless.h>
#include <ArduinoJson.h>



// Definitions
#define IndexUpTHRESHOLD      1000
#define IndexDownTHRESHOLD    400
#define MiddleUpTHRESHOLD     1300
#define MiddleDownTHRESHOLD   150
#define RingUpTHRESHOLD       1500
#define RingDownTHRESHOLD     300
uint32_t Ticks;
enum STATE {Default, AllPress, IndexPress, RingPress, IndexRelease, RingRelease};
enum SWIPE {Left, Right, Up, Down};
struct StateMachine
{
    STATE state;
};


// Objects
BLEHID myHID;
Sensor mySensor;
Haptic myHaptic;
StateMachine myState;
Preferences myPreference;


// Objects from Companion App
float Sensitivity = 1.0;
bool HapticOn = true;
Pattern HapticMode = Pattern::Subtle;
uint32_t GestureOne = 1, GestureTwo = 2, GestureThree = 3, GestureFour = 4, GestureFive = 5;


// Functions
void ReadSavedData();
void Map2D(Sensor &Sens, BLEHID &HD);
void Scroll(Sensor &Sens, BLEHID &HD);
void RunStateMachine(StateMachine &SM, Sensor SN, BLEHID &HD);
void RunSwipe(Sensor &Sens);
void Swipe(SWIPE Direction);
// void RunGesture();
void RunKeyboardCombo(uint32_t Combo);


// Tasks & Queues
QueueHandle_t HapticQueue;
TaskHandle_t HapticTaskHandle;
void HapticTask(void *param);
TaskHandle_t GestureTaskHandle;


void setup()
{
  USBSerial.begin(115200);
  
  // Initializations
  myHID.Initialize();
  myHaptic.Initialize();
  mySensor.InitializeIMU(ACCEL_FS::A2G, GYRO_FS::G500DPS);
  mySensor.InitializeHall();
  myState.state = STATE::Default;
  ReadSavedData();
  
  // Queue
  HapticQueue = xQueueCreate(1, sizeof(Pattern));
  // GestureQueue = xQueueCreate(1, sizeof(uint8_t));
  // FeaturesQueue = xQueueCreate(4, sizeof(Feature));
  
  // Tasks
  if (HapticOn)
  {
    xTaskCreatePinnedToCore(
      HapticTask,
      "HapticTask",
      1500,
      NULL,
      1,
      &HapticTaskHandle,
      0
    );
  }
  
  // xTaskCreatePinnedToCore(
  //   GestureTask,
  //   "GestureTask",
  //   20000,
  //   NULL,
  //   1,
  //   &GestureTaskHandle,
  //   1
  // );
  
}

void loop()
{
  // // State based on fingers data
  // RunGesture();
  RunStateMachine(myState, mySensor, myHID);
  Map2D(mySensor, myHID);

// ***************************************************Testing****************************************//

  // float Pitch, Roll, Yaw;
  // float GX, GY, GZ;
  // mySensor.UpdateData();
  // mySensor.GetRawData(&GX, &GY, &GZ);
  // mySensor.CalculateOrientation(&Pitch, &Roll, &Yaw);

  // USBSerial.print(">GX:");
  // USBSerial.println(GX);
  // USBSerial.print(">GY:");
  // USBSerial.println(GY);
  // USBSerial.print(">GZ:");
  // USBSerial.println(GZ);

  // JsonDocument DataToApp;
  // DataToApp["X"] = 0;
  // DataToApp["Y"] = 0;
  // DataToApp["Z"] = 0;
  // DataToApp["Pitch"] = Pitch;
  // DataToApp["Roll"] = Roll;
  // DataToApp["Yaw"] = Yaw;

  // std::string Datastring;
  // serializeJson(DataToApp, Datastring);
  // myHID.WriteToApp(Datastring);

  // vTaskDelay(10 / portTICK_RATE_MS);
}





// Tasks
void HapticTask(void *param)
{
  Pattern Data;

  while (1)
  {
    if (xQueueReceive(HapticQueue, &Data, 0) == pdTRUE)
    {
      myHaptic.Vibrate(Data);
    }

    vTaskDelay(10 / portTICK_RATE_MS);
  }
}


// State Machine
void RunStateMachine(StateMachine &SM, Sensor SN, BLEHID &HD)
{
  switch (SM.state)
  {

  case STATE::Default:
    if (analogRead(RingFingerPin) < RingDownTHRESHOLD)
    {
      SM.state = STATE::RingPress;
    }
    else if (analogRead(IndexFingerPin) < IndexDownTHRESHOLD)
    {
      SM.state = STATE::IndexPress;
    }
    break;

  case STATE::AllPress:
  xQueueSend(HapticQueue, &HapticMode, 1);
  while (analogRead(RingFingerPin) < RingUpTHRESHOLD)
  {
      // ***ALL PRESS***
      USBSerial.println("AllPress");
      // Continuous Action Begin // 
      HD.ClickLeft();
      Map2D(SN, HD); 
      // Action End // 

      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    SM.state = STATE::Default;
    break;

  case STATE::IndexPress:
    Ticks = xTaskGetTickCount();
    SM.state = STATE::IndexRelease;
    while (analogRead(IndexFingerPin) < IndexUpTHRESHOLD)
    {
      if ((analogRead(RingFingerPin) < RingDownTHRESHOLD) && (analogRead(MiddleFingerPin) < MiddleDownTHRESHOLD))
      {
        SM.state = STATE::AllPress;
        break;
      }

      // ***INDEX PRESS***
      USBSerial.println("IndexPress");
      // Continuous Action Begin // 
      // Action End // 

      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    break;
    
  case STATE::RingPress:
    Ticks = xTaskGetTickCount();
    SM.state = STATE::RingRelease;
    while (analogRead(RingFingerPin) < RingUpTHRESHOLD)
    {
        if ((analogRead(IndexFingerPin) < IndexDownTHRESHOLD) && (analogRead(MiddleFingerPin) < MiddleDownTHRESHOLD))
        {
          SM.state = STATE::AllPress;
          break;
        }
        else if (analogRead(MiddleFingerPin) < MiddleDownTHRESHOLD)
        {
            // ***DOUBLE PRESS***
            USBSerial.println("TwoPress");
            // Continuous Action Begin // 
            // This for Scroll
            // Action End // 
          }
          else
          {
            // ***SINGLE PRESS***
            USBSerial.println("OnePress");
            // Continuous Action Begin // 
            RunSwipe(mySensor);
            // Action End // 
        }
        
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    break;

  case STATE::IndexRelease:
    if((xTaskGetTickCount() - Ticks) < 250)
      {
        // ***INDEX RELEASE***
        xQueueSend(HapticQueue, &HapticMode, 1);
        USBSerial.println("LeftClick");
        // Action Begin // 
        HD.ClickLeft();
        // Action End // 
      }
    SM.state = STATE::Default;
    break;

  case STATE::RingRelease:
    if((xTaskGetTickCount() - Ticks) < 250)
    {
        // ***RING RELEASE***
        xQueueSend(HapticQueue, &HapticMode, 1);
        USBSerial.println("RightClick");
        // Action Begin // 
        HD.ClickRight();
         // Action End // 
    }
    SM.state = STATE::Default;
    break;

  default:
    USBSerial.println("State Error!!");
    ESP.restart();
    break;
  }
}

// Functions
void ReadSavedData()
{
  myPreference.begin("AppData", true);

  if (myPreference.isKey("Sensitivity"))
  {
    Sensitivity = myPreference.getUInt("Sensitivity") / 100.f;
    HapticOn = myPreference.getBool("HapticOn");
    HapticMode = (Pattern)myPreference.getUInt("HapticMode");
  }

  if (myPreference.isKey("GestureOne"))
  {
    GestureOne = myPreference.getUInt("GestureOne");
    GestureTwo = myPreference.getUInt("GestureTwo");
    GestureThree = myPreference.getUInt("GestureThree");
    GestureFour = myPreference.getUInt("GestureFour");
    GestureFive = myPreference.getUInt("GestureFive");
  }

  myPreference.end();
}

void Map2D(Sensor &Sens, BLEHID &HD)
{
  static float Pitch, Roll, Yaw;
  static float GY, GZ, GX;
  static float X,Y;
  // static Feature thisFeature;

  Sens.UpdateData();
  // Sens.GetRawData(&thisFeature.AX, &thisFeature.AY, &thisFeature.AZ, &thisFeature.GX, &thisFeature.GY, &thisFeature.GZ);
  // xQueueSend(FeaturesQueue, &thisFeature, 0);
  
  Sens.CalculateOrientation(&Pitch, &Roll, &Yaw);
  Sens.GetRawData(&GX, &GY, &GZ);


  GY /= 800;
  GZ /= 500;
  if ((GY < 127.) && (GY > -127.))
  {
    Y = int8_t(-GY);
  }
  else if (GY)
  {
    Y = -127;
  }
  else
  {
    Y = 127;
  }
  if ((GZ < 127.) && (GZ > -127.))
  {
    X = int8_t(-GZ);
  }
  else if (GZ)
  {
    X = -127;
  }
  else
  {
    X = 127;
  }

  HD.Move(X * Sensitivity, Y * Sensitivity, 0);

  // Handle Data from App
  myHID.HandleAppData();
}

void Scroll(Sensor &Sens, BLEHID &HD)
{
  float Pitch, Roll, Yaw;
  Sens.CalculateOrientation(&Pitch, &Roll, &Yaw);

  HD.Move(0, 0, Pitch);
}

void RunSwipe(Sensor &Sens)
{
  float GX, GY, GZ;
  Sens.UpdateData();
  Sens.CalculateOrientation();
  Sens.GetRawData(&GX, &GY, &GZ);

  if (GZ > 20000)
  {
    Swipe(SWIPE::Left);
    vTaskDelay(150 / portTICK_RATE_MS);
  }
  else if (GZ < -20000)
  {
    Swipe(SWIPE::Right);
    vTaskDelay(150 / portTICK_RATE_MS);
  }
  else if (GY > 20000)
  {
    Swipe(SWIPE::Up);
    vTaskDelay(150 / portTICK_RATE_MS);
  }
  else if (GY < -20000)
  {
    Swipe(SWIPE::Down);
    vTaskDelay(150 / portTICK_RATE_MS);
  }

  vTaskDelay(10 / portTICK_RATE_MS);
}

void Swipe(SWIPE Direction)
{
  switch (Direction)
  {
  case SWIPE::Left:
    myHID.Press(MODIFIERKEY::LeftControl);
    myHID.Press(MODIFIERKEY::LeftGUI);
    myHID.Press(CONTROLKEY::RightArrow);
    myHID.SendKeys();
    myHID.Press(MODIFIERKEY::LeftControl);
    myHID.Press(MODIFIERKEY::LeftAlt);
    myHID.Press(CONTROLKEY::RightArrow);
    myHID.SendKeys();
    break;
  case SWIPE::Right:
    myHID.Press(MODIFIERKEY::LeftControl);
    myHID.Press(MODIFIERKEY::LeftGUI);
    myHID.Press(CONTROLKEY::LeftArrow);
    myHID.SendKeys();
    myHID.Press(MODIFIERKEY::LeftControl);
    myHID.Press(MODIFIERKEY::LeftAlt);
    myHID.Press(CONTROLKEY::LeftArrow);
    myHID.SendKeys();
    break;
  case SWIPE::Up:
    myHID.Press(MODIFIERKEY::LeftGUI);
    myHID.SendKeys();
  break;
  case SWIPE::Down:
    myHID.Press(MODIFIERKEY::LeftControl);
    myHID.Press(MODIFIERKEY::LeftAlt);
    myHID.Press("d");
    myHID.SendKeys();
    myHID.Press(MODIFIERKEY::LeftGUI);
    myHID.Press("d");
    myHID.SendKeys();
  break;
  
  default:
    break;
  }
}

// void RunGesture()
// {
//   if (xQueueReceive(GestureQueue, &Gesture, 1))
//   {
//     switch (Gesture)
//     {
//     case 1:
//       RunKeyboardCombo(GestureOne);
//       break;
//     case 2:
//       RunKeyboardCombo(GestureTwo);
//       break;
//     case 3:
//       RunKeyboardCombo(GestureThree);
//       break;
//     case 4:
//       RunKeyboardCombo(GestureFour);
//       break;
//     case 5:
//       RunKeyboardCombo(GestureFive);
//       break;
    
//     default:
//       break;
//     }
//   }
// }

void RunKeyboardCombo(uint32_t Combo)
{
  switch (Combo)
  {
  case 1:                                         // Alt + F4
    myHID.Press(MODIFIERKEY::LeftAlt);
    myHID.Press(CONTROLKEY::F4);
    myHID.SendKeys();
    break;
    case 2:                                       // Ctrl + H
    myHID.Press(MODIFIERKEY::LeftControl);
    myHID.Press("h");
    myHID.SendKeys();
    break;
    case 3:                                       // Ctrl + S
    myHID.Press(MODIFIERKEY::LeftControl);
    myHID.Press("s");
    myHID.SendKeys();
    break;
    case 4:                                       // Alt + Enter
    myHID.Press(MODIFIERKEY::LeftAlt);
    myHID.Press(CONTROLKEY::Enter);
    myHID.SendKeys();
    break;
    case 5:                                       // Backspace
    myHID.Press(CONTROLKEY::Backspace);
    myHID.SendKeys();
    break;
  
  default:
    break;
  }
}

// ****************************************************CaptureData***************************************************************//

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
