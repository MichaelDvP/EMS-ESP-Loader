/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020  Paul Derbyshire
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ESP8266React.h>

AsyncWebServer webServer(80);
ESP8266React   esp8266React(&webServer, &LittleFS);

void init_eth() {
#if CONFIG_IDF_TARGET_ESP32
    // default values for E32 gateway
    int8_t phy_type     = 1 + ETH_PHY_LAN8720;
    int8_t power        = 16;
    int8_t phy_addr     = 1;
    int8_t clk_mode     = ETH_CLOCK_GPIO0_IN;
    File   settingsFile = LittleFS.open("/config/emsespSettings.json");
    if (settingsFile) {
        DynamicJsonDocument  root  = DynamicJsonDocument(FS_BUFFER_SIZE * 2);
        DeserializationError error = deserializeJson(root, settingsFile);
        if (error == DeserializationError::Ok && root.is<JsonObject>()) {
            phy_type = root["phy_type"] | (1 + ETH_PHY_LAN8720);
            power    = root["eth_power"] | 16;
            phy_addr = root["eth_phy_addr"] | 1;
            clk_mode = root["eth_clock_mode"] | ETH_CLOCK_GPIO0_IN;
        }
    }
    if (phy_type > 0) {
        ETH.begin(phy_addr, power, 23, 18, (eth_phy_type_t)(phy_type - 1), (eth_clock_mode_t)clk_mode);
    }
#endif
}

// ems-esp filenames
#define MQTT_SETTINGS_FILE "/config/mqttSettings.json"
#define EMSESP_SETTINGS_FILE "/config/emsespSettings.json"
#define EMSESP_CUSTOMIZATION_FILE "/config/emsespCustomization.json"
#define EMSESP_SCHEDULER_FILE "/config/emsespScheduler.json"
#define EMSESP_ENTITY_FILE "/config/emsespEntity.json"

// save settings file using input from a json object
bool saveSettings(const char * filename, const char * section, JsonObject & input) {
    JsonObject section_json = input[section];
    if (section_json) {
        File section_file = LittleFS.open(filename, "w");
        if (section_file) {
            serializeJson(section_json, section_file);
            section_file.close();
            return true; // reboot required
        }
    }
    return false; // not found
}

bool check_restore() {
    bool reboot_required = false;
    // see if we have a temp file, if so try and read it
    File new_file = LittleFS.open(TEMP_FILENAME_PATH);
    if (new_file) {
        DynamicJsonDocument  jsonDocument = DynamicJsonDocument(FS_BUFFER_SIZE * 8);
        DeserializationError error        = deserializeJson(jsonDocument, new_file);
        if (error == DeserializationError::Ok && jsonDocument.is<JsonObject>()) {
            JsonObject input = jsonDocument.as<JsonObject>();
            // see what type of file it is, either settings or customization. anything else is ignored
            std::string settings_type = input["type"];
            if (settings_type == "settings") {
                // It's a settings file. Parse each section separately. If it's system related it will require a reboot
                reboot_required = saveSettings(NETWORK_SETTINGS_FILE, "Network", input);
                reboot_required |= saveSettings(AP_SETTINGS_FILE, "AP", input);
                reboot_required |= saveSettings(MQTT_SETTINGS_FILE, "MQTT", input);
                reboot_required |= saveSettings(NTP_SETTINGS_FILE, "NTP", input);
                reboot_required |= saveSettings(SECURITY_SETTINGS_FILE, "Security", input);
                reboot_required |= saveSettings(EMSESP_SETTINGS_FILE, "Settings", input);
                reboot_required |= saveSettings(OTA_SETTINGS_FILE, "OTA", input);
            } else if (settings_type == "customizations") {
                // it's a customization file, just replace it and there's no need to reboot
                saveSettings(EMSESP_CUSTOMIZATION_FILE, "Customizations", input);
            } else if (settings_type == "schedule") {
                // it's a schedule file, just replace it and there's no need to reboot
                saveSettings(EMSESP_SCHEDULER_FILE, "Schedule", input);
            } else if (settings_type == "entities") {
                // it's a entity file, just replace it and there's no need to reboot
                saveSettings(EMSESP_ENTITY_FILE, "Entities", input);
            }
        }
        // close (just in case) and remove the temp file
        new_file.close();
        LittleFS.remove(TEMP_FILENAME_PATH);
    }
    return reboot_required;
}

void system_restart() {
    delay(1000);
    ESP.restart();
}

void setup() {
    Serial.begin(19200);
    LittleFS.begin(true);
    esp8266React.begin();
    init_eth();
    if (check_restore()) {
        system_restart();
    }
    webServer.begin();
}

void loop() {
    esp8266React.loop(); // web services
}
