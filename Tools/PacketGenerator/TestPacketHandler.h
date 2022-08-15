
#pragma once
#include "BufferReader.h"
#include "BufferWriter.h"
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

// TODO
enum : uint16 {
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);

class TestPacketHandler
{
public:
	static void Init() {
    for (int32 i = 0; i < UINT16_MAX; ++i)
			GPacketHandler[0] = Handle_INVALID;
  }

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len) {
    PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
    return GPacketHandler[header->id](session, buffer, len);
  }

	// TODO

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
