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

void setup() {
    Serial.begin(19200);
    if (!LittleFS.begin(true)) {
        return;
    }
    esp8266React.begin();
#if CONFIG_IDF_TARGET_ESP32
    ETH.begin(1, 16, 23, 18, ETH_PHY_LAN8720); // E32
#endif
    webServer.begin();
}

void loop() {
    esp8266React.loop(); // web services
}
