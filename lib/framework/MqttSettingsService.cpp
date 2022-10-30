#include <MqttSettingsService.h>

// #include "../../src/emsesp_stub.hpp" // proddy added
#include "../../src/default_settings.h" // proddy added

using namespace std::placeholders; // for `_1` etc

/**
 * Retains a copy of the cstr provided in the pointer provided using dynamic allocation.
 *
 * Frees the pointer before allocation and leaves it as nullptr if cstr == nullptr.
 */
static char * retainCstr(const char * cstr, char ** ptr) {
    // free up previously retained value if exists
    free(*ptr);
    *ptr = nullptr;

    // dynamically allocate and copy cstr (if non null)
    if (cstr != nullptr) {
        *ptr = (char *)malloc(strlen(cstr) + 1);
        strcpy(*ptr, cstr);
    }

    // return reference to pointer for convenience
    return *ptr;
}

MqttSettingsService::MqttSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(MqttSettings::read, MqttSettings::update, this, server, MQTT_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(MqttSettings::read, MqttSettings::update, this, fs, MQTT_SETTINGS_FILE)
    , _retainedHost(nullptr)
    , _retainedClientId(nullptr)
    , _retainedUsername(nullptr)
    , _retainedPassword(nullptr)
    , _reconfigureMqtt(false)
    , _disconnectedAt(0)
    , _disconnectReason(AsyncMqttClientDisconnectReason::TCP_DISCONNECTED)
    , _mqttClient() {
    WiFi.onEvent(std::bind(&MqttSettingsService::WiFiEvent, this, _1, _2));
    _mqttClient.onConnect(std::bind(&MqttSettingsService::onMqttConnect, this, _1));
    _mqttClient.onDisconnect(std::bind(&MqttSettingsService::onMqttDisconnect, this, _1));
    addUpdateHandler([&](const String & originId) { onConfigUpdated(); }, false);
}

MqttSettingsService::~MqttSettingsService() {
}

void MqttSettingsService::begin() {
    _fsPersistence.readFromFS();
}

void MqttSettingsService::loop() {
    if (_reconfigureMqtt || (_disconnectedAt && (uint32_t)(millis() - _disconnectedAt) >= MQTT_RECONNECTION_DELAY)) {
        // reconfigure MQTT client
        configureMqtt();

        // clear the reconnection flags
        _reconfigureMqtt = false;
        _disconnectedAt  = 0;
    }
}

bool MqttSettingsService::isEnabled() {
    return _state.enabled;
}

bool MqttSettingsService::isConnected() {
    return _mqttClient.connected();
}

const char * MqttSettingsService::getClientId() {
    return _mqttClient.getClientId();
}

AsyncMqttClientDisconnectReason MqttSettingsService::getDisconnectReason() {
    return _disconnectReason;
}

AsyncMqttClient * MqttSettingsService::getMqttClient() {
    return &_mqttClient;
}

void MqttSettingsService::onMqttConnect(bool sessionPresent) {
    // emsesp::EMSESP::logger().info("Connected to MQTT, %s", (sessionPresent) ? ("with persistent session") : ("without persistent session"));
}

void MqttSettingsService::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    // emsesp::EMSESP::logger().info("Disconnected from MQTT reason: %d", (uint8_t)reason);
    _disconnectReason = reason;
    _disconnectedAt   = millis();
}

void MqttSettingsService::onConfigUpdated() {
    _reconfigureMqtt = true;
    _disconnectedAt  = 0;

    // added by proddy
    // emsesp::EMSESP::mqtt_.start(); // reload EMS-ESP MQTT settings
}

void MqttSettingsService::WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
    switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
    case ARDUINO_EVENT_ETH_GOT_IP:
    case ARDUINO_EVENT_ETH_GOT_IP6:
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
        if (_state.enabled) {
            // emsesp::EMSESP::logger().info("IPv4 Network connection found, starting MQTT client");
            onConfigUpdated();
        }
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
    case ARDUINO_EVENT_ETH_DISCONNECTED:
        if (_state.enabled) {
            // emsesp::EMSESP::logger().info("Network connection dropped, stopping MQTT client");
            _mqttClient.disconnect();
            // onConfigUpdated();
        }
        break;

    default:
        break;
    }
}

