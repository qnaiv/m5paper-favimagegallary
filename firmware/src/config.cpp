#include "config.h"
#include <ArduinoJson.h>
#include <SD.h>

bool loadConfig(Config& config) {
    File file = SD.open("/config.json");
    if (!file) {
        Serial.println("[Config] Failed to open /config.json");
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.printf("[Config] JSON parse error: %s\n", error.c_str());
        return false;
    }

    config.wifi_ssid          = doc["wifi_ssid"]          | "";
    config.wifi_password      = doc["wifi_password"]      | "";
    config.server_base_url    = doc["server_base_url"]    | "";
    config.update_interval_sec = doc["update_interval_sec"] | 3600U;

    Serial.printf("[Config] ssid=%s url=%s interval=%u\n",
        config.wifi_ssid.c_str(),
        config.server_base_url.c_str(),
        config.update_interval_sec);

    return true;
}
