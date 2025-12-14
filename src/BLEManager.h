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
    void process();

    void updateTremor(uint8_t score);
    void updateDyskinesia(uint8_t score);
    void updateFOG(uint8_t score);

private:
    BLE &_ble;

    uint8_t _tremor;
    uint8_t _dyskinesia;
    uint8_t _fog;

    uint8_t _lastTremor;
    uint8_t _lastDyskinesia;
    uint8_t _lastFOG;

    GattCharacteristic *_tremorChar;
    GattCharacteristic *_dyskinesiaChar;
    GattCharacteristic *_fogChar;

    void onInitComplete(BLE::InitializationCompleteCallbackContext *params);

    static void scheduleBleEvents(
        BLE::OnEventsToProcessCallbackContext *context
    );
};
