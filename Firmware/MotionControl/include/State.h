#include <Arduino.h>
#include <Sensor.h>
#include <Wireless.h>


#define THRESHOLD 300
uint32_t Ticks;


enum STATE {Default, Press, Release};
struct StateMachine
{
    STATE state;
};

void RunStateMachine(StateMachine &SM, BLEHID &HD)
{
  switch (SM.state)
  {

  case STATE::Default:
    if (analogRead(RingFingerPin) < THRESHOLD)
    {
        SM.state = STATE::Press;
    }
    else if (analogRead(IndexFingerPin) < THRESHOLD)
    {
        HD.ClickLeft();
    }
    break;

  case STATE::Press:
    Ticks = xTaskGetTickCount();
    while (analogRead(RingFingerPin) < THRESHOLD)
    {
        if (analogRead(MiddleFingerPin) < THRESHOLD)
        {
            USBSerial.println("2 Press");
        }
        else
        {
            USBSerial.println("1 Press");
        }
    }
    SM.state = STATE::Release;
    break;

  case STATE::Release:
    if((xTaskGetTickCount() - Ticks) < 150)
    {
        HD.Press(CONTROLKEY::Backspace);
        HD.SendKeys();
    }
    SM.state = STATE::Default;
    break;

  default:
    USBSerial.println("State Error!!");
    ESP.restart();
    break;
  }
}
