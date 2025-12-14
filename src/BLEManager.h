#pragma once

#include "mbed.h"
#include "ble/BLE.h"
#include "ble/gap/Gap.h"
#include "ble/gap/AdvertisingDataBuilder.h"
#include "events/EventQueue.h"

class BLEManager {
public:
    BLEManager();

    void init();
    void process();

    void updateTremor(uint8_t level);
    void updateDyskinesia(uint8_t level);
    void updateFOG(uint8_t state);

private:
    /* BLE core */
    BLE &_ble;
    events::EventQueue _event_queue;

    /* BLE values */
    uint8_t _tremor;
    uint8_t _dyskinesia;
    uint8_t _fog;

    /* BLE callbacks */
    void onInitComplete(BLE::InitializationCompleteCallbackContext *params);
    static void scheduleBleEvents(BLE::OnEventsToProcessCallbackContext *context);

    /* BLE characteristics */
    GattCharacteristic *_tremorChar;
    GattCharacteristic *_dyskinesiaChar;
    GattCharacteristic *_fogChar;
};
