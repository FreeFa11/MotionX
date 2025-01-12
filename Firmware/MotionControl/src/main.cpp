#include <Arduino.h>
#include <freertos/FreeRTOS.h>


#include <Wireless.h>
#include <Sensor.h>

BLEHID myHID;
Sensor mySensor;


void setup()
{
  Serial.begin(115200);

  pinMode(2, INPUT);
  gpio_pulldown_en(GPIO_NUM_2);

  mySensor.InitializeIMU();
  myHID.InitHID();
}

void loop()
{
  static float VA, VB, VC;
  static int8_t XX,YY;

  mySensor.UpdateData();
  mySensor.CalculateOrientation(&VA, &VB, &VC);
  // mySensor.CalculateVelocity(&VA, &VB, &VC);

  // VA /= 600;
  // VB /= 600;

  if ((VA < 127.) && (VA > -127.))
  {
    XX = int8_t(VA);
  }
  else if (VA)
  {
    XX = 127;
  }
  else
  {
    XX = -127;
  }
  if ((VB < 127.) && (VB > -127.))
  {
    YY = int8_t(VB);
  }
  else if (VB)
  {
    YY = 127;
  }
  else
  {
    YY = -127;
  }


  Serial.print(">X:");
  Serial.println(XX);
  Serial.print(">Y:");
  Serial.println(YY);

  myHID.Move(XX, YY, 0);

  if (digitalRead(2))
  {
    myHID.ClickRight();
  }


  vTaskDelay(10 / portTICK_RATE_MS);
}