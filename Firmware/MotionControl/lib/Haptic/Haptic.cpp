#include <Haptic.h>


Haptic::Haptic()
{}
Haptic::~Haptic()
{}

void Haptic::Initialize(uint8_t OutputPin, uint8_t Channel, uint8_t Resolution, uint32_t Frequency)
{
    this->Pin = OutputPin;
    this->Channel = Channel;
    
    ledcSetup(Channel, Frequency, Resolution);
    ledcAttachPin(OutputPin, Channel);

}

void Haptic::Write(uint16_t Intensity, uint16_t Period)
{
    ledcWrite(Channel, Intensity);
    vTaskDelay(Period / portTICK_RATE_MS);
    ledcWrite(Channel, 0);
}

void Haptic::Write(int16_t StartIntensity, int16_t EndIntensity, int16_t Period)
{
    int16_t Periods = Period / 5;
    int16_t Increment = (EndIntensity - StartIntensity) / Periods;

    for (int16_t i=0; i<Periods; ++i)
    {
        ledcWrite(Channel, (StartIntensity + (i*Increment)));
        vTaskDelay(5 / portTICK_RATE_MS);
    }

    ledcWrite(Channel, EndIntensity);

}


void Haptic::Vibrate(Pattern VibrationPattern)
{
    switch (VibrationPattern)
    {
        case Pattern::Subtle:
            this->Write(HapticIntensityHigh, 20);
            break;

        case Pattern::Firm:
            this->Write(0, HapticIntensityHigh, 10);
            this->Write(HapticIntensityHigh, 50);
            this->Write(HapticIntensityHigh, 0, 10);
            break;

        case Pattern::Long:
            this->Write(0, HapticIntensityMedium, 50);
            this->Write(HapticIntensityMedium, 200);
            this->Write(HapticIntensityMedium, 0, 80);
            break;
            
        case Pattern::Double:
            this->Write(0, HapticIntensityMedium, 15);
            this->Write(HapticIntensityHigh, 25);
            this->Write(0, 65);
            this->Write(HapticIntensityMedium, 30);
            break;
        
        default:
            break;
    }
}
