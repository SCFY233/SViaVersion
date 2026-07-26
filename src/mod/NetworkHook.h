#pragma once
#include <vector>
#include <cstddef>

namespace svviaversion {

class NetworkHook {
public:
    static NetworkHook& getInstance();
    
    bool setup();
    void cleanup();
    
    // 被Hook调用的转换入口
    void onPacketSend(class NetworkIdentifier const& netId, std::vector<std::byte>& packetData);
    void onPacketReceive(class NetworkIdentifier const& netId, std::vector<std::byte>& packetData);

private:
    bool mSetup = false;
};

} // namespace svviaversion