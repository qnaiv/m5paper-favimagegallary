#pragma once
#include <Arduino.h>

struct Config {
    String wifi_ssid;
    String wifi_password;
    String server_base_url;
    uint32_t update_interval_sec;
};

bool loadConfig(Config& config);
