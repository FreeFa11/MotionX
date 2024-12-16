#include <Arduino.h>
#include <freertos/FreeRTOS.h>

#include <math.h>
#include <Wire.h>
#include <MPU6050.h>
#include <HMC5883L.h>


// Definitions
#define SDAWire             39
#define SCLWire             40

#define AlphaLPF            0.3
#define AlphaCF             0.2
#define IMUPeriod           100
#define IMUFrequency        (1000.0 / 100)     

// Functions
void SetupIMU();
void SetupCompass();
void ReadBias(int Iteration);
void EstimateEulerAngles(float *Pitch, float *Roll, float *Yaw);