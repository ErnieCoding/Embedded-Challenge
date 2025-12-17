#include "BLEManager.h"

static events::EventQueue bleEventQueue;

// ===== UUID（你可以换成自己的，只要保持一致） =====
static const UUID MOTION_SERVICE_UUID ("12345678-1234-5678-1234-56789abc0000");

static const UUID TREMOR_UUID        ("12345678-1234-5678-1234-56789abc0001");
static const UUID DYSKINESIA_UUID    ("12345678-1234-5678-1234-56789abc0002");
static const UUID FOG_UUID           ("12345678-1234-5678-1234-56789abc0003");

static const UUID TREMOR_TEXT_UUID   ("12345678-1234-5678-1234-56789abc0011");
static const UUID DYSK_TEXT_UUID     ("12345678-1234-5678-1234-56789abc0012");
static const UUID FOG_TEXT_UUID      ("12345678-1234-5678-1234-56789abc0013");

BLEManager::BLEManager()
    : _ble(BLE::Instance()),
      _tremor(0),
      _dyskinesia(0),
      _fog(0),
      _tremorChar(nullptr),
      _dyskinesiaChar(nullptr),
      _fogChar(nullptr),
      _tremorTextChar(nullptr),
      _dyskinesiaTextChar(nullptr),
      _fogTextChar(nullptr)
{
    memset(_tremorText, 0, sizeof(_tremorText));
    memset(_dyskinesiaText, 0, sizeof(_dyskinesiaText));
    memset(_fogText, 0, sizeof(_fogText));
}

void BLEManager::init() {
    _ble.onEventsToProcess(scheduleBleEvents);
    _ble.init(this, &BLEManager::onInitComplete);

    static Thread bleThread;
    bleThread.start(callback(&bleEventQueue,
                             &events::EventQueue::dispatch_forever));
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

    // ===== 数值 characteristic（保留，但不会再 write） =====
    _tremorChar = new GattCharacteristic(
        TREMOR_UUID, &_tremor, sizeof(_tremor), sizeof(_tremor),
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ
    );

    _dyskinesiaChar = new GattCharacteristic(
        DYSKINESIA_UUID, &_dyskinesia, sizeof(_dyskinesia),
        sizeof(_dyskinesia),
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ
    );

    _fogChar = new GattCharacteristic(
        FOG_UUID, &_fog, sizeof(_fog), sizeof(_fog),
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ
    );

    // ===== 文本 characteristic（真正广播的） =====
    _tremorTextChar = new GattCharacteristic(
        TREMOR_TEXT_UUID,
        _tremorText, 0, sizeof(_tremorText),
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY |
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ
    );

    _dyskinesiaTextChar = new GattCharacteristic(
        DYSK_TEXT_UUID,
        _dyskinesiaText, 0, sizeof(_dyskinesiaText),
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY |
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ
    );

    _fogTextChar = new GattCharacteristic(
        FOG_TEXT_UUID,
        _fogText, 0, sizeof(_fogText),
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY |
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ
    );

    GattCharacteristic *chars[] = {
        _tremorChar,
        _dyskinesiaChar,
        _fogChar,
        _tremorTextChar,
        _dyskinesiaTextChar,
        _fogTextChar
    };

    GattService motionService(
        MOTION_SERVICE_UUID,
        chars,
        sizeof(chars) / sizeof(chars[0])
    );

    _ble.gattServer().addService(motionService);

    uint8_t advBuffer[31];
    ble::AdvertisingDataSimpleBuilder<sizeof(advBuffer)> advData;

    advData.setFlags();
    advData.setName("Parkinson-Monitor");

    _ble.gap().setAdvertisingPayload(
        ble::LEGACY_ADVERTISING_HANDLE,
        advData.getAdvertisingData()
    );

    _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);

    printf("BLE advertising started\n");
}

//
// ===== 数值接口：只更新变量，不广播 =====
//

void BLEManager::updateFOG(uint8_t score) {
    _fog = score;
}

void BLEManager::updateTremor(uint8_t score) {
    _tremor = score;
}

void BLEManager::updateDyskinesia(uint8_t score) {
    _dyskinesia = score;
}

//
// ===== 文本接口：真正广播 =====
//

static void writeText(BLE &ble,
                      GattCharacteristic *ch,
                      uint8_t *buf,
                      const char *text)
{
    if (!ch) return;

    size_t len = strlen(text);
    if (len > 20) len = 20;

    memset(buf, 0, 20);
    memcpy(buf, text, len);

    ble.gattServer().write(
        ch->getValueHandle(),
        buf,
        len
    );
}

void BLEManager::updateFOGText(const char *text) {
    writeText(_ble, _fogTextChar, _fogText, text);
}

void BLEManager::updateTremorText(const char *text) {
    writeText(_ble, _tremorTextChar, _tremorText, text);
}

void BLEManager::updateDyskinesiaText(const char *text) {
    writeText(_ble, _dyskinesiaTextChar, _dyskinesiaText, text);
}
