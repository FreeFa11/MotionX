#ifndef _Sensor_
#define _Sensor_

#include <Arduino.h>
#include <freertos/FreeRTOS.h>

#include <math.h>
#include <esp_dsp.h>

#include <Wire.h>
#include <Preferences.h>
#include <MPU6050.h>
#include <HMC5883L.h>


// Definitions
#define SDADefault              15
#define SCLDefault              18

#define IndexFingerPin          11
#define MiddleFingerPin         17
#define RingFingerPin           16

#define AlphaLowPassAcc         0.3
#define AlphaLowPassGyro        0.3
#define AlphaComplementary      0.05
#define IMUPeriod               100
#define IMUFrequency            (1000.0 / IMUPeriod) 

// CxyG represents Compass gain of x.y Gauss
enum COMPASS_GAIN {C09G, C13G, C19G, C25G, C40G, C47G, C56G, C81G};


// Class
class Sensor
{
    private:
    // IMU
        float AX, AY, AZ, GX, GY, GZ;       // IMU Data
        float sinRoll, cosRoll, sinPitch, cosPitch, sinYaw, cosYaw, tanPitch;
        float Roll, Pitch, Yaw;             // Orientation 
        uint32_t TicksSoFar, Ticks;         // Time for Calculus

        MPU6050 IMU;


    // Compass
        // float CX, CY, CZ;                   // Filterered Compass Data

        // HMC5883L Compass;
        // COMPASS_GAIN CompassGain;
        // int CompassOffsetX, CompassOffsetY, CompassOffsetZ;


    // Flash
        Preferences PersistentData;


    public:
        Sensor(int SDA = SDADefault, int SCL = SCLDefault);
        ~Sensor();

        void InitializeIMU(ACCEL_FS AccRange = ACCEL_FS::A2G, GYRO_FS GyroRange = GYRO_FS::G250DPS);
        void InitializeHall(adc_attenuation_t Attenuation = ADC_2_5db);
        void CalibrateIMU();
        void UpdateData();
        void GetRawData(float *GX, float *GY, float *GZ);
        void GetRawData(float *AX, float *AY, float *AZ, float *GX, float *GY, float *GZ);
        void CalculateOrientation();
        void CalculateOrientation(float *Pitch, float *Roll, float *Yaw);
        void CalculateVelocity(float *X, float *Y, float *Z);
        // void InitializeCompass(COMPASS_GAIN CompassGain = COMPASS_GAIN::C09G);
        // void CalibrateCompass(int Samples = 1500);
};

#endif