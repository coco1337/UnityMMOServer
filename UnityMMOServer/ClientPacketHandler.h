
#pragma once
#include "BufferReader.h"
#include "BufferWriter.h"
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

// TODO
enum : uint16 {
  PKT_CS_REGISTER_REQ = 1000,
  PKT_SC_REGISTER_RES = 1001,
  PKT_CS_LOGIN_REQ = 1002,
  PKT_SC_LOGIN_RES = 1003,
  PKT_CS_SEND_CHAT_REQ = 1004,
  PKT_SC_CHAT_NOTI = 1005,
  PKT_CS_SPAWN_REQ = 1006,
  PKT_SC_SPAWN_RES = 1007,
  PKT_SC_SPAWN_NOTI = 1008,
  PKT_SC_DESPAWN_NOTI = 1009,
  PKT_CS_MOVE_REQ = 1010,
  PKT_SC_MOVEDATA_NOTI = 1011,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_CS_REGISTER_REQ(PacketSessionRef& session, Protocol::CS_REGISTER_REQ& pkt);
bool Handle_CS_LOGIN_REQ(PacketSessionRef& session, Protocol::CS_LOGIN_REQ& pkt);
bool Handle_CS_SEND_CHAT_REQ(PacketSessionRef& session, Protocol::CS_SEND_CHAT_REQ& pkt);
bool Handle_CS_SPAWN_REQ(PacketSessionRef& session, Protocol::CS_SPAWN_REQ& pkt);
bool Handle_CS_MOVE_REQ(PacketSessionRef& session, Protocol::CS_MOVE_REQ& pkt);

class ClientPacketHandler
{
public:
	static void Init() {
    for (int32 i = 0; i < UINT16_MAX; ++i)
			GPacketHandler[0] = Handle_INVALID;
    GPacketHandler[PKT_CS_REGISTER_REQ] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CS_REGISTER_REQ>(Handle_CS_REGISTER_REQ, session, buffer, len); };
    GPacketHandler[PKT_CS_LOGIN_REQ] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CS_LOGIN_REQ>(Handle_CS_LOGIN_REQ, session, buffer, len); };
    GPacketHandler[PKT_CS_SEND_CHAT_REQ] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CS_SEND_CHAT_REQ>(Handle_CS_SEND_CHAT_REQ, session, buffer, len); };
    GPacketHandler[PKT_CS_SPAWN_REQ] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CS_SPAWN_REQ>(Handle_CS_SPAWN_REQ, session, buffer, len); };
    GPacketHandler[PKT_CS_MOVE_REQ] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CS_MOVE_REQ>(Handle_CS_MOVE_REQ, session, buffer, len); };
  }

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len) {
    PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
    return GPacketHandler[header->id](session, buffer, len);
  }

	// TODO
	static SendBufferRef MakeSendBuffer(Protocol::SC_REGISTER_RES& pkt) { return _MakeSendBuffer(pkt, PKT_SC_REGISTER_RES); }
	static SendBufferRef MakeSendBuffer(Protocol::SC_LOGIN_RES& pkt) { return _MakeSendBuffer(pkt, PKT_SC_LOGIN_RES); }
	static SendBufferRef MakeSendBuffer(Protocol::SC_CHAT_NOTI& pkt) { return _MakeSendBuffer(pkt, PKT_SC_CHAT_NOTI); }
	static SendBufferRef MakeSendBuffer(Protocol::SC_SPAWN_RES& pkt) { return _MakeSendBuffer(pkt, PKT_SC_SPAWN_RES); }
	static SendBufferRef MakeSendBuffer(Protocol::SC_SPAWN_NOTI& pkt) { return _MakeSendBuffer(pkt, PKT_SC_SPAWN_NOTI); }
	static SendBufferRef MakeSendBuffer(Protocol::SC_DESPAWN_NOTI& pkt) { return _MakeSendBuffer(pkt, PKT_SC_DESPAWN_NOTI); }
	static SendBufferRef MakeSendBuffer(Protocol::SC_MOVEDATA_NOTI& pkt) { return _MakeSendBuffer(pkt, PKT_SC_MOVEDATA_NOTI); }

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
