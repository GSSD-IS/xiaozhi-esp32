#include "wifi_profile_connector.h"

#include <cstring>

#include <esp_eap_client.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <wifi_manager.h>

namespace {

const char* kTag = "WifiProfileConnector";

bool CopyToWifiField(const std::string& value, uint8_t* field, size_t capacity,
                     const char* field_name) {
    if (value.size() >= capacity) {
        ESP_LOGE(kTag, "%s is too long", field_name);
        return false;
    }

    memset(field, 0, capacity);
    memcpy(field, value.data(), value.size());
    return true;
}

bool LogOnError(esp_err_t err, const char* message) {
    if (err != ESP_OK) {
        ESP_LOGE(kTag, "%s: %s", message, esp_err_to_name(err));
        return false;
    }
    return true;
}

}  // namespace

WifiProfileConnector& WifiProfileConnector::GetInstance() {
    static WifiProfileConnector instance;
    return instance;
}

bool WifiProfileConnector::Connect(const NetworkProfile& profile) {
    if (!profile.IsValid()) {
        ESP_LOGE(kTag, "Cannot connect with invalid profile");
        return false;
    }

    auto& wifi_manager = WifiManager::GetInstance();
    if (!wifi_manager.IsInitialized() && !wifi_manager.Initialize()) {
        ESP_LOGE(kTag, "Failed to initialize WiFi manager");
        return false;
    }

    if (wifi_manager.IsConfigMode()) {
        wifi_manager.StopConfigAp();
    }
    wifi_manager.StopStation();
    vTaskDelay(pdMS_TO_TICKS(200));

    if (profile.IsEnterprise()) {
        return ConnectPeap(profile);
    }

    return ConnectPsk(profile);
}

bool WifiProfileConnector::ConnectPsk(const NetworkProfile& profile) {
    wifi_config_t config = {};
    if (!CopyToWifiField(profile.ssid, config.sta.ssid, sizeof(config.sta.ssid), "SSID") ||
        !CopyToWifiField(profile.password, config.sta.password, sizeof(config.sta.password),
                         "password")) {
        return false;
    }

    ResetEnterpriseConfig();

    return LogOnError(esp_wifi_set_mode(WIFI_MODE_STA), "Failed to set station mode") &&
           LogOnError(esp_wifi_set_config(WIFI_IF_STA, &config), "Failed to configure station") &&
           LogOnError(esp_wifi_start(), "Failed to start WiFi") &&
           LogOnError(esp_wifi_connect(), "Failed to connect to WiFi");
}

bool WifiProfileConnector::ConnectPeap(const NetworkProfile& profile) {
    wifi_config_t config = {};
    if (!CopyToWifiField(profile.ssid, config.sta.ssid, sizeof(config.sta.ssid), "SSID")) {
        return false;
    }

    ResetEnterpriseConfig();

    const auto* identity = reinterpret_cast<const unsigned char*>(profile.username.c_str());
    const auto* username = reinterpret_cast<const unsigned char*>(profile.username.c_str());
    const auto* password = reinterpret_cast<const unsigned char*>(profile.password.c_str());

    return LogOnError(esp_wifi_set_mode(WIFI_MODE_STA), "Failed to set station mode") &&
           LogOnError(esp_wifi_set_config(WIFI_IF_STA, &config), "Failed to configure enterprise SSID") &&
           LogOnError(esp_eap_client_set_identity(identity, profile.username.size()),
                      "Failed to set enterprise identity") &&
           LogOnError(esp_eap_client_set_username(username, profile.username.size()),
                      "Failed to set enterprise username") &&
           LogOnError(esp_eap_client_set_password(password, profile.password.size()),
                      "Failed to set enterprise password") &&
           LogOnError(esp_wifi_sta_enterprise_enable(), "Failed to enable enterprise WiFi") &&
           LogOnError(esp_wifi_start(), "Failed to start enterprise WiFi") &&
           LogOnError(esp_wifi_connect(), "Failed to connect to enterprise WiFi");
}

void WifiProfileConnector::ResetEnterpriseConfig() {
    esp_wifi_disconnect();
    esp_wifi_sta_enterprise_disable();
    esp_eap_client_clear_identity();
    esp_eap_client_clear_username();
    esp_eap_client_clear_password();
}
