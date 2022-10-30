# ![logo](media/EMS-ESP_logo_dark.png)

**EMS-ESP** is an open-source firmware for the Espressif ESP8266 and ESP32 microcontroller that communicates with **EMS** (Energy Management System) based equipment from manufacturers like Bosch, Buderus, Nefit, Junkers, Worcester and Sieger.

This project is the specifically for the ESP32 and EMS-ESP v3.5 and above.
It is only a loader in a smaller factory or OTA-partitions, giving more memory space to EMS-ESP.

# **Libraries used**

- [esp8266-react](https://github.com/rjwats/esp8266-react) by @rjwats for the framework that provides the core of the Web UI
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) for JSON
- ESPAsyncWebServer and AsyncTCP for the Web server and TCP backends, with custom modifications for performance

# **License**

This program is licensed under GPL-3.0
