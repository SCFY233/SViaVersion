#include "ProtocolTranslator.h"
#include "SViaVersion.h"
#include <sculk/protocol/utility/ReadOnlyBinaryStream.hpp>
#include <sculk/protocol/utility/BinaryStream.hpp>

namespace svviaversion {

ProtocolTranslator& ProtocolTranslator::getInstance() {
    static ProtocolTranslator instance;
    return instance;
}

ProtocolTranslator::ProtocolTranslator() {
    initMappings();
}

int ProtocolTranslator::getClientProtocolVersion(NetworkIdentifier const& netId) {
    // TODO: 从 NetworkIdentifier 或连接信息中获取客户端协议版本
    // 临时返回 898 表示同版本
    return 898;
}

void ProtocolTranslator::initMappings() {
    // 初始化包ID映射
    // 示例：协议 1001 -> 898 的映射
    // 这些值需要根据实际协议差异填写
    
    // LoginPacket
    mPacketIdMap[1001][0x01] = 0x01;
    // PlayStatusPacket
    mPacketIdMap[1001][0x02] = 0x02;
    // ServerToClientHandshakePacket
    mPacketIdMap[1001][0x03] = 0x03;
    // ... 更多映射
    
    SViaVersion::getInstance().getLogger().info("Protocol mappings initialized");
}

int ProtocolTranslator::readPacketId(const std::vector<std::byte>& data) {
    if (data.empty()) return -1;
    
    sculk::protocol::ReadOnlyBinaryStream stream(
        std::span<const std::byte>(data.data(), data.size())
    );
    
    // Bedrock 包头：第一个字节通常是 Packet ID（压缩后）
    // 实际格式可能需要根据协议调整
    std::uint32_t packetId = 0;
    auto result = stream.readUnsignedVarInt(packetId);
    
    if (!result || stream.isOverflowed()) {
        return -1;
    }
    
    return static_cast<int>(packetId);
}

void ProtocolTranslator::writePacketId(std::vector<std::byte>& data, int packetId) {
    std::vector<std::byte> newData;
    sculk::protocol::BinaryStream stream(newData);
    
    stream.writeUnsignedVarInt(static_cast<std::uint32_t>(packetId));
    
    // 写入剩余数据（跳过原来的包头）
    // 实际需要解析原包长度后追加
    
    data = std::move(newData);
}

bool ProtocolTranslator::translatePacket(
    std::vector<std::byte>& data,
    int fromProtocol,
    int toProtocol
) {
    if (fromProtocol == toProtocol) {
        return true;
    }
    
    int originalPacketId = readPacketId(data);
    if (originalPacketId < 0) {
        SViaVersion::getInstance().getLogger().warn("Failed to read packet ID");
        return false;
    }
    
    // 查找映射
    auto& map = mPacketIdMap[fromProtocol];
    auto it = map.find(originalPacketId);
    
    if (it != map.end()) {
        int newPacketId = it->second;
        writePacketId(data, newPacketId);
        SViaVersion::getInstance().getLogger().debug(
            "Translated packet {} -> {} ({} -> {})",
            originalPacketId, newPacketId, fromProtocol, toProtocol
        );
    }
    
    // TODO: 字段级转换
    // 用 BedrockProtocol 的 createPacket -> read -> 修改字段 -> writeWithHeader
    
    return true;
}

} // namespace svviaversion