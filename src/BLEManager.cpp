#include "BLEManager.h"

static events::EventQueue bleEventQueue;

static const UUID MOTION_SERVICE_UUID("12345678-1234-5678-1234-56789abc0000");
static const UUID TREMOR_UUID        ("12345678-1234-5678-1234-56789abc0001");
static const UUID DYSKINESIA_UUID    ("12345678-1234-5678-1234-56789abc0002");
static const UUID FOG_UUID           ("12345678-1234-5678-1234-56789abc0003");

BLEManager::BLEManager()
    : _ble(BLE::Instance()),
      _tremor(0), _dyskinesia(0), _fog(0),
      _lastTremor(0), _lastDyskinesia(0), _lastFOG(0),
      _tremorChar(nullptr),
      _dyskinesiaChar(nullptr),
      _fogChar(nullptr) {}

void BLEManager::init() {
    _ble.onEventsToProcess(scheduleBleEvents);
    _ble.init(this, &BLEManager::onInitComplete);

    static Thread bleThread;
    bleThread.start(callback(&bleEventQueue, &events::EventQueue::dispatch_forever));
}

void BLEManager::scheduleBleEvents(
    BLE::OnEventsToProcessCallbackContext *context
) {
    bleEventQueue.call(
        callback(&context->ble, &BLE::processEvents)
    );
}

void BLEManager::onInitComplete(
    BLE::InitializationCompleteCallbackContext *params
) {
    if (params->error != BLE_ERROR_NONE) {
        printf("BLE init failed\n");
        return;
    }

    _tremorChar = new GattCharacteristic(
        TREMOR_UUID, &_tremor, sizeof(_tremor), sizeof(_tremor),
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY
    );

    _dyskinesiaChar = new GattCharacteristic(
        DYSKINESIA_UUID, &_dyskinesia, sizeof(_dyskinesia), sizeof(_dyskinesia),
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY
    );

    _fogChar = new GattCharacteristic(
        FOG_UUID, &_fog, sizeof(_fog), sizeof(_fog),
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY
    );

    GattCharacteristic *chars[] = {
        _tremorChar,
        _dyskinesiaChar,
        _fogChar
    };

    GattService motionService(
        MOTION_SERVICE_UUID,
        chars,
        3
    );

    _ble.gattServer().addService(motionService);

    uint8_t advBuffer[31];
    ble::AdvertisingDataSimpleBuilder<sizeof(advBuffer)> advData;

    advData.setFlags();
    advData.setName("Parkinson-Monitor");

    _ble.gap().setAdvertisingParameters(
        ble::LEGACY_ADVERTISING_HANDLE,
        ble::AdvertisingParameters(
            ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
            ble::adv_interval_t(160)
        )
    );

    _ble.gap().setAdvertisingPayload(
        ble::LEGACY_ADVERTISING_HANDLE,
        advData.getAdvertisingData()
    );

    _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);

    printf("BLE advertising started\n");
}
void BLEManager::updateFOG(uint8_t score) {
    _fog = score;   // 保存值
    if (_fogChar) {
        _ble.gattServer().write(_fogChar->getValueHandle(), &_fog, 1);
    }
}
void BLEManager::updateTremor(uint8_t score) {
    _tremor = score;
    if (_tremorChar) {
        _ble.gattServer().write(_tremorChar->getValueHandle(), &_tremor, 1);
    }
}
void BLEManager::updateDyskinesia(uint8_t score) {
    _dyskinesia = score;
    if (_dyskinesiaChar) {
        _ble.gattServer().write(_dyskinesiaChar->getValueHandle(), &_dyskinesia, 1);
    }
}
