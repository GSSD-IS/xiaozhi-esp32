#ifndef NETWORK_PROFILE_STORE_H
#define NETWORK_PROFILE_STORE_H

#include "network_profile.h"

class NetworkProfileStore {
public:
    static NetworkProfileStore& GetInstance();

    bool HasProfile() const;
    bool Load(NetworkProfile& profile) const;
    bool Save(const NetworkProfile& profile);
    void Clear();

private:
    NetworkProfileStore() = default;
};

#endif  // NETWORK_PROFILE_STORE_H
