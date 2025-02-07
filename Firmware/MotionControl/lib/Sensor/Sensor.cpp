#include <Sensor.h>



Sensor::Sensor(int SDA , int SCL)
{
  Wire.begin(SDA, SCL);
}
Sensor::~Sensor(){}


void Sensor::InitializeIMU(ACCEL_FS AccRange, GYRO_FS GyroRange)
{
  IMU.initialize(ACCEL_FS::A2G, GYRO_FS::G250DPS);

  // Filter of Cutoff 98Hz
  IMU.setDLPFMode(MPU6050_DLPF_BW_98);

// Check for Persistent Data of Calibration
  PersistentData.begin("Calibration", true);
  if (PersistentData.isKey("GyroOffsetX"))
  {
    IMU.setXGyroOffset(PersistentData.getShort("GyroOffsetX"));
    IMU.setYGyroOffset(PersistentData.getShort("GyroOffsetY"));
    IMU.setZGyroOffset(PersistentData.getShort("GyroOffsetZ"));
  }

  PersistentData.end();
}

void Sensor::InitializeHall(adc_attenuation_t Attenuation)
{
  analogSetAttenuation(Attenuation);
  
  pinMode(RingFingerPin, INPUT);
  pinMode(IndexFingerPin, INPUT);
  pinMode(MiddleFingerPin, INPUT);
}

void Sensor::CalibrateIMU()
{
// // Custom Average Based Gyro Bias Calibration
  // IMU.setFullScaleAccelRange(uint8_t(ACCEL_FS::A16G));
  // IMU.setFullScaleGyroRange(uint8_t(GYRO_FS::G2000DPS));

  // float GXB=0, GYB=0, GZB=0;
  // for (int i = 0; i < 1500; i++)
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

// Using the Calibration from Library
  IMU.CalibrateGyro();

// Saving the Data in Flash
  PersistentData.begin("Calibration", false);

  PersistentData.putShort("GyroOffsetX", IMU.getXGyroOffset());
  PersistentData.putShort("GyroOffsetY", IMU.getYGyroOffset());
  PersistentData.putShort("GyroOffsetZ", IMU.getZGyroOffset());
  
  PersistentData.end();
}

void Sensor::UpdateData()
{
// Accelero
  // Bias
  float TX, TY, TZ;
  TX = IMU.getAccelerationX() - 186;
  TY = IMU.getAccelerationY() - 22;
  TZ = IMU.getAccelerationZ() + 260;
  // Scaling & Cross-Axis
  AX = TX * 0.609860911       + TY * 0.000203560209     - TZ * 0.000691596337;
  AY = TX * 0.0000905356887   + TY * 0.611274234        + TZ * 0.000183010164;
  AZ = TX * -0.00448895538    + TY * 0.00352896401      + TZ * 0.611871348;
// Gyro
  GX = IMU.getRotationX();
  GY = IMU.getRotationY();
  GZ = IMU.getRotationZ();

// Compass Data
  // CX = AlphaLowPass * (Compass.getHeadingX()-CompassOffsetX)   +   (1 - AlphaLowPass) * CX;
  // CY = AlphaLowPass * (Compass.getHeadingY()-CompassOffsetY)   +   (1 - AlphaLowPass) * CY;
  // CZ = AlphaLowPass * (Compass.getHeadingZ()-CompassOffsetZ)   +   (1 - AlphaLowPass) * CZ;
  // CompYaw = atanf(CY/CX) * 180.0 / PI;

// **Time for Integral** //
  Ticks = xTaskGetTickCount() - TicksSoFar;
  TicksSoFar += Ticks;
}

void Sensor::GetRawData(float *AX, float *AY, float *AZ, float *GX, float *GY, float *GZ)
{
  *AX = this->AX;
  *AY = this->AY;
  *AZ = this->AZ;
  *GX = this->GX;
  *GY = this->GY;
  *GZ = this->GZ;
}

void Sensor::GetRawData(float *GX, float *GY, float *GZ)
{
  *GX = this->GX;
  *GY = this->GY;
  *GZ = this->GZ;
}

