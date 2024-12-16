#include <Sensor.h>


// Objects
MPU6050 IMU;
HMC5883L Compass;

float AX, AY, AZ, GX, GY, GZ, MX, MY, MZ;
float GXB, GYB, GZB;
float AccPitch, AccRoll, GyroPitch, GyroRoll, GyroRollRate, GyroPitchRate;
float EulerRoll, EulerPitch, EulerYaw;
int CurrentTick, Ticks;



void SetupIMU()
{
  Wire.begin(SDAWire, SCLWire);
  IMU.initialize(ACCEL_FS::A2G, GYRO_FS::G250DPS);
}

void SetupCompass()
{
  Compass.initialize(); 
  Compass.setDataRate(HMC5883L_RATE_75);
  Compass.setSampleAveraging(HMC5883L_AVERAGING_8);
}

void ReadBias(int Iteration)
{
  for (int i = 0; i < Iteration; i++)
    {
      GXB += IMU.getRotationX();
      GYB += IMU.getRotationY();
      GZB += IMU.getRotationZ();

      vTaskDelay(40 * portTICK_RATE_MS);
    }

    GXB = GXB / Iteration;
    GYB = GYB / Iteration;
    GZB = GZB / Iteration;
}

void EstimateEulerAngles(float *Pitch, float *Roll, float *Yaw)
{

// Pitch & Roll
  // Filtered Data
  AX = AlphaLPF * IMU.getAccelerationX() + (1 - AlphaLPF) * AX;
  AY = AlphaLPF * IMU.getAccelerationY() + (1 - AlphaLPF) * AY;
  AZ = AlphaLPF * IMU.getAccelerationZ() + (1 - AlphaLPF) * AZ;
  GX = AlphaLPF * (IMU.getRotationX() - GXB) + (1 - AlphaLPF) * GX;
  GY = AlphaLPF * (IMU.getRotationY() - GYB) + (1 - AlphaLPF) * GY;
  GZ = AlphaLPF * (IMU.getRotationZ() - GZB) + (1 - AlphaLPF) * GZ;

  // **Time for Integral** //
  CurrentTick = xTaskGetTickCount();
  Ticks = CurrentTick - Ticks;

  // Acce Attitude
  AccPitch = atanf(-AX/AZ);
  AccRoll = atanf(AY/AZ);

  // Gyro Rates
  GyroRollRate  = GX +  GY * sinf(AccRoll) * tanf(AccPitch) + GZ * cosf(AccRoll) * tanf(AccPitch);     // Careful about the Angle Units
  GyroPitchRate = GY * cosf(AccRoll) - GZ * sinf(AccRoll);

  // Gyro Attitude by Feedback
  GyroPitch = EulerPitch + 0.0001f * GyroPitchRate * Ticks;
  GyroRoll = EulerRoll + 0.0001f * GyroRollRate * Ticks;

  // Complementary Filter
  EulerPitch = AlphaCF * AccPitch * 90.0 + (1 - AlphaCF) * GyroPitch;       // Scaled by 90 in this case
  EulerRoll = AlphaCF * AccRoll * 90.0 + (1 - AlphaCF) * GyroRoll;

  *Pitch = EulerPitch;
  *Roll = EulerRoll;
  *Yaw = 0.0;

  // **Time for Integral** //
  Ticks = CurrentTick;
}