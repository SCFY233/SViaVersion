#include "NetworkHook.h"
#include "ProtocolTranslator.h"
#include "SViaVersion.h"
#include <ll/api/memory/Hook.h>
#include <ll/api/service/Bedrock.h>

// 你需要用工具找到准确的装饰名
// 先用占位符，编译时替换为实际符号
INSTANCE_HOOK(
    SendPacketHook,
    ll::memory::HookPriority::Normal,
    ServerNetworkHandler,
    "?send@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVPacket@@E@Z",
    void,
    class NetworkIdentifier const& netId,
    class Packet const& packet,
    unsigned char subClientId
) {
    // 获取包的原始字节
    // BDS 的 Packet 类有 serialize 方法或类似机制
    // 这里假设我们能拿到字节流，实际需要根据BDS符号调整
    
    std::vector<std::byte> packetData; // = packet.serialize();
    
    // 调用转换
    NetworkHook::getInstance().onPacketSend(netId, packetData);
    
    // 如果转换后数据变化，需要重新构造Packet发送
    // 这里简化处理，实际需根据BDS内部API调整
    origin(netId, packet, subClientId);
}

namespace svviaversion {

NetworkHook& NetworkHook::getInstance() {
    static NetworkHook instance;
    return instance;
}

bool NetworkHook::setup() {
    // INSTANCE_HOOK 宏在加载时自动注册，这里做额外初始化
    mSetup = true;
    SViaVersion::getInstance().getLogger().info("NetworkHook setup complete");
    return true;
}

void NetworkHook::cleanup() {
    mSetup = false;
}

void NetworkHook::onPacketSend(NetworkIdentifier const& netId, std::vector<std::byte>& packetData) {
    // 获取客户端协议版本
    int clientVersion = ProtocolTranslator::getInstance().getClientProtocolVersion(netId);
    int serverVersion = 898;
    
    if (clientVersion != serverVersion) {
        ProtocolTranslator::getInstance().translatePacket(packetData, clientVersion, serverVersion);
    }
}

void NetworkHook::onPacketReceive(NetworkIdentifier const& netId, std::vector<std::byte>& packetData) {
    int clientVersion = ProtocolTranslator::getInstance().getClientProtocolVersion(netId);
    int serverVersion = 898;
    
    if (clientVersion != serverVersion) {
        ProtocolTranslator::getInstance().translatePacket(packetData, serverVersion, clientVersion);
    }
}

} // namespace svviaversion