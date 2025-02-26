#include <Wireless.h>

BLEHID::BLEHID(){}

void BLEHID::Initialize()
{
    // Initiation
    BLEDevice::init("MotionX");
    BLEDevice::setPower(ESP_PWR_LVL_N0);
    BLEDevice::setMTU(247);
    BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);

    // Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(this);

    // Security
    pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

    // HID Device
    pHID = new BLEHIDDevice(pServer);
    pHID->manufacturer()->setValue("Thapathali");                           // Manufacturer
    pHID->pnp(0x02, 0xe502, 0xa111, 0x0210);
    pHID->hidInfo(0x00,0x01);
    pHID->reportMap((uint8_t*)HIDReportMap, sizeof(HIDReportMap));
    pKeyboard = pHID->inputReport(0x01);
    pMouse = pHID->inputReport(0x02);
    pHID->startServices();

    // App Service
    this->pAppService = pServer->createService(APPLICATION_SERVICE_UUID);
    this->pAppChar = pAppService->createCharacteristic(APPLICATION_CHAR_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    pAppChar->addDescriptor(new BLE2902);
    pAppChar->setCallbacks(this);
    pAppService->start();
    pHID->setBatteryLevel(100);

    // Advertising
    pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->setAppearance(HID_KEYBOARD);
    pAdvertising->addServiceUUID(pHID->hidService()->getUUID());
    pAdvertising->addServiceUUID(APPLICATION_SERVICE_UUID);
    pAdvertising->start();

    vTaskDelay(1000 / portTICK_RATE_MS);
    Serial.println("Service Started!!!!");
    USBSerial.println("Service Started!!!!");
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

        this->pMouse->setValue(mouseData, sizeof(mouseData));
        this->pMouse->notify();

        if (Buttons) {Buttons = 0x00;}
    }
}


void BLEHID::Point(uint16_t Xaxis, uint16_t Yaxis, uint16_t Wheel)
{
  
}

void BLEHID::ClickLeft()
{
    Buttons = B00000001; 
}

void BLEHID::ClickRight()
{
    Buttons = B00000010; 
}

void BLEHID::ClickMiddle()
{
    Buttons = B00000100; 
}

void BLEHID::Press(std::string Key)
{
    if (DeviceConnected)
    {
        Keys[2] = asciihid[uint8_t(Key.c_str()[0]) - ASCII_HID_MAP_KEY_OFFSET];
    }
}

void BLEHID::Press(CONTROLKEY Key)
{
    if (DeviceConnected)
    {
        Keys[7] = Key;
    }
}

void BLEHID::Press(MODIFIERKEY Key)
{
    if (DeviceConnected)
    {
        Keys[0] = Keys[0] | Key;
    }
}

void BLEHID::SendKeys()
{
    if (DeviceConnected)
    {
        this->pKeyboard->setValue(Keys, sizeof(Keys));
        this->pKeyboard->notify();
        
        memset(Keys, 0, sizeof(Keys));
        this->pKeyboard->setValue(Keys, sizeof(Keys));
        this->pKeyboard->notify();
    }
}

void BLEHID::WriteToApp(std::string Data)
{
    if (DeviceConnected)
    {
        pAppChar->setValue(Data);
        pAppChar->notify();
    }
}

void BLEHID::HandleAppData()
{
    if (!AppData.empty())
    {
        JsonDocument DataJSON;
        USBSerial.println(AppData.c_str());
        
        if (AppData.length() > 0) {
    
            // Error Handling in case of improper data
            DeserializationError error = deserializeJson(DataJSON, AppData);
            
            if (error)
            {
                USBSerial.println("Data not JSON Error:\t" + String(AppData.c_str()));
            }
            else
            {
                if (DataJSON["Type"].as<String>() == "N")
                {
                    Preference.begin("AppData", false);
        
                    Preference.putUInt("Sensitivity", DataJSON["SN"].as<uint32_t>());
                    Preference.putBool("HapticOn", DataJSON["HO"].as<bool>());
                    Preference.putUInt("HapticMode", DataJSON["HM"].as<uint32_t>());
    
                    Preference.end();
                }
                else if(DataJSON["Type"].as<String>() == "G")
                {
                    Preference.begin("AppData", false);
        
                    Preference.putUInt("GestureOne", DataJSON["G1"].as<uint32_t>());
                    Preference.putUInt("GestureTwo", DataJSON["G2"].as<uint32_t>());
                    Preference.putUInt("GestureThree", DataJSON["G3"].as<uint32_t>());
                    Preference.putUInt("GestureFour", DataJSON["G4"].as<uint32_t>());
                    Preference.putUInt("GestureFive", DataJSON["G5"].as<uint32_t>());
    
                    Preference.end();
                }
                
                ESP.restart();
            }
        }

        AppData.clear();
    }
}

void BLEHID::onConnect(BLEServer* pServer) {
    DeviceConnected = true;

    // Set notifications
    BLE2902* descriptor = (BLE2902*)this->pKeyboard->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    descriptor->setNotifications(true);
    descriptor = (BLE2902*)this->pMouse->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    descriptor->setNotifications(true);
    Serial.println("Client connected!!!!");
    USBSerial.println("Client connected!!!!");
}

void BLEHID::onDisconnect(BLEServer* pServer) {
    DeviceConnected = false;

    // Reset notifications
    BLE2902* descriptor = (BLE2902*)this->pKeyboard->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    descriptor->setNotifications(false);
    descriptor = (BLE2902*)this->pMouse->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    descriptor->setNotifications(false);
    Serial.println("Client disconnected!!!!");
    USBSerial.println("Client disconnected!!!!");

    pAdvertising->start();
}


void BLEHID::onWrite(BLECharacteristic *pCharacteristic) {

    AppData = pCharacteristic->getValue();
}