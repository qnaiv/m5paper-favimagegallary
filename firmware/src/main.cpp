#include <M5Unified.h>
#include <WiFi.h>
#include <time.h>
#include <SD.h>
#include "config.h"

// M5Paper SD card CS pin
#define SD_CS_PIN          4
#define WIFI_TIMEOUT_MS    30000
#define NTP_SERVER1        "pool.ntp.org"
#define NTP_SERVER2        "ntp.nict.jp"
// JST = UTC+9
#define JST_OFFSET_SEC     (9 * 3600)

static Config g_config;

static void displayError(const char* msg) {
    M5.Display.setTextColor(TFT_BLACK, TFT_WHITE);
    M5.Display.setTextSize(2);
    M5.Display.println("[ERROR]");
    M5.Display.println(msg);
    M5.Display.display();
    Serial.printf("[ERROR] %s\n", msg);
}

static bool mountSD() {
    // Explicitly set VSPI pins used by M5Paper for SD
    SPI.begin(18, 19, 23, SD_CS_PIN);
    if (!SD.begin(SD_CS_PIN, SPI, 25000000)) {
        Serial.println("[SD] Mount failed");
        return false;
    }
    Serial.println("[SD] Mounted");
    return true;
}

static bool connectWiFi(const String& ssid, const String& password) {
    if (ssid.isEmpty()) {
        Serial.println("[WiFi] SSID is empty");
        return false;
    }
    Serial.printf("[WiFi] Connecting to: %s\n", ssid.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - start > WIFI_TIMEOUT_MS) {
            Serial.println("\n[WiFi] Connection timeout");
            return false;
        }
        delay(500);
        Serial.print(".");
    }
    Serial.printf("\n[WiFi] Connected, IP: %s\n", WiFi.localIP().toString().c_str());
    return true;
}

static void syncNTP() {
    configTime(JST_OFFSET_SEC, 0, NTP_SERVER1, NTP_SERVER2);
    Serial.print("[NTP] Syncing");
    struct tm timeinfo;
    for (int i = 0; i < 20; i++) {
        if (getLocalTime(&timeinfo)) {
            char buf[32];
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
            Serial.printf(" done: %s\n", buf);
            return;
        }
        delay(500);
        Serial.print(".");
    }
    Serial.println(" timeout");
}

static void drawStatusScreen(const String& ssid, const IPAddress& ip) {
    struct tm timeinfo = {};
    char timebuf[32] = "--:--:--";
    if (getLocalTime(&timeinfo)) {
        strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    }

    int battery = M5.Power.getBatteryLevel();

    M5.Display.clearDisplay(TFT_WHITE);
    M5.Display.setTextColor(TFT_BLACK, TFT_WHITE);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(10, 10);
    M5.Display.printf("SSID : %s\n\n", ssid.c_str());
    M5.Display.printf("IP   : %s\n\n", ip.toString().c_str());
    M5.Display.printf("Time : %s\n\n", timebuf);
    M5.Display.printf("Batt : %d%%\n", battery);
    M5.Display.display();

    Serial.printf("[Status] SSID=%s IP=%s Time=%s Batt=%d%%\n",
        ssid.c_str(), ip.toString().c_str(), timebuf, battery);
}

void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.println("[Boot] Start");

    // Init M5Unified (auto-detects M5Paper hardware)
    auto cfg = M5.config();
    M5.begin(cfg);

    // Full e-paper refresh to eliminate ghosting
    M5.Display.setEpdMode(epd_mode_t::epd_quality);
    M5.Display.clearDisplay(TFT_WHITE);
    M5.Display.setTextColor(TFT_BLACK, TFT_WHITE);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(10, 10);
    M5.Display.println("Booting...");
    M5.Display.display();

    // Mount SD card
    if (!mountSD()) {
        M5.Display.clearDisplay(TFT_WHITE);
        M5.Display.setCursor(10, 10);
        displayError("SD mount failed.\nInsert SD card and reboot.");
        return;
    }

    // Load config.json from SD card root
    if (!loadConfig(g_config)) {
        M5.Display.clearDisplay(TFT_WHITE);
        M5.Display.setCursor(10, 10);
        displayError("config.json read failed.\nPlace config.json on SD root.");
        return;
    }

    // Connect to WiFi (max 30s)
    if (!connectWiFi(g_config.wifi_ssid, g_config.wifi_password)) {
        M5.Display.clearDisplay(TFT_WHITE);
        M5.Display.setCursor(10, 10);
        displayError("WiFi connect failed.\nCheck SSID/password in config.json.");
        return;
    }

    // Sync time via NTP (JST)
    syncNTP();

    // Draw status screen
    drawStatusScreen(g_config.wifi_ssid, WiFi.localIP());

    Serial.println("[Boot] Complete");
}

void loop() {
    M5.update();
    delay(100);
}
