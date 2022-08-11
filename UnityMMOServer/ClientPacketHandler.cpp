#include "pch.h"
#include "ClientPacketHandler.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& sesison, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : log
	return false;
}

bool Handle_CS_TEST(PacketSessionRef& session, Protocol::CS_TEST& pkt)
{
	return false;
}

bool Handle_CS_LOGIN(PacketSessionRef& session, Protocol::CS_LOGIN& pkt)
{
	return false;
}

bool Handle_S_TEST(PacketSessionRef& session, Protocol::CS_TEST& pkt)
{
	return true;
}
