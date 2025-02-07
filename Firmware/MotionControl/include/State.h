#include <Arduino.h>
#include <Sensor.h>
#include <Wireless.h>
#include <Haptic.h>


#define THRESHOLD 300
uint32_t Ticks;

void Map2D(Sensor &Sens, BLEHID &HD);
void Scroll(Sensor &Sens, BLEHID &HD);


enum STATE {Default, AllPress, IndexPress, RingPress, IndexRelease, RingRelease};
struct StateMachine
{
    STATE state;
};


// State Machine
void RunStateMachine(StateMachine &SM, Sensor SN, BLEHID &HD, Haptic &HP)
{
  int Index, Middle, Ring;
  Index = analogRead(IndexFingerPin);
  Middle = analogRead(MiddleFingerPin);
  Ring = analogRead(RingFingerPin);

  switch (SM.state)
  {

  case STATE::Default:
    if (Ring < THRESHOLD)
    {
      SM.state = STATE::RingPress;
    }
    else if (Index < THRESHOLD)
    {
      HP.Vibrate(Pattern::Subtle);
      SM.state = STATE::IndexPress;
    }
    break;

  case STATE::AllPress:
    USBSerial.println("AllPress");
    while (analogRead(RingFingerPin) < THRESHOLD)
    {
      HD.ClickLeft();
      Map2D(SN, HD); 

      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    SM.state = STATE::Default;
    break;

  case STATE::IndexPress:
    Ticks = xTaskGetTickCount();
    SM.state = STATE::IndexRelease;
    while (analogRead(IndexFingerPin) < THRESHOLD)
    {
      if ((analogRead(RingFingerPin) < THRESHOLD) && (analogRead(MiddleFingerPin)))
      {
        SM.state = STATE::AllPress;
        break;
      }

      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    break;
    
  case STATE::RingPress:
    Ticks = xTaskGetTickCount();
    SM.state = STATE::RingRelease;
    while (analogRead(RingFingerPin) < THRESHOLD)
    {
        if ((analogRead(IndexFingerPin) < THRESHOLD) && (analogRead(MiddleFingerPin)))
        {
          SM.state = STATE::AllPress;
          break;
        }
        else if (analogRead(MiddleFingerPin) < THRESHOLD)
        {
            USBSerial.println("2 Press");
            // Scroll(SN, HD);
        }
        else
        {
            USBSerial.println("1 Press");
        }
        
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    break;

  case STATE::IndexRelease:
    if((xTaskGetTickCount() - Ticks) < 150)
      {
          HD.ClickLeft();

          HP.Vibrate(Pattern::Double);
      }
    SM.state = STATE::Default;
    break;

  case STATE::RingRelease:
    if((xTaskGetTickCount() - Ticks) < 150)
    {
        HD.Press(CONTROLKEY::Backspace);
        HD.SendKeys();

        HP.Vibrate(Pattern::Double);
    }
    SM.state = STATE::Default;
    break;

  default:
    USBSerial.println("State Error!!");
    ESP.restart();
    break;
  }
}


void Map2D(Sensor &Sens, BLEHID &HD)
{
  static float Pitch, Roll, Yaw;
  static float GY, GZ, GX;
  static float X,Y;

  Sens.UpdateData();
  Sens.CalculateOrientation(&Pitch, &Roll, &Yaw);
  Sens.GetRawData(&GX, &GY, &GZ);
  // Sens.CalculateVelocity(&DA, &DB, &DC);
  
  GY /= 600;
  GZ /= 600;
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

  HD.Move(X, Y, 0);
}

void Scroll(Sensor &Sens, BLEHID &HD)
{
  float Pitch, Roll, Yaw;
  Sens.CalculateOrientation(&Pitch, &Roll, &Yaw);

  HD.Move(0, 0, Pitch);
}
