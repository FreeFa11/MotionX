#include <Includes.h>

void setup() {
  // Temp Setup for IMU Power
  pinMode(42, OUTPUT);
  pinMode(41, OUTPUT);
  digitalWrite(42, HIGH);
  digitalWrite(41, LOW);
  Serial.begin(115200);

  SetupIMU();
  ReadBias(30);
  SetupHaptic();
}

void loop() {

  static float Roll, Pitch, Yaw;
  EstimateEulerAngles(&Pitch, &Roll, &Yaw);

  Serial.print(">Pitch:");
  Serial.println(Pitch);
  Serial.print(">Roll:");
  Serial.println(Roll);

  // static uint8_t count = 0;
  // WriteHaptic(count, 500);
  // vTaskDelay(1000 / portTICK_RATE_MS);
  // count +=50;
  // count %= 250;

  vTaskDelay(50 / portTICK_RATE_MS);
}