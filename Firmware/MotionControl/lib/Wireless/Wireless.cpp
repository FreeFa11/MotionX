#include <Wireless.h>

BLEHID::BLEHID(){}

void BLEHID::InitHID()
{
    // Server
    BLEDevice::init("MotionX");
    BLEDevice::setPower(ESP_PWR_LVL_N24);

    // Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(this);

    // Device
    pHID = new BLEHIDDevice(pServer);
    pMouse = pHID->inputReport(0);
    pHID->manufacturer()->setValue("KTAHaru");
    pHID->pnp(0x02, 0xE502, 0xA111, 0x0210);      // Vendor ID, Product ID, Version
    pHID->hidInfo(0x00, 0x02);  

    // Security
    pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

    // ReportMap
    pHID->reportMap((uint8_t*)HIDReportMap, sizeof(HIDReportMap));

    // Service
    pHID->startServices();
    this->onStarted(pServer);

    // Advertising
    pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->setAppearance(GENERIC_HID);
    pAdvertising->addServiceUUID(pHID->hidService()->getUUID());
    pAdvertising->start();

    vTaskDelay(3000 / portTICK_RATE_MS);
    Serial.println("BLE Service Started!!!!");
}

void BLEHID::Move(int8_t Xaxis, int8_t Yaxis, int8_t Wheel)
{
    if (DeviceConnected)
    {
        uint8_t mouseData[4];
        mouseData[0] = Buttons;
        mouseData[1] = uint8_t(Xaxis);
        mouseData[2] = uint8_t(Yaxis);
        mouseData[3] = uint8_t(Wheel);

        this->pMouse->setValue(mouseData, 5);
        this->pMouse->notify();

        if (Buttons) {Buttons = 0x00;}
    }
}


void BLEHID::Point(uint16_t Xaxis, uint16_t Yaxis, uint16_t Wheel)
{
  
}

void BLEHID::ClickRight()
{
    Buttons = B00000001; 
}

void BLEHID::ClickLeft()
{
    Buttons = B00000010; 
}

void BLEHID::ClickMiddle()
{
    Buttons = B00000100; 
}

void BLEHID::onConnect(BLEServer* pServer) {
    DeviceConnected = true;
    // Set notifications
    BLE2902* desc = (BLE2902*)this->pMouse->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    desc->setNotifications(true);
    Serial.println("Client connected");
}

void BLEHID::onDisconnect(BLEServer* pServer) {
    DeviceConnected = false;
    // Reset notifications
    BLE2902* desc = (BLE2902*)this->pMouse->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    desc->setNotifications(false);
    Serial.println("Client disconnected");

    pAdvertising->start();
}