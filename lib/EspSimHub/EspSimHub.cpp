#include <WiFi.h>
#include <esp_mac.h>
#include "esp_system.h"
#define ESPSIMHUB 1

// Fake an Arduino Mega
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x98
#define SIGNATURE_2 0x01

// Configure FASTLED with proper pin order
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER

// A unique identifier for the device.
//  in the future we could use the bytes to generate some
//  other format (ala UUID), but now it's just a unique 
//  string tied to the device.

String getMacAddress() {
    uint8_t baseMac[6];
    // Get MAC address for WiFi station
    esp_read_mac(baseMac, ESP_MAC_EFUSE_FACTORY);
    char baseMacChr[18] = {0};
    sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
    return (baseMacChr);
}

String getUniqueId() {

	unsigned char mac_base[6] = {0};
    esp_efuse_mac_get_default(mac_base);
    esp_read_mac(mac_base, ESP_MAC_EFUSE_FACTORY);
 return getMacAddress();
}