#include <RestartService.h>
#include <esp_ota_ops.h>

using namespace std::placeholders; // for `_1` etc

RestartService::RestartService(AsyncWebServer * server, SecurityManager * securityManager) {
    server->on(RESTART_SERVICE_PATH, HTTP_POST, securityManager->wrapRequest(std::bind(&RestartService::restart, this, _1), AuthenticationPredicates::IS_ADMIN));
    server->on(PARTITION_SERVICE_PATH, HTTP_POST, securityManager->wrapRequest(std::bind(&RestartService::partition, this, _1), AuthenticationPredicates::IS_ADMIN));
}

void RestartService::restart(AsyncWebServerRequest * request) {
    request->onDisconnect(RestartService::restartNow);
    request->send(200);
}

void RestartService::partition(AsyncWebServerRequest * request) {
    const esp_partition_t* partition = esp_ota_get_next_update_partition(NULL);
    if (!partition) {
        request->send(400); // bad request
        return;
    }
    uint64_t buffer;
    esp_partition_read(partition, 0, &buffer, sizeof(buffer));
    if (buffer == 0xFFFFFFFFFFFFFFFF) { // partition empty
        request->send(400); // bad request
        return;
    }
    esp_ota_set_boot_partition(partition);
    request->onDisconnect(RestartService::restartNow);
    request->send(200);
}
