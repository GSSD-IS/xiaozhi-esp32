#ifndef NETWORK_PROFILE_H
#define NETWORK_PROFILE_H

#include <string>

enum class NetworkAuthType : int {
    kPsk = 0,
    kPeap = 1,
};

struct NetworkProfile {
    NetworkAuthType auth_type = NetworkAuthType::kPsk;
    std::string ssid;
    std::string username;
    std::string password;

    bool IsEnterprise() const {
        return auth_type == NetworkAuthType::kPeap;
    }

    bool IsValid() const {
        if (ssid.empty()) {
            return false;
        }

        if (IsEnterprise()) {
            return !username.empty() && !password.empty();
        }

        return !password.empty();
    }
};

#endif  // NETWORK_PROFILE_H
