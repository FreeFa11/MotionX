#ifndef _Haptic
#define _Haptic

#include <Arduino.h>
#include <freertos/FreeRTOS.h>


// Definitions
#define HapticOutputPinDefault    47
#define HapticChannelDefault      0
#define HapticResolutionDefault   8
#define HapticFrequencyDefault    30000

#define HapticIntensityLow        100
#define HapticIntensityMedium     150
#define HapticIntensityHigh       220

// Class
enum Pattern {Subtle, Firm, Long, Double};

class Haptic
{
private:
    uint8_t Pin, Channel;
    void Write(uint16_t Intensity, uint16_t Period);
    void Write(int16_t StartIntensity, int16_t EndIntensity, int16_t Period);

public:
    Haptic();
    ~Haptic();

    // The Haptic is controlled using PWM for fine control over the intensity of Vibration
    void Initialize(uint8_t OutputPin = HapticOutputPinDefault, uint8_t Channel = HapticChannelDefault, uint8_t Resolution = HapticResolutionDefault, uint32_t Frequency = HapticFrequencyDefault);

    // Haptic class supports vibrations corresponding to values of "Pattern" enum
    void Vibrate(Pattern VibrationPattern);
};

#endif