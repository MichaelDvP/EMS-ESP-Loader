#include <MqttStatus.h>

// #include "../../src/emsesp_stub.hpp" // proddy added

using namespace std::placeholders; // for `_1` etc

MqttStatus::MqttStatus(AsyncWebServer * server, MqttSettingsService * mqttSettingsService, SecurityManager * securityManager)
    : _mqttSettingsService(mqttSettingsService) {
    server->on(MQTT_STATUS_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest(std::bind(&MqttStatus::mqttStatus, this, _1), AuthenticationPredicates::IS_AUTHENTICATED));
}

void MqttStatus::mqttStatus(AsyncWebServerRequest * request) {
    AsyncJsonResponse * response = new AsyncJsonResponse(false, MAX_MQTT_STATUS_SIZE);
    JsonObject          root     = response->getRoot();

    root["enabled"]           = _mqttSettingsService->isEnabled();
    root["client_id"]         = _mqttSettingsService->getClientId();

    response->setLength();
    request->send(response);
}
