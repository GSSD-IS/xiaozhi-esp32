#ifndef WIFI_PROFILE_CONNECTOR_H
#define WIFI_PROFILE_CONNECTOR_H

#include "network_profile.h"

class WifiProfileConnector {
public:
    static WifiProfileConnector& GetInstance();

    bool Connect(const NetworkProfile& profile);

private:
    WifiProfileConnector() = default;

    bool ConnectPsk(const NetworkProfile& profile);
    bool ConnectPeap(const NetworkProfile& profile);
    void ResetEnterpriseConfig();
};

#endif  // WIFI_PROFILE_CONNECTOR_H
