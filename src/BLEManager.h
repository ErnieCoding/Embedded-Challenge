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

    // ===== 原来的 0/1 接口（保留，但不再广播） =====
    void updateTremor(uint8_t score);
    void updateDyskinesia(uint8_t score);
    void updateFOG(uint8_t score);

    // ===== 新增：文字广播接口（真正用的） =====
    void updateTremorText(const char *text);
    void updateDyskinesiaText(const char *text);
    void updateFOGText(const char *text);

private:
    BLE &_ble;

    // ===== 数值状态（仍然存在，但不广播） =====
    uint8_t _tremor;
    uint8_t _dyskinesia;
    uint8_t _fog;

    // ===== 文本 buffer（BLE 单包 ≤ 20 bytes） =====
    uint8_t _tremorText[20];
    uint8_t _dyskinesiaText[20];
    uint8_t _fogText[20];

    // ===== 原来的数值 characteristic（保留） =====
    GattCharacteristic *_tremorChar;
    GattCharacteristic *_dyskinesiaChar;
    GattCharacteristic *_fogChar;

    // ===== 新的文本 characteristic（真正广播） =====
    GattCharacteristic *_tremorTextChar;
    GattCharacteristic *_dyskinesiaTextChar;
    GattCharacteristic *_fogTextChar;

    void onInitComplete(BLE::InitializationCompleteCallbackContext *params);

    static void scheduleBleEvents(
        BLE::OnEventsToProcessCallbackContext *context
    );
};
