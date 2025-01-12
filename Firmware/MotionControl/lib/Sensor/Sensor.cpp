#include <Sensor.h>



Sensor::Sensor(int SDA , int SCL)
{
  Wire.begin(SDA, SCL);
}
Sensor::~Sensor(){}


void Sensor::InitializeIMU(ACCEL_FS AccRange, GYRO_FS GyroRange)
{
  IMU.initialize(ACCEL_FS::A2G, GYRO_FS::G250DPS);

  AccelerationRange = AccRange;
  GyroscopeRange = GyroRange;

// Check for Persistent Data of Calibration
  PersistentData.begin("Calibration", true);
  if (PersistentData.isKey("GyroOffsetX"))
  {
    IMU.setXGyroOffset(PersistentData.getShort("GyroOffsetX"));
    IMU.setYGyroOffset(PersistentData.getShort("GyroOffsetY"));
    IMU.setZGyroOffset(PersistentData.getShort("GyroOffsetZ"));
  }
  if (PersistentData.isKey("AccelOffsetX"))
  {
    IMU.setXAccelOffset(PersistentData.getShort("AccelOffsetX"));
    IMU.setYAccelOffset(PersistentData.getShort("AccelOffsetY"));
    IMU.setZAccelOffset(PersistentData.getShort("AccelOffsetZ"));
  }
  PersistentData.end();
}

void Sensor::CalibrateIMU()
{
// // Custom Average Based Gyro Bias Calibration
  // IMU.setFullScaleAccelRange(uint8_t(ACCEL_FS::A16G));
  // IMU.setFullScaleGyroRange(uint8_t(GYRO_FS::G2000DPS));

  // float GXB=0, GYB=0, GZB=0;
  // for (int i = 0; i < 700; i++)
  //   {
  //     GXB += IMU.getRotationX();
  //     GYB += IMU.getRotationY();
  //     GZB += IMU.getRotationZ();

  //     vTaskDelay(5 * portTICK_RATE_MS);
  //   }
  // GXB = - (GXB / 700);
  // GYB = - (GYB / 700);
  // GZB = - (GZB / 700);
  
  // IMU.setXGyroOffset(int16_t(GXB / 4.0));
  // IMU.setYGyroOffset(int16_t(GYB) / 4.0);
  // IMU.setZGyroOffset(int16_t(GZB) / 4.0);

  // IMU.setFullScaleAccelRange(uint8_t(this->AccelerationRange));
  // IMU.setFullScaleGyroRange(uint8_t(this->GyroscopeRange));

// Using the Calibration from JrowBergs' Library
  IMU.CalibrateAccel();
  IMU.CalibrateGyro();

// Saving the Data in Flash
  PersistentData.begin("Calibration", false);

  PersistentData.putShort("GyroOffsetX", IMU.getXGyroOffset());
  PersistentData.putShort("GyroOffsetY", IMU.getYGyroOffset());
  PersistentData.putShort("GyroOffsetZ", IMU.getZGyroOffset());
  PersistentData.putShort("AccelOffsetX", IMU.getXAccelOffset());
  PersistentData.putShort("AccelOffsetY", IMU.getYAccelOffset());
  PersistentData.putShort("AccelOffsetZ", IMU.getZAccelOffset());
  
  PersistentData.end();
}

void Sensor::UpdateData()
{
// IMU Data
  AX = AlphaLowPass * IMU.getAccelerationX()    +   (1 - AlphaLowPass) * AX;
  AY = AlphaLowPass * IMU.getAccelerationY()    +   (1 - AlphaLowPass) * AY;
  AZ = AlphaLowPass * IMU.getAccelerationZ()    +   (1 - AlphaLowPass) * AZ;
  GX = AlphaLowPass * IMU.getRotationX()        +   (1 - AlphaLowPass) * GX;
  GY = AlphaLowPass * IMU.getRotationY()        +   (1 - AlphaLowPass) * GY;
  GZ = AlphaLowPass * IMU.getRotationZ()        +   (1 - AlphaLowPass) * GZ;

// Compass Data
  // CX = AlphaLowPass * (Compass.getHeadingX()-CompassOffsetX)   +   (1 - AlphaLowPass) * CX;
  // CY = AlphaLowPass * (Compass.getHeadingY()-CompassOffsetY)   +   (1 - AlphaLowPass) * CY;
  // CZ = AlphaLowPass * (Compass.getHeadingZ()-CompassOffsetZ)   +   (1 - AlphaLowPass) * CZ;
  // CompYaw = atanf(CY/CX) * 180.0 / PI;

// **Time for Integral** //
  Ticks = xTaskGetTickCount() - TicksSoFar;
  TicksSoFar += Ticks;
}

