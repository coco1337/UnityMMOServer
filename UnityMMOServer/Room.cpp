#include "pch.h"
#include "ClientPacketHandler.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"

shared_ptr<Room> GRoom = make_shared<Room>();

void Room::Enter(PlayerRef player)
{
	_players[player->playerId] = player;
	player->Init();
	auto gameSession = player->ownerSession.lock();
	if (gameSession == nullptr)
	{
		GConsoleLogger->WriteStdErr(Color::RED, L"Enter error\n");
		return;
	}
	
	GConsoleLogger->WriteStdOut(Color::BLUE, L"player joined %d\n", player->playerId);
	gameSession->_room = GRoom;

	// broadcast other player
	Protocol::SC_SPAWN_NOTI noti;
	noti.set_id(PKT_SC_SPAWN_NOTI);
	Protocol::PlayerData playerData;
	playerData.set_uid(player->playerId);
	playerData.set_username(player->name);
	playerData.mutable_movedata()->CopyFrom(player->GetMoveData());
	noti.mutable_player()->CopyFrom(playerData);

	BroadcastOtherPlayers(ClientPacketHandler::MakeSendBuffer(noti), player->playerId);
}

void Room::Leave(PlayerRef player)
{
	GConsoleLogger->WriteStdOut(Color::BLUE, L"player leave %d\n", player->playerId);

	Protocol::SC_DESPAWN_NOTI noti;
	noti.set_id(PKT_SC_DESPAWN_NOTI);
	noti.set_uid(player->playerId);
	BroadcastOtherPlayers(ClientPacketHandler::MakeSendBuffer(noti), player->playerId);
	_players.erase(player->playerId);	
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
	GameSessionRef gameSession;
	for (auto& p : _players)
	{
		gameSession = p.second->ownerSession.lock();
		if (gameSession == nullptr) continue;
		gameSession->Send(sendBuffer);
	}
}

void Room::BroadcastOtherPlayers(SendBufferRef sendBuffer, uint64 uid)
{
	GameSessionRef gameSession;
	for (auto& p : _players)
	{
		if (p.first != uid)
		{
			gameSession = p.second->ownerSession.lock();
			if (gameSession == nullptr) continue;
			gameSession->Send(sendBuffer);
		}
	}
}

bool Room::GetAllPlayerData(OUT Vector<Protocol::PlayerData>& players)
{
	for (auto _player : _players)
	{
		Protocol::PlayerData player;

		player.set_uid(_player.first);
		player.set_username(_player.second->name);
		player.mutable_movedata()->CopyFrom(_player.second->GetMoveData());

		players.push_back(player);
	}

	return true;
}

void Room::HandleSpawn(PlayerRef player)
{
	Protocol::SC_SPAWN_RES res;
	for (auto _player : _players)
	{
		Protocol::PlayerData p;

		p.set_uid(_player.first);
		p.set_username(_player.second->name);
		p.mutable_movedata()->CopyFrom(_player.second->GetMoveData());

		*res.mutable_players()->Add() = p;
	}
	
	res.set_id(PKT_SC_SPAWN_RES);
	res.set_packetresult(Protocol::PacketErrorType::PACKET_ERROR_TYPE_SUCCESS);
	res.set_myid(player->playerId);

	auto gameSession = player->ownerSession.lock();
	gameSession->Send(ClientPacketHandler::MakeSendBuffer(res));
}

void Room::HandleMove(PlayerRef player, Protocol::CS_MOVE_REQ pkt)
{
	Protocol::SC_MOVEDATA_NOTI noti;
	noti.set_id(PKT_SC_MOVEDATA_NOTI);

	if (player == nullptr) return;

	auto& moveData = pkt.movedata();
	auto playerMoveData = player->GetMoveData();

	// TODO : 맵 정보 확인해서 이동할 수 있는지 체크, 못하면 노티 안보내기

	player->UpdateMoveData(moveData);

	// broadcast
	noti.set_uid(player->playerId);
	noti.mutable_movedata()->CopyFrom(moveData);

	Broadcast(ClientPacketHandler::MakeSendBuffer(noti));
}
