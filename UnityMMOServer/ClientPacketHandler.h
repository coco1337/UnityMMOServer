
#pragma once
#include "BufferReader.h"
#include "BufferWriter.h"
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

// TODO
enum : uint16 {
  PKT_SC_TEST = 1000,
  PKT_CS_TEST = 1001,
  PKT_CS_LOGIN = 1002,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_CS_TEST(PacketSessionRef& session, Protocol::CS_TEST& pkt);
bool Handle_CS_LOGIN(PacketSessionRef& session, Protocol::CS_LOGIN& pkt);

class ClientPacketHandler
{
public:
	static void Init() {
    for (int32 i = 0; i < UINT16_MAX; ++i)
			GPacketHandler[0] = Handle_INVALID;
    GPacketHandler[PKT_CS_TEST] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CS_TEST>(Handle_CS_TEST, session, buffer, len); };
    GPacketHandler[PKT_CS_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CS_LOGIN>(Handle_CS_LOGIN, session, buffer, len); };
  }

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len) {
    PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
    return GPacketHandler[header->id](session, buffer, len);
  }

	// TODO
	static SendBufferRef MakeSendBuffer(Protocol::SC_TEST& pkt) { return _MakeSendBuffer(pkt, PKT_SC_TEST); }

private:
  template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len) {
    PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
      return false;

    return func(session, pkt);

  }

  template<typename T>
	static SendBufferRef _MakeSendBuffer(T& pkt, uint16 pktId) {
    const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
    const uint16 packetSize = dataSize + sizeof(PacketHeader);

    SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);

    PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
    header->size = packetSize;
    header->id = pktId;

    ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));

    sendBuffer->Close(packetSize);

    return sendBuffer;
  }
};
