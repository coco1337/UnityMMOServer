#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"

shared_ptr<Room> GRoom = make_shared<Room>();

void Room::Enter(PlayerRef player)
{
	_players[player->playerId] = player;
	GConsoleLogger->WriteStdOut(Color::BLUE, L"player joined %d\n", player->playerId);
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
