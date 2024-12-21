#include <Sensor.h>


// Objects

float AX, AY, AZ, GX, GY, GZ, CX, CY, CZ;
float AccPitch, AccRoll, CompYaw, GyroPitch, GyroRoll, GyroYaw, GyroRollRate, GyroPitchRate, GyroYawRate;
float EulerRoll, EulerPitch, EulerYaw;
int CurrentTick, Ticks;



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

void Sensor::InitializeCompass(COMPASS_GAIN ComGain)
{
  Compass.initialize();
  Compass.setGain(uint8_t(ComGain));
  Compass.setSampleAveraging(HMC5883L_AVERAGING_8);

  CompassGain = ComGain;

// Check for Persistent Data of Calibration
  PersistentData.begin("Calibration", true);
  if (PersistentData.isKey("CompOffsetX"))
  {
    CompassOffsetX= PersistentData.getShort("CompOffsetX");
    CompassOffsetY= PersistentData.getShort("CompOffsetY");
    CompassOffsetZ= PersistentData.getShort("CompOffsetZ");
  }
  PersistentData.end();
}

void Sensor::CalibrateIMU()
{
  // // Gyro Bias Calibration
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

void Sensor::CalibrateCompass(int Samples)
{
  // Hard Iron Calibration
  int16_t Max[3] = {0,0,0}, Min[3] = {0,0,0}, Data[3];

  Serial.println("Rotate the Compass in all directions!");
  vTaskDelay(800 / portTICK_RATE_MS);

  for (int i=0; i<Samples; i++)
  {
    Compass.getHeading(&Data[0], &Data[1], &Data[2]);
    
    for (int j=0; j<3; ++j)
    {
      if (Max[j] < Data[j])
      {
        Max[j] = Data[j];
      }
      else if (Min[j] > Data[j])
      {
        Min[j] = Data[j];
      }
    }

    vTaskDelay(7 / portTICK_RATE_MS);
  }

  CompassOffsetX = (Max[0] + Min[0]) / 2;
  CompassOffsetY = (Max[1] + Min[1]) / 2;
  CompassOffsetZ = (Max[2] + Min[2]) / 2;


  // Soft Iron Calibration
  // Coming Soon ....

  
// Saving the Data in Flash
  PersistentData.begin("Calibration", false);

  PersistentData.putShort("CompOffsetX", CompassOffsetX);
  PersistentData.putShort("CompOffsetY", CompassOffsetY);
  PersistentData.putShort("CompOffsetZ", CompassOffsetZ);

  PersistentData.end();
}


void Sensor::EstimateEuler(float *Pitch, float *Roll, float *Yaw)
{
// Filtered Data
  AX = AlphaLowPass * IMU.getAccelerationX()    +   (1 - AlphaLowPass) * AX;
  AY = AlphaLowPass * IMU.getAccelerationY()    +   (1 - AlphaLowPass) * AY;
  AZ = AlphaLowPass * IMU.getAccelerationZ()    +   (1 - AlphaLowPass) * AZ;
  GX = AlphaLowPass * IMU.getRotationX()        +   (1 - AlphaLowPass) * GX;
  GY = AlphaLowPass * IMU.getRotationY()        +   (1 - AlphaLowPass) * GY;
  GZ = AlphaLowPass * IMU.getRotationZ()        +   (1 - AlphaLowPass) * GZ;
  // **Time for Integral** //
  CurrentTick = xTaskGetTickCount();
  Ticks = CurrentTick - Ticks;

// Yaw
  CX = AlphaLowPass * (Compass.getHeadingX()-CompassOffsetX)   +   (1 - AlphaLowPass) * CX;
  CY = AlphaLowPass * (Compass.getHeadingY()-CompassOffsetY)   +   (1 - AlphaLowPass) * CY;
  CZ = AlphaLowPass * (Compass.getHeadingZ()-CompassOffsetZ)   +   (1 - AlphaLowPass) * CZ;
  CompYaw = atanf(CY/CX) * 180.0 / PI;

// Pitch & Roll

  // Acce Attitude
  AccPitch = atanf(-AX/AZ);
  AccRoll = atanf(AY/AZ);

  // Gyro Rates
  GyroRollRate  = GX  +   GY * sinf(AccRoll) * tanf(AccPitch)     +     GZ * cosf(AccRoll) * tanf(AccPitch);     // Careful about the Angle Units
  GyroPitchRate =         GY * cosf(AccRoll)                      -     GZ * sinf(AccRoll);
  GyroYawRate   =         GY * sinf(AccRoll) / cosf(AccPitch)     +     GZ * cosf(AccRoll) / cosf(AccPitch); 

  // Gyro Attitude by Feedback
  GyroRoll  = EulerRoll     +   0.00001f * GyroRollRate * Ticks;
  GyroPitch = EulerPitch    +   0.00001f * GyroPitchRate * Ticks;
  GyroYaw   = EulerYaw      +   0.00001f * GyroYawRate * Ticks;

  // Complementary Filter
  EulerRoll   = AlphaComplementary * AccRoll  * 180 / PI    +   (1 - AlphaComplementary) * GyroRoll;              // Converting Angles from Acc to Degree
  EulerPitch  = AlphaComplementary * AccPitch * 180 / PI    +   (1 - AlphaComplementary) * GyroPitch;
  EulerYaw    = AlphaComplementary * CompYaw     +   (1 - AlphaComplementary) * GyroYaw;


  *Roll = EulerRoll;
  *Pitch = EulerPitch;
  *Yaw = EulerYaw;

  // **Time for Integral** //
  Ticks = CurrentTick;
}