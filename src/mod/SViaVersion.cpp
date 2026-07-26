#include "SVViaVersion.h"
#include "NetworkHook.h"
#include <ll/api/mod/RegisterHelper.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/command/CommandHandle.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>

namespace svviaversion {

SViaVersion& SViaVersion::getInstance() {
    static SViaVersion instance;
    return instance;
}

bool SViaVersion::load() {
    mLogger = std::make_unique<ll::Logger>("SViaVersion");
    getLogger().info("SViaVersion loading...");
    return true;
}

bool SViaVersion::enable() {
    getLogger().info("SViaVersion enabled!");
    
    // 注册命令
    auto& registrar = ll::command::CommandRegistrar::getInstance();
    auto& cmd = registrar.getOrCreateCommand(
        "svversion",
        "Check SViaVersion status",
        ll::command::CommandPermissionLevel::Any
    );
    
    cmd.overload().execute([](CommandOrigin const&, CommandOutput& output) {
        output.success("SViaVersion v0.1.0");
        output.success("Server protocol: 898 (1.21.132)");
    });
    
    // 注册网络Hook
    if (!NetworkHook::getInstance().setup()) {
        getLogger().error("Failed to setup network hooks!");
        return false;
    }
    
    getLogger().info("Network hooks registered");
    return true;
}

bool SViaVersion::disable() {
    NetworkHook::getInstance().cleanup();
    getLogger().info("SViaVersion disabled");
    return true;
}

ll::Logger& SViaVersion::getLogger() const {
    return *mLogger;
}

} // namespace svviaversion

LL_REGISTER_MOD(svviaversion::SViaVersion, svviaversion::SViaVersion::getInstance());