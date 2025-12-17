#include "BLEManager.h"
#include <cstdio>

static events::EventQueue bleEventQueue;
static const UUID MOTION_SERVICE_UUID("12345678-1234-5678-1234-56789abc0000");
static const UUID STATUS_CHAR_UUID   ("12345678-1234-5678-1234-56789abc0001");

BLEManager::BLEManager() : _ble(BLE::Instance()), _statusChar(nullptr) {
    memset(_statusBuffer, 0, sizeof(_statusBuffer));
}

void BLEManager::init() {
    _ble.onEventsToProcess(scheduleBleEvents);
    _ble.init(this, &BLEManager::onInitComplete);
    static Thread bleThread;
    bleThread.start(callback(&bleEventQueue,
                             &events::EventQueue::dispatch_forever));
}

void BLEManager::scheduleBleEvents(BLE::OnEventsToProcessCallbackContext *context) {
    bleEventQueue.call(callback(&context->ble, &BLE::processEvents));
}

void BLEManager::onInitComplete(BLE::InitializationCompleteCallbackContext *params) {
    if (params->error != BLE_ERROR_NONE) return;

    _statusChar = new GattCharacteristic(
        STATUS_CHAR_UUID, _statusBuffer, sizeof(_statusBuffer), sizeof(_statusBuffer),
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY
    );
    GattCharacteristic *chars[] = { _statusChar };
    GattService motionService(MOTION_SERVICE_UUID, chars, 1);
    _ble.gattServer().addService(motionService);

    uint8_t advBuffer[31];
    ble::AdvertisingDataSimpleBuilder<sizeof(advBuffer)> advData;
    advData.setFlags();
    advData.setName("Parkinson-Monitor");

    _ble.gap().setAdvertisingParameters(ble::LEGACY_ADVERTISING_HANDLE, 
        ble::AdvertisingParameters(ble::advertising_type_t::CONNECTABLE_UNDIRECTED, ble::adv_interval_t(160)));
    _ble.gap().setAdvertisingPayload(ble::LEGACY_ADVERTISING_HANDLE, advData.getAdvertisingData());
    _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
    printf("BLE Started (Single Service)\n");
}

void BLEManager::updateStatus(const char* statusString) {
    if (!_statusChar) return;
    strncpy((char*)_statusBuffer, statusString, sizeof(_statusBuffer) - 1);
    _ble.gattServer().write(_statusChar->getValueHandle(), _statusBuffer, strlen((char*)_statusBuffer));
}
