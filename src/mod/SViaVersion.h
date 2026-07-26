#pragma once
#include <ll/api/mod/NativeMod.h>
#include <ll/api/Logger.h>
#include <memory>

namespace svviaversion {

class SViaVersion {
public:
    static SViaVersion& getInstance();
    
    bool load();
    bool enable();
    bool disable();
    
    ll::Logger& getLogger() const;

private:
    ll::mod::NativeMod& mSelf;
    std::unique_ptr<ll::Logger> mLogger;
};

} // namespace svviaversion