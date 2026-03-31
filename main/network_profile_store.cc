#include "network_profile_store.h"

#include "settings.h"

#include <esp_log.h>

namespace {

constexpr char kSettingsNamespace[] = "netcfg";
constexpr char kVersionKey[] = "version";
constexpr char kAuthTypeKey[] = "auth";
constexpr char kSsidKey[] = "ssid";
constexpr char kUsernameKey[] = "user";
constexpr char kPasswordKey[] = "pass";
constexpr char kLegacySettingsNamespace[] = "wifi";
constexpr char kLegacySsidKey[] = "ssid";
constexpr char kLegacyPasswordKey[] = "password";
constexpr int kCurrentVersion = 1;

const char* kTag = "NetworkProfileStore";

bool TryLoadLegacyProfile(NetworkProfile& profile) {
    Settings legacy_settings(kLegacySettingsNamespace, false);
    auto ssid = legacy_settings.GetString(kLegacySsidKey);
    auto password = legacy_settings.GetString(kLegacyPasswordKey);
    if (ssid.empty() || password.empty()) {
        return false;
    }

    profile = {};
    profile.auth_type = NetworkAuthType::kPsk;
    profile.ssid = ssid;
    profile.password = password;
    return true;
}

}  // namespace

NetworkProfileStore& NetworkProfileStore::GetInstance() {
    static NetworkProfileStore instance;
    return instance;
}

bool NetworkProfileStore::HasProfile() const {
    Settings settings(kSettingsNamespace, false);
    if (!settings.GetString(kSsidKey).empty()) {
        return true;
    }

    NetworkProfile legacy_profile;
    return TryLoadLegacyProfile(legacy_profile);
}

bool NetworkProfileStore::Load(NetworkProfile& profile) const {
    Settings settings(kSettingsNamespace, false);
    if (settings.GetInt(kVersionKey, 0) <= 0) {
        if (TryLoadLegacyProfile(profile)) {
            ESP_LOGI(kTag, "Loaded legacy PSK WiFi profile from wifi namespace");
            return true;
        }
        return false;
    }

    profile = {};
    profile.auth_type = static_cast<NetworkAuthType>(
        settings.GetInt(kAuthTypeKey, static_cast<int>(NetworkAuthType::kPsk)));
    profile.ssid = settings.GetString(kSsidKey);
    profile.username = settings.GetString(kUsernameKey);
    profile.password = settings.GetString(kPasswordKey);

    if (!profile.IsValid()) {
        ESP_LOGW(kTag, "Stored network profile is invalid");
        return false;
    }

    return true;
}

bool NetworkProfileStore::Save(const NetworkProfile& profile) {
    if (!profile.IsValid()) {
        ESP_LOGE(kTag, "Refusing to save invalid network profile");
        return false;
    }

    Settings settings(kSettingsNamespace, true);
    settings.SetInt(kVersionKey, kCurrentVersion);
    settings.SetInt(kAuthTypeKey, static_cast<int>(profile.auth_type));
    settings.SetString(kSsidKey, profile.ssid);
    settings.SetString(kPasswordKey, profile.password);

    if (profile.IsEnterprise()) {
        settings.SetString(kUsernameKey, profile.username);
    } else {
        settings.EraseKey(kUsernameKey);
    }

    return true;
}

void NetworkProfileStore::Clear() {
    Settings settings(kSettingsNamespace, true);
    settings.EraseAll();
}
