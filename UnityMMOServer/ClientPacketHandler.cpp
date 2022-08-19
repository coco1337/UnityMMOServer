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
	wcout << L"recieved" << endl;
	wcout << L"attack : " << pkt.attack() << endl;

	Protocol::SC_TEST sct;
	sct.set_id(PKT_SC_TEST);
	
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sct);
	session->Send(sendBuffer);
	return true;
}

bool Handle_CS_LOGIN(PacketSessionRef& session, Protocol::CS_LOGIN& pkt)
{
	return false;
}

bool Handle_CS_REGISTERREQ(PacketSessionRef& session, Protocol::CS_REGISTERREQ& pkt)
{
	cout << "id : " << pkt.userid() << endl;
	cout << "password : " << pkt.password() << endl;
	return false;
}

bool Handle_S_TEST(PacketSessionRef& session, Protocol::CS_TEST& pkt)
{
	return true;
}