void Sensor::CalculateOrientation(float *Pitch, float *Roll, float *Yaw)
{
// Objects
  static float RollGyro, PitchGyro, YawGyro, RollAcc, PitchAcc, RollEuler, PitchEuler;                            // Static for consistent and fast data
  static float RollRateGyro, PitchRateGyro, YawRateGyro;


// Pitch | Roll | Yaw

  // Acc Attitude
  PitchAcc =  atanf(-AX/AZ);
  RollAcc =   atanf(AY/AZ);

  // Gyro Rates
  RollRateGyro  =   GX  +   GY * sinf(RollAcc) * tanf(PitchAcc)     +     GZ * cosf(RollAcc) * tanf(PitchAcc);    // Careful about the Angle Units
  PitchRateGyro =           GY * cosf(RollAcc)                      -     GZ * sinf(RollAcc);
  YawRateGyro   =           GY * sinf(RollAcc) / cosf(PitchAcc)     +     GZ * cosf(RollAcc) / cosf(PitchAcc); 

  // Gyro Attitude by Feedback
  RollGyro  = RollEuler     +   0.00001f * RollRateGyro   *   Ticks;                                              // 0.00001f is Integration Scaling Costant
  PitchGyro = PitchEuler    +   0.00001f * PitchRateGyro  *   Ticks;
  YawGyro   = YawGyro       +   0.00001f * YawRateGyro    *   Ticks;

  // Complementary Filter
  RollEuler   = AlphaComplementary * RollAcc  * 180 / PI    +   (1 - AlphaComplementary) * RollGyro;              // Converting Angles from Acc to Degree
  PitchEuler  = AlphaComplementary * PitchAcc * 180 / PI    +   (1 - AlphaComplementary) * PitchGyro;
  // EulerYaw    = AlphaComplementary * CompYaw     +   (1 - AlphaComplementary) * YawGyro;

  *Roll =   RollEuler;
  *Pitch =  PitchEuler;
  *Yaw =    YawGyro;
}

void Sensor::CalculateVelocity(float *X, float *Y, float *Z)
{
  static float VelocityX, VelocityY, VelocityZ;
  static float Roll, Pitch, Yaw;

  AX -= 16400 * sin(Pitch * PI / 180.) * cos(Roll * PI / 180.);
  AY -= 16400 * sin(Roll * PI / 180.) * cos(Pitch * PI / 180.);

  VelocityX = VelocityX + Ticks * AX * 0.0001f;
  VelocityY = VelocityY + Ticks * AY * 0.0001f;
  // VelocityZ = VelocityZ + Ticks * AZ * 0.0001f;

  *X = VelocityX;
  *Y = VelocityY;
  *Z = AZ;
}





// Compass
// void Sensor::CalibrateCompass(int Samples)
// {
// // Hard Iron Calibration
//   int16_t Max[3] = {0,0,0}, Min[3] = {0,0,0}, Data[3];

//   Serial.println("Rotate the Compass in all directions!");
//   vTaskDelay(800 / portTICK_RATE_MS);

//   for (int i=0; i<Samples; i++)
//   {
//     Compass.getHeading(&Data[0], &Data[1], &Data[2]);
    
//     for (int j=0; j<3; ++j)
//     {
//       if (Max[j] < Data[j])
//       {
//         Max[j] = Data[j];
//       }
//       else if (Min[j] > Data[j])
//       {
//         Min[j] = Data[j];
//       }
//     }

//     vTaskDelay(7 / portTICK_RATE_MS);
//   }

//   CompassOffsetX = (Max[0] + Min[0]) / 2;
//   CompassOffsetY = (Max[1] + Min[1]) / 2;
//   CompassOffsetZ = (Max[2] + Min[2]) / 2;

// // Soft Iron Calibration
//   // Coming Soon ....

// // Saving the Data in Flash
//   PersistentData.begin("Calibration", false);

//   PersistentData.putShort("CompOffsetX", CompassOffsetX);
//   PersistentData.putShort("CompOffsetY", CompassOffsetY);
//   PersistentData.putShort("CompOffsetZ", CompassOffsetZ);

//   PersistentData.end();
// }

// void Sensor::InitializeCompass(COMPASS_GAIN ComGain)
// {
//   Compass.initialize();
//   Compass.setGain(uint8_t(ComGain));
//   Compass.setSampleAveraging(HMC5883L_AVERAGING_8);

//   CompassGain = ComGain;

// // Check for Persistent Data of Calibration
//   PersistentData.begin("Calibration", true);
//   if (PersistentData.isKey("CompOffsetX"))
//   {
//     CompassOffsetX= PersistentData.getShort("CompOffsetX");
//     CompassOffsetY= PersistentData.getShort("CompOffsetY");
//     CompassOffsetZ= PersistentData.getShort("CompOffsetZ");
//   }
//   PersistentData.end();
// }