#include <Haptic.h>


void SetupHaptic()
{
    ledcSetup(PWMChannel, PWMFrequency, PWMResolution);
    ledcAttachPin(OutputPin, PWMChannel);
}
void WriteHaptic(uint16_t Intensity, uint16_t Period)
{
    ledcWrite(PWMChannel, Intensity);
    vTaskDelay(Period / portTICK_RATE_MS);
    ledcWrite(0, PWMChannel);
}