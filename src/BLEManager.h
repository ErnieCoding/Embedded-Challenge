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
    void updateStatus(const char* statusString); 

private:
    BLE &_ble;
    uint8_t _statusBuffer[32];
    GattCharacteristic *_statusChar;
    void onInitComplete(BLE::InitializationCompleteCallbackContext *params);
    static void scheduleBleEvents(BLE::OnEventsToProcessCallbackContext *context);
};