void MqttSettingsService::configureMqtt() {
    // disconnect if connected
    _mqttClient.disconnect();
    // only connect if WiFi is connected and MQTT is enabled
    if (_state.enabled) {
        _mqttClient.setServer(retainCstr(_state.host.c_str(), &_retainedHost), _state.port);
        if (_state.username.length() > 0) {
            _mqttClient.setCredentials(retainCstr(_state.username.c_str(), &_retainedUsername),
                                       retainCstr(_state.password.length() > 0 ? _state.password.c_str() : nullptr, &_retainedPassword));
        } else {
            _mqttClient.setCredentials(retainCstr(nullptr, &_retainedUsername), retainCstr(nullptr, &_retainedPassword));
        }
        _mqttClient.setClientId(retainCstr(_state.clientId.c_str(), &_retainedClientId));
        _mqttClient.setKeepAlive(_state.keepAlive);
        _mqttClient.setCleanSession(_state.cleanSession);
        _mqttClient.setMaxTopicLength(_state.maxTopicLength);
        _mqttClient.connect();
        // } else {
        // emsesp::EMSESP::logger().info("Error configuring Mqtt client");
    }
}

void MqttSettings::read(MqttSettings & settings, JsonObject & root) {
    root["enabled"]          = settings.enabled;
    root["host"]             = settings.host;
    root["port"]             = settings.port;
    root["base"]             = settings.base;
    root["username"]         = settings.username;
    root["password"]         = settings.password;
    root["client_id"]        = settings.clientId;
    root["keep_alive"]       = settings.keepAlive;
    root["clean_session"]    = settings.cleanSession;
    root["max_topic_length"] = settings.maxTopicLength;

    // added by proddy for EMS-ESP
    root["publish_time_boiler"]     = settings.publish_time_boiler;
    root["publish_time_thermostat"] = settings.publish_time_thermostat;
    root["publish_time_solar"]      = settings.publish_time_solar;
    root["publish_time_mixer"]      = settings.publish_time_mixer;
    root["publish_time_other"]      = settings.publish_time_other;
    root["publish_time_sensor"]     = settings.publish_time_sensor;
    root["mqtt_qos"]                = settings.mqtt_qos;
    root["mqtt_retain"]             = settings.mqtt_retain;
    root["ha_enabled"]              = settings.ha_enabled;
    root["nested_format"]           = settings.nested_format;
    root["discovery_prefix"]        = settings.discovery_prefix;
    root["publish_single"]          = settings.publish_single;
    root["publish_single2cmd"]      = settings.publish_single2cmd;
    root["send_response"]           = settings.send_response;
}

StateUpdateResult MqttSettings::update(JsonObject & root, MqttSettings & settings) {
    MqttSettings newSettings = {};

    newSettings.enabled        = root["enabled"] | FACTORY_MQTT_ENABLED;
    newSettings.host           = root["host"] | FACTORY_MQTT_HOST;
    newSettings.port           = root["port"] | FACTORY_MQTT_PORT;
    newSettings.base           = root["base"] | FACTORY_MQTT_BASE;
    newSettings.username       = root["username"] | FACTORY_MQTT_USERNAME;
    newSettings.password       = root["password"] | FACTORY_MQTT_PASSWORD;
    newSettings.clientId       = root["client_id"] | FACTORY_MQTT_CLIENT_ID;
    newSettings.keepAlive      = root["keep_alive"] | FACTORY_MQTT_KEEP_ALIVE;
    newSettings.cleanSession   = root["clean_session"] | FACTORY_MQTT_CLEAN_SESSION;
    newSettings.maxTopicLength = root["max_topic_length"] | FACTORY_MQTT_MAX_TOPIC_LENGTH;
    newSettings.mqtt_qos       = root["mqtt_qos"] | EMSESP_DEFAULT_MQTT_QOS;
    newSettings.mqtt_retain    = root["mqtt_retain"] | EMSESP_DEFAULT_MQTT_RETAIN;

    newSettings.publish_time_boiler     = root["publish_time_boiler"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_thermostat = root["publish_time_thermostat"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_solar      = root["publish_time_solar"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_mixer      = root["publish_time_mixer"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_other      = root["publish_time_other"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_sensor     = root["publish_time_sensor"] | EMSESP_DEFAULT_PUBLISH_TIME;

    newSettings.ha_enabled         = root["ha_enabled"] | EMSESP_DEFAULT_HA_ENABLED;
    newSettings.nested_format      = root["nested_format"] | EMSESP_DEFAULT_NESTED_FORMAT;
    newSettings.discovery_prefix   = root["discovery_prefix"] | EMSESP_DEFAULT_DISCOVERY_PREFIX;
    newSettings.publish_single     = root["publish_single"] | EMSESP_DEFAULT_PUBLISH_SINGLE;
    newSettings.publish_single2cmd = root["publish_single2cmd"] | EMSESP_DEFAULT_PUBLISH_SINGLE2CMD;
    newSettings.send_response      = root["send_response"] | EMSESP_DEFAULT_SEND_RESPONSE;

    settings = newSettings;

    return StateUpdateResult::CHANGED;
}
