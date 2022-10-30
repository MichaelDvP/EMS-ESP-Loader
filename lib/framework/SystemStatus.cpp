#include <SystemStatus.h>

#include "../../src/version.h" // proddy added

using namespace std::placeholders; // for `_1` etc

SystemStatus::SystemStatus(AsyncWebServer * server, SecurityManager * securityManager) {
    server->on(SYSTEM_STATUS_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest(std::bind(&SystemStatus::systemStatus, this, _1), AuthenticationPredicates::IS_AUTHENTICATED));
}

std::string SystemStatus::format_timestamp_s() {
    uint16_t days;
    uint8_t  hours, minutes, seconds;
    uint32_t timestamp_ms = millis();

    days = timestamp_ms / 86400000UL;
    timestamp_ms %= 86400000UL;

    hours = timestamp_ms / 3600000UL;
    timestamp_ms %= 3600000UL;

    minutes = timestamp_ms / 60000UL;
    timestamp_ms %= 60000UL;

    seconds = timestamp_ms / 1000UL;
    timestamp_ms %= 1000UL;

    static std::vector<char> text(10 + 1 /* days */ + 2 + 1 /* hours */ + 2 + 1 /* minutes */ + 2 + 1 /* seconds */ + 1);

    snprintf(text.data(), text.size(), ("%u+%02u:%02u:%02u"), days, hours, minutes, seconds);

    return text.data();
}


void SystemStatus::systemStatus(AsyncWebServerRequest * request) {
    AsyncJsonResponse * response = new AsyncJsonResponse(false, MAX_ESP_STATUS_SIZE);
    JsonObject          root     = response->getRoot();

    root["emsesp_version"]   = EMSESP_APP_VERSION;
    root["esp_platform"]     = EMSESP_PLATFORM;
    root["cpu_freq_mhz"]     = ESP.getCpuFreqMHz();
    root["max_alloc_heap"]   = ESP.getMaxAllocHeap() / 1024;
    root["free_heap"]        = ESP.getFreeHeap() / 1024;
    root["sdk_version"]      = ESP.getSdkVersion();
    root["flash_chip_size"]  = ESP.getFlashChipSize() / 1024;
    root["flash_chip_speed"] = ESP.getFlashChipSpeed();
    uint16_t appused         = ESP.getSketchSize() / 1024;
    uint16_t appfree         = 0x140000  / 1024 - appused; // ESP.getFreeSketchSpace()
    root["app_used"]         = appused;
    root["app_free"]         = appfree;
    uint32_t FSused          = LittleFS.usedBytes() / 1024;
    root["fs_used"]          = FSused;
    root["fs_free"]          = LittleFS.totalBytes() / 1024 - FSused;
    root["uptime"]           = format_timestamp_s();
    root["psram_size"]       = ESP.getPsramSize() / 1024;
    root["free_psram"]       = ESP.getFreePsram() / 1024;

    response->setLength();
    request->send(response);
}
