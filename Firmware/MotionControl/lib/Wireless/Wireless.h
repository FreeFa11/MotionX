#ifndef _Wireless_
#define _Wireless_

#include <Arduino.h>
#include <freertos/FreeRTOS.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <BLEUtils.h>
#include <BLEHIDDevice.h>
#include <BLECharacteristic.h>
#include <BLEAdvertisedDevice.h>

#include <Preferences.h>
#include <ArduinoJson.h>



#include <HID.h>


// Definitions
#define APPLICATION_SERVICE_UUID            "785e99cc-e61f-41b0-899e-f59fa295441a"
#define APPLICATION_CHAR_UUID               "7fb99d10-d067-42f2-99f4-b515e595c91c"


// Class
class BLEHID : public BLEServerCallbacks, public BLECharacteristicCallbacks
// ServerCallbacks for onConnect & onDisconnect, CharacteristicCallbacks for onWrite
{
    private:

    uint8_t Buttons = 0x00;
    uint8_t Keys[8];
    std::string AppData;

    bool DeviceConnected = false;
    BLEServer *pServer = nullptr;
    BLEAdvertising *pAdvertising = nullptr;
    BLESecurity *pSecurity = nullptr;
    BLEHIDDevice *pHID = nullptr;
    BLECharacteristic *pMouse = nullptr;
    BLECharacteristic *pKeyboard = nullptr;
    BLEService *pAppService = nullptr;
    BLECharacteristic *pAppChar = nullptr;
    Preferences Preference;

    public:
    BLEHID();

    void Initialize();
    void Move(int8_t Xaxis, int8_t Yaxis, int8_t Wheel);
    void Point(uint16_t Xaxis, uint16_t Yaxis, uint16_t Wheel = 0);
    void ClickRight();
    void ClickLeft();
    void ClickMiddle();
    void Press(std::string Key);
    void Press(CONTROLKEY Key);
    void Press(MODIFIERKEY Key);
    void SendKeys();
    void WriteToApp(std::string Data);
    void HandleAppData();

    protected:
    virtual void onConnect(BLEServer* pServer) override;
    virtual void onDisconnect(BLEServer* pServer) override;
    virtual void onWrite(BLECharacteristic *pCharacteristic);
};

#endif