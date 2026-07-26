#pragma once
#include <vector>
#include <cstddef>
#include <unordered_map>

// 前向声明
class NetworkIdentifier;

namespace svviaversion {

class ProtocolTranslator {
public:
    static ProtocolTranslator& getInstance();
    
    // 获取客户端协议版本
    int getClientProtocolVersion(NetworkIdentifier const& netId);
    
    // 核心转换：将 input 从 fromProtocol 转换到 toProtocol
    bool translatePacket(
        std::vector<std::byte>& data,
        int fromProtocol,
        int toProtocol
    );

private:
    ProtocolTranslator();
    
    // 包ID映射表
    std::unordered_map<int, std::unordered_map<int, int>> mPacketIdMap;
    
    void initMappings();
    
    // 用 BedrockProtocol 解析包头
    int readPacketId(const std::vector<std::byte>& data);
    void writePacketId(std::vector<std::byte>& data, int packetId);
};

} // namespace svviaversion