#include "pch.h"
#include "ClientPacketHandler.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"

shared_ptr<Room> GRoom = make_shared<Room>();

void Room::Enter(PlayerRef player)
{
	_players[player->playerId] = player;
	GConsoleLogger->WriteStdOut(Color::BLUE, L"player joined %d\n", player->playerId);

	// broadcast other player
	Protocol::SC_SPAWN_NOTI noti;
	noti.set_id(PKT_SC_SPAWN_NOTI);

	BroadcastOtherPlayers(ClientPacketHandler::MakeSendBuffer(noti), player->playerId);
}

void Room::Leave(PlayerRef player)
{
	_players.erase(player->playerId);
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
	GameSessionRef gameSession;
	for (auto& p : _players)
	{
		gameSession = p.second->ownerSession.lock();
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

	Broadcast(ClientPacketHandler::MakeSendBuffer(res));
}

void Room::HandleMove(PlayerRef player, Protocol::CS_MOVE_REQ pkt)
{
	Protocol::SC_MOVEDATA_NOTI noti;
	noti.set_id(PKT_SC_MOVEDATA_NOTI);

	if (player == nullptr) return;

	auto& moveData = pkt.movedata();
	auto playerMoveData = player->GetMoveData();

	// TODO : �� ���� Ȯ���ؼ� �̵��� �� �ִ��� üũ, ���ϸ� ��Ƽ �Ⱥ�����

	player->UpdateMoveData(moveData);

	// broadcast
	noti.set_uid(player->playerId);
	noti.mutable_movedata()->CopyFrom(moveData);

	Broadcast(ClientPacketHandler::MakeSendBuffer(noti));
}
