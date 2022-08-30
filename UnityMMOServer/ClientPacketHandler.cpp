#include "pch.h"
#include "ClientPacketHandler.h"
#include "DBConnectionPool.h"
#include "GenProcedures.h"
#include "GameSession.h"
#include "Player.h"
#include "Room.h"
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
	Protocol::SC_REGISTER_RES res;
	res.set_id(PKT_SC_REGISTER_RES);

	auto* dbConn = GDBConnectionPool->Pop();
	wstring id = wstring(pkt.userid().begin(), pkt.userid().end());
	wstring pw = wstring(pkt.password().begin(), pkt.password().end());

	// TOOD : id, pw 예외 처리 필요(NVARCHAR값을 넘었는지 등 체크)

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
		GConsoleLogger->WriteStdErr(Color::RED, L"Handle_CS_REGISTER_REQ failed.\n");
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
	Protocol::SC_LOGIN_RES res;
	res.set_id(PKT_SC_LOGIN_RES);

	auto* dbConn = GDBConnectionPool->Pop();
	wstring id = wstring(pkt.userid().begin(), pkt.userid().end());
	wstring pw = wstring(pkt.password().begin(), pkt.password().end());

	WCHAR userId[50];
	WCHAR userPw[200];
	wmemset(userId, L'\0', 50);
	wmemset(userPw, L'\0', 200);
	wmemcpy(userId, id.c_str(), id.size());
	wmemcpy(userPw, pw.c_str(), pw.size());

	int32 uid = -1;

	SP::GetUserLoginInfo getUserLoginInfo(*dbConn);
	getUserLoginInfo.In_Id(userId);
	getUserLoginInfo.In_Pw(userPw);
	getUserLoginInfo.Out_Uid(OUT uid);

	if (!getUserLoginInfo.Execute() || !getUserLoginInfo.Fetch() || uid == -1)
	{
		GConsoleLogger->WriteStdErr(Color::RED, L"Handle_CS_LOGIN_REQ failed.\n");
		GDBConnectionPool->Push(dbConn);
		res.set_packetresult(PacketErrorType::PACKET_ERROR_TYPE_FAILED);
		res.set_uid(0); 
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
		session->Send(sendBuffer);
		return false;
	}

	GDBConnectionPool->Push(dbConn);

	res.set_packetresult(PacketErrorType::PACKET_ERROR_TYPE_SUCCESS);
	res.set_uid(uid);

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
	session->Send(sendBuffer);

	// create player	
	auto player = MakeShared<Player>();
	player->name = pkt.userid();
	player->playerId = uid;

	auto gameSession = static_pointer_cast<GameSession>(session);
	gameSession->_currentPlayer = player;
	player->ownerSession = gameSession;
	
	GRoom->DoAsync(&Room::Enter, player);

	return true;
}

bool Handle_CS_SEND_CHAT_REQ(PacketSessionRef& session, Protocol::CS_SEND_CHAT_REQ& pkt)
{
	Protocol::SC_CHAT_NOTI res;
	res.set_id(PKT_SC_CHAT_NOTI);

	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	res.set_senderid(gameSession->_currentPlayer->name);
	res.set_msg(pkt.msg());

	GRoom->DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(res));
	return true;
}

bool Handle_CS_SPAWN_REQ(PacketSessionRef& session, Protocol::CS_SPAWN_REQ& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);
	GRoom->DoAsync(&Room::HandleSpawn, gameSession->_currentPlayer);
	return true;
}

bool Handle_CS_MOVE_REQ(PacketSessionRef& session, Protocol::CS_MOVE_REQ& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);
	GRoom->DoAsync(&Room::HandleMove, gameSession->_currentPlayer, pkt);
	return true;
}
