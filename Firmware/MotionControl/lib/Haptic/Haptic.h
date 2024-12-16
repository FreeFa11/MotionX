#include <Arduino.h>
#include <freertos/FreeRTOS.h>


// Definitions
#define OutputPin           4

#define PWMChannel          0
#define PWMFrequency        30000
#define PWMResolution       8

// Functions
void SetupHaptic();
void WriteHaptic(uint16_t Intensity, uint16_t Period);