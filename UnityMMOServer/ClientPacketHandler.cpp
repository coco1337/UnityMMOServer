#include "pch.h"
#include "ClientPacketHandler.h"
#include "DBConnectionPool.h"
#include "GenProcedures.h"
using Protocol::PacketErrorType;

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& sesison, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : log
	return false;
}

bool Handle_CS_REGISTER_REQ(PacketSessionRef& session, Protocol::CS_REGISTER_REQ& pkt)
{
	cout << "id : " << pkt.userid() << endl;
	cout << "password : " << pkt.password() << endl;

	Protocol::SC_REGISTER_RES res;
	res.set_id(PKT_SC_REGISTER_RES);

	auto* dbConn = GDBConnectionPool->Pop();
	wstring id = wstring(pkt.userid().begin(), pkt.userid().end());
	wstring pw = wstring(pkt.password().begin(), pkt.password().end());

	WCHAR userId[50];
	WCHAR userPw[200];
	wmemset(userId, L'\0', 50);
	wmemset(userPw, L'\0', 200);
	wmemcpy(userId, id.c_str(), id.size());
	wmemcpy(userPw, pw.c_str(), pw.size());

	SP::RegisterUser registerUser(*dbConn);
	registerUser.In_AccId(userId);
	registerUser.In_AccPw(userPw);

	if (!registerUser.Execute())
	{
		wcout << L"Handle_CS_REGISTER_REQ failed." << endl;
		GDBConnectionPool->Push(dbConn);

		res.set_packetresult(PacketErrorType::PACKET_ERROR_TYPE_FAILED);

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
		session->Send(sendBuffer);
		return false;
	}
	
	GDBConnectionPool->Push(dbConn);
	res.set_packetresult(PacketErrorType::PACKET_ERROR_TYPE_SUCCESS);
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
	session->Send(sendBuffer);

	return true;
}

bool Handle_CS_LOGIN_REQ(PacketSessionRef& session, Protocol::CS_LOGIN_REQ& pkt)
{
	return false;
}