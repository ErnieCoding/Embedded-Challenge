#pragma once
#include "mbed.h"
#include "ble/BLE.h"
#include "ble/GattCharacteristic.h"
#include "ble/GattService.h"
#include "ble/gap/AdvertisingDataSimpleBuilder.h"

class BLEManager {
public:
    BLEManager();

    void init();
    void updateStatus(const char* statusString); // Single update function

private:
    BLE &_ble;
    
    // Buffer for the status string (e.g., "TREM:DETECTED")
    uint8_t _statusBuffer[32];
    
    GattCharacteristic *_statusChar; // Single Characteristic

    void onInitComplete(BLE::InitializationCompleteCallbackContext *params);
    static void scheduleBleEvents(BLE::OnEventsToProcessCallbackContext *context);
};