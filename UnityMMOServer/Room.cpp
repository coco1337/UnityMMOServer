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
	noti.set_id(player->playerId);

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
		tuple<float, float, float> playerPos;
		tuple<float, float, float> playerRot;

		player.set_uid(_player.first);
		player.set_username(_player.second->name);

		{
			auto pos = MakeShared<Protocol::Position>();
			playerPos = _player.second->GetPos();
			pos->set_x(get<0>(playerPos));
			pos->set_y(get<1>(playerPos));
			pos->set_z(get<2>(playerPos));
			player.mutable_position()->CopyFrom(*pos);
		}

		{
			auto rot = MakeShared<Protocol::Rotation>();
			playerRot = _player.second->GetRot();
			rot->set_x(get<0>(playerRot));
			rot->set_y(get<1>(playerRot));
			rot->set_z(get<2>(playerRot));
			player.mutable_rotation()->CopyFrom(*rot);
		}

		players.push_back(player);
	}

	return true;
}