void Sensor::CalculateOrientation()
{
  // Objects
  static float RollGyro, PitchGyro, YawGyro, RollAcc, PitchAcc;                           // Static for consistent and fast data
  static float RollRateGyro, PitchRateGyro, YawRateGyro;
  static float RollRad, PitchRad, YawRad;
  // Updates
  RollRad = this->Roll * PI / 180;
  PitchRad = this->Pitch * PI / 180;
  YawRad = this->Yaw * PI / 180;
  cosPitch = cosf(PitchRad);
  sinPitch = sinf(PitchRad);
  cosRoll = cosf(RollRad);
  sinRoll = sinf(RollRad);
  cosYaw = cosf(YawRad);
  sinYaw = sinf(YawRad);
  tanPitch = tanf(PitchRad);

// Pitch | Roll | Yaw
  // Acc Attitude
  PitchAcc =  atanf(-AX/AZ) * 180 / PI;                                                   // Converting Angles from Acc to Degree
  RollAcc =   atanf(AY/AZ)  * 180 / PI;

  // Gyro Rates
  RollRateGyro  =   GX  +   GY * sinRoll * tanPitch     +     GZ * cosRoll * tanPitch;
  PitchRateGyro =           GY * cosRoll                -     GZ * sinRoll;
  YawRateGyro   =           GY * sinRoll / cosPitch     +     GZ * cosRoll / cosPitch; 

  // Gyro Attitude by Feedback
  RollGyro  =   this->Roll     +   0.000007f * RollRateGyro   *   Ticks;                  // Integration Scaling Costant
  PitchGyro =   this->Pitch    +   0.000007f * PitchRateGyro  *   Ticks;
  YawGyro   =   this->Yaw       +   0.000007f * YawRateGyro    *   Ticks;

  // Complementary Filter
  this->Roll   =  AlphaComplementary * RollAcc    +   (1 - AlphaComplementary) * RollGyro;
  this->Pitch  =  AlphaComplementary * PitchAcc   +   (1 - AlphaComplementary) * PitchGyro;
  this->Yaw    =  YawGyro;

}


void Sensor::CalculateOrientation(float *Pitch, float *Roll, float *Yaw)
{
  CalculateOrientation();
  
  *Roll =   this->Roll;
  *Pitch =  this->Pitch;
  *Yaw =    this->Yaw;
}

void Sensor::CalculateVelocity(float *X, float *Y, float *Z)
{
  static float VelocityX, VelocityY, VelocityZ;

// // Rotation matrix components
//     float R11 = cosYaw * cosPitch;
//     float R12 = cosYaw * sinPitch * sinRoll - sinYaw * cosRoll;
//     float R13 = cosYaw * sinPitch * cosRoll + sinYaw * sinRoll;
//     float R21 = sinYaw * cosPitch;
//     float R22 = sinYaw * sinPitch * sinRoll + cosYaw * cosRoll;
//     float R23 = sinYaw * sinPitch * cosRoll - cosYaw * sinRoll;
//     float R31 = -sinPitch;
//     float R32 = cosPitch * sinRoll;
//     float R33 = cosPitch * cosRoll;
// // Transform to world frame
//     VelocityX = R11 * AX + R12 * AY + R13 * AZ;
//     VelocityY = R21 * AX + R22 * AY + R23 * AZ;
//     VelocityZ = R31 * AX + R32 * AY + R33 * AZ;
// Same Thing
  VelocityX += 0.0005f * Ticks * (cosYaw * cosPitch * AX    +    (cosYaw * sinPitch * sinRoll - sinYaw * cosRoll) * AY    +   (cosYaw * sinPitch * cosRoll + sinYaw * sinRoll) * AZ);
  VelocityY += 0.0005f * Ticks * (sinYaw * cosPitch * AX    +    (sinYaw * sinPitch * sinRoll + cosYaw * cosRoll) * AY    +   (sinYaw * sinPitch * cosRoll - cosYaw * sinRoll) * AZ);
  VelocityZ += 0.0005f * Ticks * (-sinPitch * AX            +     cosPitch * sinRoll * AY                                 +   cosPitch * cosRoll * AZ)       -     0.0005f * 16540.0f;

  // int Magnitude = sqrt(pow(AX, 2) + pow(AY, 2) + pow(AZ, 2));
  // USBSerial.print(">AX:");
  // USBSerial.println(AX + Magnitude * sinPitch * cosRoll);
  // USBSerial.print(">AY:");
  // USBSerial.println(AY - Magnitude * cosPitch * sinRoll);
  // USBSerial.print(">AZ:");
  // USBSerial.println(AZ - Magnitude * cosPitch * cosRoll);

  *X = VelocityX;
  *Y = VelocityY;
  *Z = VelocityZ;
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