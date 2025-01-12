#include <Arduino.h>
#include <freertos/FreeRTOS.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEScan.h>
#include <BLE2902.h>
#include <BLEHIDDevice.h>
#include <BLEAdvertisedDevice.h>

#include <Preferences.h>

#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)


// Definitions
const uint8_t HIDReportMap[] = {
    USAGE_PAGE(1),       0x01, // USAGE_PAGE (Generic Desktop)
    USAGE(1),            0x02, // USAGE (Mouse)
    COLLECTION(1),       0x01, // COLLECTION (Application)
    USAGE(1),            0x01, //   USAGE (Pointer)
    COLLECTION(1),       0x00, //   COLLECTION (Physical)
    // Buttons (Left, Right, Middle, Back, Forward)
    USAGE_PAGE(1),       0x09, //     USAGE_PAGE (Button)
    USAGE_MINIMUM(1),    0x01, //     USAGE_MINIMUM (Button 1)
    USAGE_MAXIMUM(1),    0x05, //     USAGE_MAXIMUM (Button 5)
    LOGICAL_MINIMUM(1),  0x00, //     LOGICAL_MINIMUM (0)
    LOGICAL_MAXIMUM(1),  0x01, //     LOGICAL_MAXIMUM (1)
    REPORT_SIZE(1),      0x01, //     REPORT_SIZE (1)
    REPORT_COUNT(1),     0x05, //     REPORT_COUNT (5)
    HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;5 button bits
    // Padding for Buttons to make it 8 bits (Byte) 
    REPORT_SIZE(1),      0x03, //     REPORT_SIZE (3)
    REPORT_COUNT(1),     0x01, //     REPORT_COUNT (1)
    HIDINPUT(1),         0x03, //     INPUT (Constant, Variable, Absolute) ;3 bit padding
    // X/Y position and Scroll Wheel
    USAGE_PAGE(1),       0x01, //     USAGE_PAGE (Generic Desktop)
    USAGE(1),            0x30, //     USAGE (X)
    USAGE(1),            0x31, //     USAGE (Y)
    USAGE(1),            0x38, //     USAGE (Wheel)
    LOGICAL_MINIMUM(1),  0x81, //     LOGICAL_MINIMUM (-127)
    LOGICAL_MAXIMUM(1),  0x7F, //     LOGICAL_MAXIMUM (127)
    REPORT_SIZE(1),      0x08, //     REPORT_SIZE (8)
    REPORT_COUNT(1),     0x03, //     REPORT_COUNT (3)
    HIDINPUT(1),         0x06, //     INPUT (Data, Variable, Relative) ;3 bytes (X,Y,Wheel)
    END_COLLECTION(0),         //   END_COLLECTION
    END_COLLECTION(0)          // END_COLLECTION
};


// Class
class BLEHID : public BLEServerCallbacks
{
    private:

    uint8_t Buttons = 0x00;
    bool DeviceConnected = false;
    BLEServer *pServer = nullptr;
    BLEAdvertising *pAdvertising = nullptr;
    BLESecurity *pSecurity = nullptr;
    BLEHIDDevice *pHID = nullptr;
    BLECharacteristic *pMouse = nullptr;

    public:
    BLEHID();

    void InitHID();
    void Move(int8_t Xaxis, int8_t Yaxis, int8_t Wheel);
    void Point(uint16_t Xaxis, uint16_t Yaxis, uint16_t Wheel = 0);
    void ClickRight();
    void ClickLeft();
    void ClickMiddle();

    protected:
    virtual void onStarted(BLEServer *pServer) { };
    virtual void onConnect(BLEServer* pServer) override;
    virtual void onDisconnect(BLEServer* pServer) override;
};


#endif