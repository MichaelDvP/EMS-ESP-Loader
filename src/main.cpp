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

void setup() {
    Serial.begin(19200);
    if (!LittleFS.begin(true)) {
        return;
    }
    esp8266React.begin();
    init_eth();
    webServer.begin();
}

void loop() {
    esp8266React.loop(); // web services
}
